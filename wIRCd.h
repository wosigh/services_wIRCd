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

#ifndef WIRCD_H_
#define WIRCD_H_

#include "subscriptions.h"
#include "luna_service.h"
#include "client.h"
#include "events.h"

#define DEFAULT_MAX_RETRIES 10
#define DEFAULT_PRE_RUN_USLEEP 0
#define DEFAULT_DEBUG_LEVEL 0

int debug;
int max_retries;
int pre_run_usleep;

GHashTable *wIRCd_clients;

irc_callbacks_t	callbacks;

typedef enum {
	event_connect_,
	event_nick_,
	event_quit_,
	event_join_,
	event_part_,
	event_mode_,
	event_umode_,
	event_topic_,
	event_kick_,
	event_channel_,
	event_privmsg_,
	event_notice_,
	event_channel_notice_,
	event_invite_,
	event_ctcp_req_,
	event_ctcp_rep_,
	event_ctcp_action_,
	event_unknown_,
	event_numeric_,
} irc_callbacks;

#endif /* WIRCD_H_ */
