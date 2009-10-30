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
#include <string.h>

#include "client.h"

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

	irc_callbacks_t	callbacks;
	memset(&callbacks, 0, sizeof(callbacks));

	irc_session_t *session = irc_create_session(&callbacks);
	if (!session)
		goto done;

	if (irc_connect(session, server, port, server_password, nick, username, realname))
		goto done;

	done:

	if (session)
		irc_destroy_session(session);

	LSErrorFree(&lserror);

	return retVal;

}
