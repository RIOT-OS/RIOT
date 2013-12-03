/**
 * Shell interpreter 
 *
 * Copyright (C) 2009, Freie Universitaet Berlin (FUB).
 * Copyright (C) 2013, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

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

#include <string.h>
#include <stdio.h>
#include <shell.h>
#include <shell_commands.h>
#include <stdint.h>
#include <stdlib.h>

static void(*find_handler(const shell_command_t *command_list, char *command))(char *)
{
    const shell_command_t *command_lists[] = {
        command_list,
#ifdef MODULE_SHELL_COMMANDS
        _shell_command_list,
#endif
    };

    const shell_command_t *entry;

    /* iterating over command_lists */
    for (unsigned int i = 0; i < sizeof(command_lists)/sizeof(entry); i++) {
        if ((entry = command_lists[i])) {
            /* iterating over commands in command_lists entry */
            while (entry->name != NULL) {
                if (strcmp(entry->name, command) == 0) {
                    return entry->handler;
                }
                else {
                    entry++;
                }
            }
        }
    }

    return NULL;
}

static void print_help(const shell_command_t *command_list)
{
    printf("%-20s %s\n", "Command", "Description");
    puts("---------------------------------------");

    const shell_command_t *command_lists[] = {
        command_list,
#ifdef MODULE_SHELL_COMMANDS
        _shell_command_list,
#endif
    };

    const shell_command_t *entry;

    /* iterating over command_lists */
    for (unsigned int i = 0; i < sizeof(command_lists)/sizeof(entry); i++) {
        if ((entry = command_lists[i])) {
            /* iterating over commands in command_lists entry */
            while (entry->name != NULL) {
                printf("%-20s %s\n", entry->name, entry->desc);
                entry++;
            }
        }
    }
}

static void handle_input_line(shell_t *shell, char *line)
{
    char line_copy[SHELL_BUFFER_SIZE];
    char *saveptr;
    strncpy(line_copy, line, sizeof(line_copy));
    char *command = strtok_r(line_copy, " ", &saveptr);

    void (*handler)(char *) = NULL;

    if (command) {
        handler = find_handler(shell->command_list, command);

        if (handler != NULL) {
            handler(line);
        }
        else {
            if (strcmp("help", command) == 0) {
                print_help(shell->command_list);
            }
            else {
                puts("shell: command not found.");
            }
        }
    }
}

static int readline(shell_t *shell, char *buf, size_t size)
{
    char *line_buf_ptr = buf;
    int c;

    while (1) {
        if ((line_buf_ptr - buf) >= ((int) size) - 1) {
            return -1;
        }

        c = shell->readchar();
        shell->put_char(c);

        if (c == 13) {
            continue;
        }

        if (c == 10) {
            *line_buf_ptr = '\0';
            return 0;
        }
        else {
            *line_buf_ptr++ = c;
        }
    }

    return 1;
}

static inline void print_prompt(shell_t *shell)
{
    shell->put_char('>');
    shell->put_char(' ');
    return;
}

void shell_run(shell_t *shell)
{
    char line_buf[SHELL_BUFFER_SIZE];

    print_prompt(shell);

    while (1) {
        int res = readline(shell, line_buf, sizeof(line_buf));

        if (!res) {
            handle_input_line(shell, line_buf);
        }

        print_prompt(shell);
    }
}

void shell_init(shell_t *shell, const shell_command_t *shell_commands, int(*readchar)(void), void(*put_char)(int))
{
    shell->command_list = shell_commands;
    shell->readchar = readchar;
    shell->put_char = put_char;
}

/** @} */
