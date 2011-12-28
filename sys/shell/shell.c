/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of FeuerWare.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

/**
 * @ingroup	shell
 * @{
 */

/**
 * @file
 * @brief 		Implementation of a very simple command interpreter.
 *              For each command (i.e. "echo"), a handler can be specified.
 *              If the first word of a user-entered command line matches the
 *              name of a handler, the handler will be called with the whole
 *              command line as parameter.
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Kaspar Schleiser <kaspar@schleiser.de>
 */

//#include <sys/unistd.h>
#include <string.h>
#include <stdio.h>
#include <shell.h>
#include <shell_commands.h>
#include <stdint.h>
#include <stdlib.h>

static void(*find_handler(const shell_command_t *command_list, char *command))(char*) {
    const shell_command_t* entry = command_list;
	if (entry) {
			while (entry->name != NULL) {
				if ( strcmp(entry->name, command) == 0) {
					return entry->handler;
				} else {
					entry++;
				}
			}
	}
			
#ifdef MODULE_SHELL_COMMANDS
    entry = _shell_command_list;
    while (entry->name != NULL) {
        if ( strcmp(entry->name, command) == 0) {
            return entry->handler;
        } else {
            entry++;
        }
    }
#endif
    return NULL;
}

static void print_help(const shell_command_t *command_list) {
    const shell_command_t *entry = command_list;

    printf("%-20s %s\n", "Command", "Description");
    puts("---------------------------------------");

	if (entry) {
			while (entry->name != NULL) {
				printf("%-20s %s\n", entry->name, entry->desc);
				entry++;
			}
	}
    
#ifdef MODULE_SHELL_COMMANDS
    entry = _shell_command_list;
    while (entry->name != NULL) {
        printf("%-20s %s\n", entry->name, entry->desc);
        entry++;
    }
#endif
}

static void handle_input_line(shell_t *shell, char* line) {
    char* saveptr;
    char* linedup = strdup(line);
    char* command = strtok_r(linedup, " ", &saveptr);

    void (*handler)(char*) = NULL;
    
    if (command) {
        handler = find_handler(shell->command_list, command);
        if (handler != NULL) {
            handler(line);
        } else {
            if ( strcmp("help", command) == 0) {
                print_help(shell->command_list);
            } else {
                puts("shell: command not found.");
            }
        }
    }

    free(linedup);
}

int readline(shell_t *shell, char* buf, int size) {
    char *line_buf_ptr = buf;
    int c;

    while (1) {
        if ( (line_buf_ptr - buf) >= size-1) {
            return -1;
        }

        c = shell->readchar();
        shell->put_char(c);

        if (c == 13) continue;

        if (c == 10) {
            *line_buf_ptr = '\0';
            return 0;
        } else {
            *line_buf_ptr++ = c;
        }
    }
}

void shell_run(shell_t *shell) {
    char line_buf[255];

    while(1) {
        shell->put_char('>');
        int res = readline(shell, line_buf, sizeof(line_buf));
        if (! res ) {
            char* line_copy = strdup(line_buf);
            handle_input_line(shell, line_copy);
            free(line_copy);
        }
    }
}

void shell_init(shell_t *shell, const shell_command_t *shell_commands, int(*readchar)(void), void(*put_char)(int)) {
    shell->command_list = shell_commands;
    shell->readchar = readchar;
    shell->put_char = put_char;
}

/** @} */
