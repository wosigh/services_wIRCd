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

#ifndef CLIENT_H_
#define CLIENT_H_

#include <pthread.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define IN_BUILDING_LIBIRC
typedef int socket_t;
typedef pthread_mutex_t port_mutex_t;

#include <lunaservice.h>
#include <libircclient.h>
#include <libirc_session.h>

typedef struct {
	irc_session_t	*session;
	pthread_t		thread;
	LSMessage		*message_monolithic;
	const char 		*server;
	int			 	port;
	const char 		*server_password;
	const char 		*nick;
	const char 		*username;
	const char 		*realname;
	const char		*interface;
	int				estabilshed;
	char ip_addr[16];
	LSMessage		*msg_event_connect;
	LSMessage		*msg_event_nick;
	LSMessage		*msg_event_quit;
	LSMessage		*msg_event_join;
	LSMessage		*msg_event_part;
	LSMessage		*msg_event_mode;
	LSMessage		*msg_event_umode;
	LSMessage		*msg_event_topic;
	LSMessage		*msg_event_kick;
	LSMessage		*msg_event_channel;
	LSMessage		*msg_event_privmsg;
	LSMessage		*msg_event_notice;
	LSMessage		*msg_event_channel_notice;
	LSMessage		*msg_event_invite;
	LSMessage		*msg_event_ctcp_req;
	LSMessage		*msg_event_ctcp_rep;
	LSMessage		*msg_event_ctcp_action;
	LSMessage		*msg_event_unknown;
	LSMessage		*msg_event_numeric;
	LSMessage		*msg_event_dcc_chat_req;
	LSMessage		*msg_event_dcc_send_req;
} wIRCd_client_t;

GHashTable *wIRCd_clients;

irc_callbacks_t	callbacks;

void dump_event(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);

void handle_event_numeric(irc_session_t * session, unsigned int event, const char * origin, const char ** params, unsigned int count);

bool client_connect(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_msg(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_me(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_notice(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_join(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_part(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_invite(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_names(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_list(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_topic(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_channel_mode(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_kick(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_nick(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_quit(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_whois(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_user_mode(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_ping(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_away(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_cmd_disconnect(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_send_raw(LSHandle* lshandle, LSMessage *message, void *ctx);
bool client_get_version(LSHandle* lshandle, LSMessage *message, void *ctx);

// New subscriptions
bool sub_event_numeric(LSHandle* lshandle, LSMessage *message, void *ctx);

#endif /* CLIENT_H_ */
