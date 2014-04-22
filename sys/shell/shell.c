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
 * @ingroup     shell
 * @{
 */

/**
 * @file
 * @brief       Implementation of a very simple command interpreter.
 *              For each command (i.e. "echo"), a handler can be specified.
 *              If the first word of a user-entered command line matches the
 *              name of a handler, the handler will be called with the whole
 *              command line as parameter.
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "shell.h"
#include "shell_commands.h"

static shell_command_handler_t find_handler(const shell_command_t *command_list, char *command)
{
    const shell_command_t *command_lists[] = {
        command_list,
#ifdef MODULE_SHELL_COMMANDS
        _shell_command_list,
#endif
    };

    const shell_command_t *entry;

    /* iterating over command_lists */
    for (unsigned int i = 0; i < sizeof(command_lists) / sizeof(entry); i++) {
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
    for (unsigned int i = 0; i < sizeof(command_lists) / sizeof(entry); i++) {
        if ((entry = command_lists[i])) {
            /* iterating over commands in command_lists entry */
            while (entry->name != NULL) {
                printf("%-20s %s\n", entry->name, entry->desc);
                entry++;
            }
        }
    }
}

static int handle_input_line(shell_t *shell)
{
    /* first we need to calculate the number of arguments */
    unsigned argc = 0;
    char *pos = shell->buffer;
    int contains_esc_seq = 0;
    while (1) {
        if ((unsigned char) *pos > ' ') {
            /* found an argument */
            if (*pos == '"' || *pos == '\'') {
                /* it's a quoted argument */
                const char quote_char = *pos;
                do {
                    ++pos;
                    if (!*pos) {
                        return -1;
                    }
                    else if (*pos == '\\') {
                        /* skip over the next character */
                        ++contains_esc_seq;
                        ++pos;
                        if (!*pos) {
                            return -1;
                        }
                        continue;
                    }
                } while (*pos != quote_char);
                if ((unsigned char) pos[1] > ' ') {
                    return -1;
                }
            }
            else {
                /* it's an unquoted argument */
                do {
                    if (*pos == '\\') {
                        /* skip over the next character */
                        ++contains_esc_seq;
                        ++pos;
                        if (!*pos) {
                            return -1;
                        }
                    }
                    ++pos;
                    if (*pos == '"') {
                        return -1;
                    }
                } while ((unsigned char) *pos > ' ');
            }

            /* count the number of arguments we got */
            ++argc;
        }

        /* zero out the current position (space or quotation mark) and advance */
        if (*pos > 0) {
            *pos = 0;
            ++pos;
        }
        else {
            break;
        }
    }
    if (!argc) {
        return 0;
    }

    /* then we fill the argv array */
    char *argv[argc + 1];
    argv[argc] = NULL;
    pos = shell->buffer;
    for (unsigned i = 0; i < argc; ++i) {
        while (!*pos) {
            ++pos;
        }
        if (*pos == '"' || *pos == '\'') {
            ++pos;
        }
        argv[i] = pos;
        while (*pos) {
            ++pos;
        }
    }
    for (char **arg = argv; contains_esc_seq && *arg; ++arg) {
        for (char *c = *arg; *c; ++c) {
            if (*c != '\\') {
                continue;
            }
            for (char *d = c; *d; ++d) {
                *d = d[1];
            }
            if (--contains_esc_seq == 0) {
                break;
            }
        }
    }

    /* then we call the appropriate handler */
    shell_command_handler_t handler = find_handler(shell->command_list, argv[0]);
    if (handler != NULL) {
        handler(argc, argv);
    }
    else {
        if (strcmp("help", argv[0]) == 0) {
            print_help(shell->command_list);
        }
        else if (strcmp("quit", argv[0]) == 0) {
            return SHELL_EXIT;
        }
        else {
            puts("shell: command not found.");
        }
    }

    return 0;
}

static int readline(shell_t *shell)
{
    unsigned index = 0;
    while (1) {
        if (index + 1 >= shell->buffer_size) {
            puts("shell: command line to long.");
            return -1;
        }

        int c = shell->readchar();
        if (c < 0) {
            return SHELL_READ_ERROR;
        }
        else if (c >= 32) {
            /* do not print control characters */
            shell->put_char(c);
        }
        else if ((c == 0x03) || (c == 0x04) || (c == 0x1a) || (c == 0x1c)) {
            /* handle CTRL+C, CTRL+D, CTRL+Z, and CTRL+\, resp. */
            return SHELL_CTRL_C;
        }

        /* We allow Unix linebreaks (\n), DOS linebreaks (\r\n), and Mac linebreaks (\r). */
        /* QEMU transmits only a single '\r' == 13 on hitting enter ("-serial stdio"). */
        /* DOS newlines are handled like hitting enter twice, but handle_input_line() ignores empty lines. */
        if (c == 10 || c == 13) {
            shell->buffer[index] = '\0';
            shell->put_char('\r');
            shell->put_char('\n');
            return 0;
        }
        else {
            shell->buffer[index++] = c;
        }
    }
}

shell_run_result_t shell_run(shell_t *shell)
{
    for (const char *c = "Type \"quit\" to close the current shell.\r\n"; *c; ++c) {
        shell->put_char((unsigned char) *c);
    }

    int res;
    do {
        shell->put_char('>');
        shell->put_char(' ');

        res = readline(shell);
        if (res == 0) {
            res = handle_input_line(shell);
            if (res < 0) {
                puts("shell: incorrect quoting");
            }
        }
    } while (res <= 0);
    return res;
}

void shell_init(shell_t *shell, const shell_command_t *shell_commands,
                char *buffer, unsigned buffer_size, int(*readchar)(void), void(*put_char)(int))
{
    shell->command_list = shell_commands;
    shell->buffer = buffer;
    shell->buffer_size = buffer_size;
    shell->readchar = readchar;
    shell->put_char = put_char;
}

/** @} */
