/*
 * Copyright (C) 2009, Freie Universitaet Berlin (FUB).
 * Copyright (C) 2013, INRIA.
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
#include <stdbool.h>
#include "shell.h"
#include "shell_commands.h"

#define ETX '\x03'  /** ASCII "End-of-Text", or ctrl-C */
#if !defined(SHELL_NO_ECHO) || !defined(SHELL_NO_PROMPT)
#ifdef MODULE_NEWLIB
/* use local copy of putchar, as it seems to be inlined,
 * enlarging code by 50% */
static void _putchar(int c) {
    putchar(c);
}
#else
#define _putchar putchar
#endif
#endif

#ifdef MODULE_SHELL_COMMANDS
#define _use_builtin_cmds true
#else
#define _use_builtin_cmds false
#endif

static void flush_if_needed(void)
{
#ifdef MODULE_NEWLIB
    fflush(stdout);
#endif
}

static shell_command_handler_t search_commands(const shell_command_t *entry,
                                               char *command)
{
    for (; entry->name != NULL; entry++) {
        if (strcmp(entry->name, command) == 0) {
            return entry->handler;
        }
    }
    return NULL;
}

static shell_command_handler_t find_handler(const shell_command_t *command_list, char *command)
{
    shell_command_handler_t handler = NULL;

    if (command_list != NULL) {
        handler = search_commands(command_list, command);
    }

    if (handler == NULL && _use_builtin_cmds) {
        handler = search_commands(_shell_command_list, command);
    }

    return handler;
}

static void print_commands(const shell_command_t *entry)
{
    for (; entry->name != NULL; entry++) {
        printf("%-20s %s\n", entry->name, entry->desc);
    }
}

static void print_help(const shell_command_t *command_list)
{
    puts("Command              Description\n---------------------------------------");

    if (command_list != NULL) {
        print_commands(command_list);
    }

    if (_use_builtin_cmds) {
        print_commands(_shell_command_list);
    }
}

enum PARSE_STATE {
    PARSE_SPACE,
    PARSE_UNQUOTED,
    PARSE_SINGLEQUOTE,
    PARSE_DOUBLEQUOTE,
    PARSE_ESCAPE_MASK,
    PARSE_UNQUOTED_ESC,
    PARSE_SINGLEQUOTE_ESC,
    PARSE_DOUBLEQUOTE_ESC,
};

static enum PARSE_STATE escape_toggle(enum PARSE_STATE s)
{
    return s ^ PARSE_ESCAPE_MASK;
}

#define SQUOTE '\''
#define DQUOTE '"'
#define ESCAPECHAR '\\'
#define BLANK ' '

static void handle_input_line(const shell_command_t *command_list, char *line)
{
    static const char *INCORRECT_QUOTING = "shell: incorrect quoting";

    /* first we need to calculate the number of arguments */
    int argc = 0;
    char *readpos = line;
    char *writepos = readpos;
    enum PARSE_STATE pstate = PARSE_SPACE;

    while (*readpos != '\0') {
        char wordbreak;

        switch (pstate) {
            case PARSE_SPACE:
                if (*readpos != BLANK) {
                    argc++;
                }
                if (*readpos == SQUOTE) {
                    pstate = PARSE_SINGLEQUOTE;
                } else if (*readpos == DQUOTE) {
                    pstate = PARSE_DOUBLEQUOTE;
                } else if (*readpos == ESCAPECHAR) {
                    pstate = PARSE_UNQUOTED_ESC;
                } else if (*readpos != BLANK) {
                    pstate = PARSE_UNQUOTED;
                    break;
                }
                goto parse_end;
            case PARSE_UNQUOTED:
                wordbreak = BLANK;
                goto break_word;
            case PARSE_SINGLEQUOTE:
                wordbreak = SQUOTE;
                goto break_word;
            case PARSE_DOUBLEQUOTE:
                wordbreak = DQUOTE;
break_word:
                if (*readpos == wordbreak) {
                    pstate = PARSE_SPACE;
                    *writepos++ = '\0';
                } else if (*readpos == ESCAPECHAR) {
                    pstate = escape_toggle(pstate);
                } else {
                    break;
                }
                goto parse_end;
            default: /* QUOTED state */
                pstate = escape_toggle(pstate);
                break;
        }
        *writepos++ = *readpos;
parse_end:
        readpos++;
    }

    *writepos = '\0';

    if (pstate != PARSE_SPACE && pstate != PARSE_UNQUOTED) {
        puts(INCORRECT_QUOTING);
        return;
    }

    if (argc == 0) {
        return;
    }

    /* then we fill the argv array */
    int collected;
    char *argv[argc];

    readpos = line;
    for (collected = 0; collected < argc; collected++) {
        argv[collected] = readpos;
        readpos += strlen(readpos) + 1;
    }

    /* then we call the appropriate handler */
    shell_command_handler_t handler = find_handler(command_list, argv[0]);
    if (handler != NULL) {
        handler(argc, argv);
    }
    else {
        if (strcmp("help", argv[0]) == 0) {
            print_help(command_list);
        }
        else {
            printf("shell: command not found: %s\n", argv[0]);
        }
    }
}

static int readline(char *buf, size_t size)
{
    char *line_buf_ptr = buf;

    while (1) {
        if ((line_buf_ptr - buf) >= ((int) size) - 1) {
            return -1;
        }

        int c = getchar();
        if (c < 0) {
            return EOF;
        }

        /* We allow Unix linebreaks (\n), DOS linebreaks (\r\n), and Mac linebreaks (\r). */
        /* QEMU transmits only a single '\r' == 13 on hitting enter ("-serial stdio"). */
        /* DOS newlines are handled like hitting enter twice, but empty lines are ignored. */
        /* Ctrl-C cancels the current line. */
        if (c == '\r' || c == '\n' || c == ETX) {
            *line_buf_ptr = '\0';
#ifndef SHELL_NO_ECHO
            _putchar('\r');
            _putchar('\n');
#endif

            /* return 1 if line is empty, 0 otherwise */
            return c == ETX || line_buf_ptr == buf;
        }
        /* QEMU uses 0x7f (DEL) as backspace, while 0x08 (BS) is for most terminals */
        else if (c == 0x08 || c == 0x7f) {
            if (line_buf_ptr == buf) {
                /* The line is empty. */
                continue;
            }

            *--line_buf_ptr = '\0';
            /* white-tape the character */
#ifndef SHELL_NO_ECHO
            _putchar('\b');
            _putchar(' ');
            _putchar('\b');
#endif
        }
        else {
            *line_buf_ptr++ = c;
#ifndef SHELL_NO_ECHO
            _putchar(c);
#endif
        }
        flush_if_needed();
    }
}

static inline void print_prompt(void)
{
#ifndef SHELL_NO_PROMPT
    _putchar('>');
    _putchar(' ');
#endif

    flush_if_needed();
}

void shell_run(const shell_command_t *shell_commands, char *line_buf, int len)
{
    print_prompt();

    while (1) {
        int res = readline(line_buf, len);

        if (res == EOF) {
            break;
        }

        if (!res) {
            handle_input_line(shell_commands, line_buf);
        }

        print_prompt();
    }
}
