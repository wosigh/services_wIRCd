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

#include "luna_service.h"
#include "client.h"

const char 		*dbusAddress 		= "us.ryanhope.wIRCd";

GMainLoop		*loop				= NULL;

LSMethod lsmethods[] = {
		{"client_connect",client_connect},
		{"client_cmd_msg",client_cmd_msg},
		{0,0}
};

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
	callbacks.event_part = dump_event;
	callbacks.event_mode = dump_event;
	callbacks.event_topic = dump_event;
	callbacks.event_kick = dump_event;
	callbacks.event_notice = dump_event;
	callbacks.event_invite = dump_event;
	callbacks.event_umode = dump_event;
	callbacks.event_ctcp_rep = dump_event;
	callbacks.event_ctcp_action = dump_event;
	callbacks.event_unknown = dump_event;

	wIRCd_clients = g_hash_table_new(g_str_hash, g_str_equal);
	if (wIRCd_clients)
		g_main_loop_run(loop);

}

void luna_service_cleanup() {

	if (wIRCd_clients)
		g_hash_table_destroy(wIRCd_clients);

}
