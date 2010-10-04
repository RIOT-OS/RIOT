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

#ifndef __SHELL_H
#define __SHELL_H 

/**
 * @defgroup shell Simple Shell Interpreter
 * @ingroup feuerware
 */

//#include "hashtable.h"

typedef struct shell_commant_t {
    char* name;
    void (*handler)(char*);
} shell_command_t;

typedef struct shell_t {
    const shell_command_t *command_list;
    int (*readchar)(void); 
    void (*put_char)(int); 
} shell_t;

/**
 * @brief Initialize a shell object
 */
void shell_init(shell_t *shell, int(*read_char)(void), void (*put_char)(int));

/**
 * @brief Register a new command handler for a shell.
 * @param shell Shell object.
 * @param name Name of the command to register.
 * @param handler Function pointer to handler that takes the complete command line as parameter.
 */
//void shell_register_cmd(shell_t *shell, char* name, void (*handler)(char* args));

/**
 * @brief Endless loop that waits for command and executes handler.
 */
void shell_run(shell_t *shell);


void shell_auto_init(shell_t *shell);

#endif /* __SHELL_H */
