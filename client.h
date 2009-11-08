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
	LSMessage		*message;
	const char 		*server;
	int			 	port;
	const char 		*server_password;
	const char 		*nick;
	const char 		*username;
	const char 		*realname;
	const char		*interface;
	int				estabilshed;
	char ip_addr[16];
} wIRCd_client_t;

GHashTable *wIRCd_clients;

irc_callbacks_t	callbacks;

void dump_event(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
void event_numeric(irc_session_t * session, unsigned int event, const char * origin, const char ** params, unsigned int count);

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

#endif /* CLIENT_H_ */
