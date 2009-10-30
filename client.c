/*=============================================================================
 Copyright (C) 2009 Ryan Hope <rmh3093@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 =============================================================================*/

#include <stdlib.h>
#include <pthread.h>

#include "luna_service.h"
#include "client.h"

bool join = true;

void *client_run(void *session) {
	irc_run(session);
}

void dump_event(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count) {

	char buf[512];
	int cnt;

	buf[0] = '\0';

	for ( cnt = 0; cnt < count; cnt++ ) {
		if ( cnt )
			strcat (buf, "|");

		strcat (buf, params[cnt]);
	}

	int len = 0;
	char *jsonResponse = 0;
	len = asprintf(&jsonResponse, "Event \"%s\", origin: \"%s\", params: %d [%s]", event, origin ? origin : "NULL", cnt, buf);

	if (jsonResponse) {
		LSError lserror;
		LSErrorInit(&lserror);
		LSMessage *message = (LSMessage*)irc_get_ctx(session);
		LSMessageReply(pub_serviceHandle,message,jsonResponse,&lserror);
		LSErrorFree(&lserror);
		free(jsonResponse);
	}

	if (join) {
		irc_cmd_join(session,"#webos-internals",0);
		join = false;
	}

}

bool client_connect(LSHandle* lshandle, LSMessage *message, void *ctx) {

	bool retVal = true;

	LSError lserror;
	LSErrorInit(&lserror);

	const char *server = 0;
	unsigned short port = 6667;
	const char *server_password = 0;
	const char *nick = 0;
	const char *username = 0;
	const char *realname = 0;

	json_t *object = LSMessageGetPayloadJSON(message);

	json_get_string(object, "server", &server);
	json_get_int(object, "port", port);
	json_get_string(object, "server_password", &server_password);
	json_get_string(object, "nick", &nick);
	json_get_string(object, "username", &username);
	json_get_string(object, "realname", &realname);

	if (!server) {
		LSMessageReply(lshandle,message,"{\"returnValue\":-1,\"errorText\":\"Server missing\"}",&lserror);
		goto done;
	} else if (!nick) {
		LSMessageReply(lshandle,message,"{\"returnValue\":-1,\"errorText\":\"Nick missing\"}",&lserror);
		goto done;
	}

	irc_session_t *session = irc_create_session(&callbacks);
	if (!session)
		goto done;

	if (irc_connect(session, server, port, server_password, nick, username, realname))
		goto done;


	LSMessageRef(message);
	irc_set_ctx(session,message);

	/*pthread_t thread;
    if (pthread_create(&thread, NULL, client_run, NULL))
    	LSMessageReply(lshandle,message,"{\"returnValue\":-1,\"errorText\":\"Failed to create thread\"}",&lserror);*/

	irc_run(session);

	done:

	if (session)
		irc_destroy_session(session);

	LSErrorFree(&lserror);

	return retVal;

}
