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
#include <stdio.h>
#include <getopt.h>

#include "wIRCd.h"

static struct option long_options[] = {
		{ "help", no_argument, 0, 'h' },
		{ "debug", no_argument, 0, 'D' },
		{ "version", no_argument, 0, 'V' },
		{ 0, 0, 0, 0 }
};

void print_version() {
	printf("wIRCd - WebOS IRC Daemon (%s)\n", VERSION);
}

void print_help(char *argv[]) {

	printf("Usage: %s [OPTION]...\n\n"
		"Miscellaneous:\n"
		"  -h, --help\t\tprint help information and exit\n"
		"  -D, --debug\t\tturn on debug output\n"
		"  -V, --version\t\tprint version information and exit\n", argv[0]);

}

int getopts(int argc, char *argv[]) {

	int c, retVal = 0;

	while (1) {
		int option_index = 0;
		c = getopt_long(argc, argv, "DVh", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'D':
			debug = 1;
			break;
		case 'V':
			print_version();
			retVal = 1;
			break;
		case 'h':
			print_help(argv);
			retVal = 1;
			break;
		case '?':
			print_help(argv);
			retVal = 1;
			break;
		default:
			abort();
		}
	}
	return retVal;

}

int main(int argc, char *argv[]) {

	if (getopts(argc, argv) == 1)
		return 1;

    if (luna_service_initialize())
            luna_service_start();

	return 0;

}
