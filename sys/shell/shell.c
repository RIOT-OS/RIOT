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

#include <sys/unistd.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <hash_string.h>
#include <shell.h>

static void handle_input_line(shell_t *shell, char* line) {
    char* saveptr;
    char* command = strtok_r(line, " ", &saveptr);

    void (*handler)(char*) = NULL;
    
    if (command) {
        handler = hashtable_search(shell->h, command);
        if (handler) {
            handler(line);
        } else {
            printf("shell: command \"%s\" not found.\n", command);
        }
    }
    
    free(line);
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
            handle_input_line(shell, strdup(line_buf));
        }
    }
}

void shell_init(shell_t *shell, int(*readchar)(void), void(*put_char)(int)) {
    shell->h = create_hashtable(16, (unsigned int (*)(void*)) hash_string, (int (*) (void*,void*)) cmp_string);
    shell->readchar = readchar;
    shell->put_char = put_char;
}

void shell_register_cmd(shell_t *shell, char* name, void (*handler)(char* args)) {
   hashtable_insert(shell->h, name, handler);
}

/** @} */
