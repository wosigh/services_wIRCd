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

#include <string.h>
#include <glib.h>

#include "wIRCd.h"

const char 		*dbusAddress 		= "us.ryanhope.wIRCd";

GMainLoop		*loop				= NULL;

LSMethod lsmethods[] = {
		// Connection subscription
		{"client_connect",client_connect},
		// Message methods
		{"client_cmd_msg",client_cmd_msg},
		{"client_cmd_me",client_cmd_me},
		{"client_cmd_notice",client_cmd_notice},
		// Channel methods
		{"client_cmd_join",client_cmd_join},
		{"client_cmd_part",client_cmd_part},
		{"client_cmd_invite",client_cmd_invite},
		{"client_cmd_names",client_cmd_names},
		{"client_cmd_list",client_cmd_list},
		{"client_cmd_topic",client_cmd_topic},
		{"client_cmd_channel_mode",client_cmd_channel_mode},
		{"client_cmd_kick",client_cmd_kick},
		// Misc methods
		{"client_cmd_nick",client_cmd_nick},
		{"client_cmd_quit",client_cmd_quit},
		{"client_cmd_whois",client_cmd_whois},
		{"client_cmd_user_mode",client_cmd_user_mode},
		// Custom methods
		{"client_cmd_ping",client_cmd_ping},
		{"client_cmd_away",client_cmd_away},
		{"client_cmd_disconnect",client_cmd_disconnect},
		// Raw
		{"client_send_raw",client_send_raw},
		// Random info
		{"client_get_version",client_get_version},
		{0,0}
};

bool connectionManagerHandler(LSHandle *sh, LSMessage *reply, void *ctx) {
	g_message("%s",LSMessageGetPayload(reply));
	return true;
}

bool luna_service_initialize() {

	bool retVal = FALSE;

	LSError lserror;
	LSErrorInit(&lserror);

	loop = g_main_loop_new(NULL, FALSE);
	if (loop==NULL)
		goto end;

	retVal = LSRegisterPalmService(dbusAddress, &serviceHandle, &lserror);
	if (retVal) {
		pub_serviceHandle = LSPalmServiceGetPublicConnection(serviceHandle);
		priv_serviceHandle = LSPalmServiceGetPrivateConnection(serviceHandle);
	} else
		goto end;

	retVal = LSPalmServiceRegisterCategory(serviceHandle, "/", lsmethods,
			NULL, NULL, NULL, &lserror);
	if (retVal)
		retVal = LSGmainAttachPalmService(serviceHandle, loop, &lserror);

	/*LSCall(pub_serviceHandle, "luna://com.palm.connectionmanager/getstatus",
        "{\"subscribe\":true}", connectionManagerHandler, NULL, NULL, &lserror);*/

	end: if (LSErrorIsSet(&lserror)) {
		LSErrorPrint(&lserror, stderr);
		LSErrorFree(&lserror);
	}

	return retVal;

}

void luna_service_start() {

	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.event_connect = dump_event;
	callbacks.event_nick = dump_event;
	callbacks.event_quit = dump_event;
	callbacks.event_join = dump_event;
	callbacks.event_part = dump_event;
	callbacks.event_mode = dump_event;
	callbacks.event_umode = dump_event;
	callbacks.event_topic = dump_event;
	callbacks.event_kick = dump_event;
	callbacks.event_channel = dump_event;
	callbacks.event_privmsg = dump_event;
	callbacks.event_notice = dump_event;
	callbacks.event_channel_notice = dump_event;
	callbacks.event_invite = dump_event;
	callbacks.event_ctcp_req = dump_event;
	callbacks.event_ctcp_rep = dump_event;
	callbacks.event_ctcp_action = dump_event;
	callbacks.event_unknown = dump_event;
	callbacks.event_numeric = handle_event_numeric;

	wIRCd_clients = g_hash_table_new(g_str_hash, g_str_equal);
	if (wIRCd_clients)
		g_main_loop_run(loop);

}

void luna_service_cleanup() {

	if (wIRCd_clients)
		g_hash_table_destroy(wIRCd_clients);

}
