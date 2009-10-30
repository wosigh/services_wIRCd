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

#include <lunaservice.h>
#include <libircclient.h>

typedef struct {
	irc_session_t	*session;
	pthread_t		*thread;
	LSMessage		*message;
} wIRCd_client_t;

GHashTable *session_thread_table;

irc_callbacks_t	callbacks;

void dump_event(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);

bool client_connect(LSHandle* lshandle, LSMessage *message, void *ctx);

#endif /* CLIENT_H_ */
