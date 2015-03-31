/*
 * Copyright (C) 2009, Freie Universitaet Berlin (FUB).
 * Copyright (C) 2013, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell
 * @{
 *
 * @file
 * @brief       Implementation of a very simple command interpreter.
 *              For each command (i.e. "echo"), a handler can be specified.
 *              If the first word of a user-entered command line matches the
 *              name of a handler, the handler will be called with the whole
 *              command line as parameter.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "shell.h"
#include "shell_commands.h"
#include "shell_print.h"

#ifdef MODULE_READLINE
#include "readline.h"
#endif

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

static void handle_input_line(shell_t *shell, char *line)
{
    static const char *COMMAND_NOT_FOUND = "shell: command not found:";
    static const char *INCORRECT_QUOTING = "shell: incorrect quoting";

    /* first we need to calculate the number of arguments */
    unsigned argc = 0;
    char *pos = line;
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
                        shell_print_error(shell, INCORRECT_QUOTING);
                        shell_putchar(shell, '\n');
                        return;
                    }
                    else if (*pos == '\\') {
                        /* skip over the next character */
                        ++contains_esc_seq;
                        ++pos;
                        if (!*pos) {
                            shell_print_error(shell, INCORRECT_QUOTING);
                            shell_putchar(shell, '\n');
                            return;
                        }
                        continue;
                    }
                } while (*pos != quote_char);
                if ((unsigned char) pos[1] > ' ') {
                    shell_print_error(shell, INCORRECT_QUOTING);
                    shell_putchar(shell, '\n');
                    return;
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
                            shell_print_error(shell, INCORRECT_QUOTING);
                            shell_putchar(shell, '\n');
                            return;
                        }
                    }
                    ++pos;
                    if (*pos == '"') {
                        shell_print_error(shell, INCORRECT_QUOTING);
                        shell_putchar(shell, '\n');
                        return;
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
        return;
    }

    /* then we fill the argv array */
    char *argv[argc + 1];
    argv[argc] = NULL;
    pos = line;
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
        else {
            shell_print_error(shell, COMMAND_NOT_FOUND);
            shell_putchar(shell, '\n');
            shell_print(shell, argv[0]);
            shell_putchar(shell, '\n');
        }
    }
}

#ifndef MODULE_READLINE

/*
 * Return negative if no input is read.
 * Else return the number of characters read.
 */
static int readline(shell_t *shell, char *buf, size_t size)
{
    int cursor = 0;
    *buf = '\0';

    while (1) {
        if (cursor >= ((int) size) - 1) {
            return -1;
        }

        int c = shell->readchar();
        if (c < 0) {
            return -1;
        }

        /* We allow Unix linebreaks (\n), DOS linebreaks (\r\n), and Mac linebreaks (\r). */
        /* QEMU transmits only a single '\r' == 13 on hitting enter ("-serial stdio"). */
        /* DOS newlines are handled like hitting enter twice, but empty lines are ignored. */
        if (c == '\r' || c == '\n') {
            return cursor;
        }
        /* QEMU uses 0x7f (DEL) as backspace, while 0x08 (BS) is for most terminals */
        else if (c == 0x08 || c == 0x7f) {
            if (cursor == 0) {
                /* The line is empty. */
                continue;
            }
            buf[--cursor] = '\0';
            /* white-tape the character */
            shell->put_char('\b');
            shell->put_char(' ');
            shell->put_char('\b');
        }
        else if (c == '\t') {
            /* Tabulator */
            for (int i = 0; i < SHELL_TAB_SIZE; i++) {
                shell_echo_char(shell, ' ');
                buf[cursor++] = ' ';
            }
            continue;
        }
        else {
            // Any other character
            shell_echo_char(shell, c);
            buf[cursor++] = c;
            buf[cursor] = '\0';
        }
    }
}
#endif // MODULE_READLINE

void shell_run(shell_t *shell)
{
    char line_buf[shell->shell_buffer_size];

    shell_prompt(shell);

    while (1) {
        int res = readline(shell, line_buf, sizeof(line_buf));

        shell_putchar(shell, '\n');
        if (res > 0) {
            /* n characters were read. */
            handle_input_line(shell, line_buf);
            /* empty the line buffer */
            *line_buf = '\0';
        }
        shell_prompt(shell);
    }
}

void shell_init(shell_t *shell, const shell_command_t *shell_commands,
                uint16_t shell_buffer_size, int(*readchar)(void), void(*put_char)(int))
{
    shell->command_list = shell_commands;
    shell->shell_buffer_size = shell_buffer_size;
    shell->readchar = readchar;
    shell->put_char = put_char;
}
