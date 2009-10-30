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

#include "client.h"

bool client_create_session(LSHandle* lshandle, LSMessage *message, void *ctx) {

	bool retVal = true;

	LSError lserror;
	LSErrorInit(&lserror);

	int len = 0;
	char *jsonResponse = 0;

	const char *token = LSMessageGetUniqueToken(message);
	irc_session_t *session = irc_create_session(NULL);
	if (token && session)
		g_hash_table_insert(client_sessions, token, session);

	len = asprintf(&jsonResponse,"{\"returnValue\":0,\"sessionToken\":\"%s\"}",token);
	if (jsonResponse) {
		LSMessageReply(lshandle,message,jsonResponse,&lserror);
		free(jsonResponse);
	} else
		LSMessageReply(lshandle,message,"{\"returnValue\":-1,\"errorText\":\"Generic error\"}",&lserror);

	LSErrorFree(&lserror);

	return retVal;

}
