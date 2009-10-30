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

void *client_run(void *sessionToken) {

	LSError lserror;
	LSErrorInit(&lserror);

	wIRCd_client_t *client = (wIRCd_client_t*)g_hash_table_lookup(session_thread_table, (char*)sessionToken);

	client->server = 0;
	client->port = 6667;
	client->server_password = 0;
	client->nick = 0;
	client->username = 0;
	client->realname = 0;

	json_t *object = LSMessageGetPayloadJSON(client->message);

	// Basic connection info
	json_get_string(object, "server", &client->server); // Required
	json_get_int(object, "port", client->port);

	// Server related connection info
	json_get_string(object, "username", &client->username);
	json_get_string(object, "server_password", &client->server_password);

	// Basic user info
	json_get_string(object, "nick", &client->nick); // Required
	json_get_string(object, "realname", &client->realname);

	if (!client->server) {
		LSMessageReply(pub_serviceHandle,client->message,"{\"returnValue\":-1,\"errorText\":\"Server missing\"}",&lserror);
		goto done;
	} else if (!client->nick) {
		LSMessageReply(pub_serviceHandle,client->message,"{\"returnValue\":-1,\"errorText\":\"Nick missing\"}",&lserror);
		goto done;
	}

	client->session = irc_create_session(&callbacks);

	if (!client->session)
		goto done;

	if (irc_connect(client->session, client->server, client->port,
			client->server_password, client->nick, client->username, client->realname))
		goto done;

	irc_set_ctx(client->session,(void*)sessionToken);

	irc_run(client->session);

	done:

	if (client->session)
		irc_destroy_session(client->session);

	LSErrorFree(&lserror);

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
		wIRCd_client_t *client = (wIRCd_client_t*)g_hash_table_lookup(session_thread_table, (char*)irc_get_ctx(session));
		LSMessageReply(pub_serviceHandle,client->message,jsonResponse,&lserror);
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

	LSMessageRef(message);

	wIRCd_client_t *client = malloc(sizeof(wIRCd_client_t));
	client->message = message;

	const char* sessionToken = LSMessageGetUniqueToken(message)+1;

	g_hash_table_insert(session_thread_table, (gpointer)sessionToken, (gpointer)client);

    if (pthread_create(&client->thread, NULL, client_run, (void*)sessionToken)) {
    	LSMessageReply(lshandle,message,"{\"returnValue\":-1,\"errorText\":\"Failed to create thread\"}",&lserror);
    	retVal = false;
    }

	LSErrorFree(&lserror);

	return retVal;

}
