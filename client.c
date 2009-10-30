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

typedef enum {
	msg_,
	me_,
	notice_,
	join_,
	part_,
	invite_,
	names_,
	list_,
	topic_,
	channel_mode_,
	kick_,
	nick_,
	quit_,
	whois_,
} irc_cmd;

void *client_run(void *sessionToken) {

	g_message("sessionToken : %s", (char*)sessionToken);

	LSError lserror;
	LSErrorInit(&lserror);

	wIRCd_client_t *client = (wIRCd_client_t*)g_hash_table_lookup(wIRCd_clients, (char*)sessionToken);

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

	LSMessageReply(pub_serviceHandle,client->message,"{\"returnValue\":0}",&lserror);
	LSMessageUnref(client->message);

	done:

	if (client->session)
		irc_destroy_session(client->session);

	g_hash_table_remove(wIRCd_clients, (gconstpointer)sessionToken);

	if (client)
		free(client);

	LSErrorFree(&lserror);

}

void dump_event(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count) {

	char *sessionToken = (char*)irc_get_ctx(session);

	char buf[1024];
	int cnt;

	buf[0] = '\0';

	for (cnt = 0; cnt < count; cnt++) {
		if (cnt)
			strcat(buf, ",");
		strcat(buf, "\"");
		strcat(buf, params[cnt]);
		strcat(buf, "\"");
	}

	int len = 0;
	char *jsonResponse = 0;
	len = asprintf(&jsonResponse, "{\"sessionToken\":\"%s\",\"event\":\"%s\",\"origin\":\"%s\",\"params\":[%s]}", sessionToken, event, origin ? origin : "NULL", buf);

	if (jsonResponse) {
		LSError lserror;
		LSErrorInit(&lserror);
		wIRCd_client_t *client = (wIRCd_client_t*)g_hash_table_lookup(wIRCd_clients, sessionToken);
		LSMessageReply(pub_serviceHandle,client->message,jsonResponse,&lserror);
		LSErrorFree(&lserror);
		free(jsonResponse);
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

	g_hash_table_insert(wIRCd_clients, (gpointer)sessionToken, (gpointer)client);

	if (pthread_create(&client->thread, NULL, client_run, (void*)sessionToken)) {
		LSMessageReply(lshandle,message,"{\"returnValue\":-1,\"errorText\":\"Failed to create thread\"}",&lserror);
		retVal = false;
	}

	LSErrorFree(&lserror);

	return retVal;

}

bool process_command(LSHandle* lshandle, LSMessage *message, irc_cmd type) {

	bool retVal = true;

	LSError lserror;
	LSErrorInit(&lserror);

	json_t *object = LSMessageGetPayloadJSON(message);

	char *sessionToken = 0;
	char *nch = 0;
	char *text = 0;
	char *channel = 0;
	char *key = 0;
	char *nick = 0;
	char *topic = 0;
	char *reason = 0;
	char *mode = 0;

	json_get_string(object, "sessionToken", &sessionToken);
	json_get_string(object, "nch", &nch);
	json_get_string(object, "text", &text);
	json_get_string(object, "channel", &channel);
	json_get_string(object, "key", &key);
	json_get_string(object, "nick", &nick);
	json_get_string(object, "topic", &topic);
	json_get_string(object, "reason", &reason);
	json_get_string(object, "mode", &mode);

	if (!sessionToken)
		goto done;

	/*if (type==msg_||type==me_||type==notice_) {
		if (!nch || !text)
			goto done;
	}

	if (type==join_||type==invite_||type==topic_||type==kick_||type==part_||type==names_||type==list_||type==channel_mode_) {
		if (!channel)
			goto done;
	}

	if (type==join_) {
		if (!key)
			goto done;
	}

	if (type==invite_ || type==kick_) {
		if (!nick)
			goto done;
	}

	if (type==topic_) {
		if (!topic)
			goto done;
	}

	if (type==kick_) {
		if (!reason)
			goto done;
	}

	if (type==channel_mode_) {
		if (!mode)
			goto done;
	}*/

	wIRCd_client_t *client = (wIRCd_client_t*)g_hash_table_lookup(wIRCd_clients, sessionToken);
	if (client) {
		int retVal = -1;
		switch (type) {
		case msg_: retVal = irc_cmd_msg(client->session, nch, text); break;
		case me_: retVal = irc_cmd_me(client->session, nch, text); break;
		case notice_: retVal = irc_cmd_notice(client->session, nch, text); break;
		case join_: retVal = irc_cmd_join(client->session, channel, key); break;
		case part_: retVal = irc_cmd_part(client->session, channel); break;
		case invite_: retVal = irc_cmd_invite(client->session, nick, channel); break;
		case names_: retVal = irc_cmd_names(client->session, channel); break;
		case list_: retVal = irc_cmd_list(client->session, channel); break;
		case topic_: retVal = irc_cmd_topic(client->session, channel, topic); break;
		case channel_mode_: retVal = irc_cmd_channel_mode(client->session, channel, mode); break;
		case kick_: retVal = irc_cmd_kick(client->session, nick, channel, reason); break;
		case nick_: retVal = irc_cmd_nick(client->session, nick); break;
		case quit_: retVal = irc_cmd_quit(client->session, reason); break;
		case whois_: retVal = irc_cmd_whois(client->session, nick); break;
		}
		char *jsonResponse = 0;
		int len = 0;
		len = asprintf(&jsonResponse, "{\"returnValue\":%d}", retVal);
		if (jsonResponse) {
			LSMessageReply(lshandle,message,jsonResponse,&lserror);
			free(jsonResponse);
		} else
			LSMessageReply(lshandle,message,"{\"returnValue\":-1,\"errorText\":\"Generic error\"}",&lserror);
	} else
		LSMessageReply(lshandle,message,"{\"returnValue\":-1,\"errorText\":\"Invalid sessionToken\"}",&lserror);

	done:

	LSErrorFree(&lserror);

	return retVal;

}


bool client_cmd_msg(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, msg_);
}

bool client_cmd_me(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, me_);
}

bool client_cmd_notice(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, notice_);
}

bool client_cmd_join(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, join_);
}

bool client_cmd_part(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, part_);
}

bool client_cmd_invite(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, invite_);
}

bool client_cmd_names(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, names_);
}

bool client_cmd_list(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, list_);
}

bool client_cmd_topic(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, topic_);
}

bool client_cmd_channel_mode(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, channel_mode_);
}

bool client_cmd_kick(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, kick_);
}

bool client_cmd_nick(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, nick_);
}

bool client_cmd_quit(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, quit_);
}

bool client_cmd_whois(LSHandle* lshandle, LSMessage *message, void *ctx) {
	return process_command(lshandle, message, whois_);
}
