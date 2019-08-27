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
#include <assert.h>
#include "shell.h"
#include "shell_commands.h"

#define ETX '\x03'  /** ASCII "End-of-Text", or ctrl-C */
#define BS  '\x08'  /** ASCII "Backspace" */
#define DEL '\x07'  /** ASCII "Delete" */

#ifdef MODULE_SHELL_COMMANDS
#define MORE_COMMANDS   _shell_command_list
#else
#define MORE_COMMANDS
#endif /* MODULE_SHELL_COMMANDS */

#ifdef MODULE_NEWLIB
#define flush_if_needed() fflush(stdout)
#else
#define flush_if_needed()
#endif /* MODULE_NEWLIB */

#ifndef SHELL_NO_ECHO
#define ECHO_ON 1
#else
#define ECHO_ON 0
#endif /* SHELL_NO_ECHO */

#ifndef SHELL_NO_PROMPT
#define PROMPT_CHAR '>'
#else
#define PROMPT_CHAR 0
#endif /* SHELL_NO_PROMPT */

struct shell_state {
    char echo_on;
    char prompt_char;
    char line[];
};

static int shell_state_init(struct shell_state *state, int len)
{
    assert(len >= sizeof(struct shell_state));

    state->echo_on = ECHO_ON;
    state->prompt_char = PROMPT_CHAR;

    return len - sizeof(struct shell_state);
}

/**
 * Code indicating that the line buffer size was exceeded.
 *
 * This definition ensures there's no collision with EOF.
 */
#define READLINE_TOOLONG (-EOF + 1)

static shell_command_handler_t find_handler(const shell_command_t *command_list, char *command)
{
    const shell_command_t *command_lists[] = {
        command_list,
        MORE_COMMANDS
    };

    /* iterating over command_lists */
    for (unsigned int i = 0; i < ARRAY_SIZE(command_lists); i++) {

        const shell_command_t *entry;

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
        MORE_COMMANDS
    };

    /* iterating over command_lists */
    for (unsigned int i = 0; i < ARRAY_SIZE(command_lists); i++) {

        const shell_command_t *entry;

        if ((entry = command_lists[i])) {
            /* iterating over commands in command_lists entry */
            while (entry->name != NULL) {
                printf("%-20s %s\n", entry->name, entry->desc);
                entry++;
            }
        }
    }
}

static void handle_input_line(const shell_command_t *command_list,
                              struct shell_state *state)
{
    static const char *INCORRECT_QUOTING = "shell: incorrect quoting";

    /* first we need to calculate the number of arguments */
    unsigned argc = 0;
    char *pos = state->line;
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
                        puts(INCORRECT_QUOTING);
                        return;
                    }
                    else if (*pos == '\\') {
                        /* skip over the next character */
                        ++contains_esc_seq;
                        ++pos;
                        if (!*pos) {
                            puts(INCORRECT_QUOTING);
                            return;
                        }
                        continue;
                    }
                } while (*pos != quote_char);
                if ((unsigned char) pos[1] > ' ') {
                    puts(INCORRECT_QUOTING);
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
                            puts(INCORRECT_QUOTING);
                            return;
                        }
                    }
                    ++pos;
                    if (*pos == '"') {
                        puts(INCORRECT_QUOTING);
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
    pos = state->line;
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
    shell_command_handler_t handler = find_handler(command_list, argv[0]);
    if (handler != NULL) {
        handler(argc, argv);
    }
    else {
        if (strcmp("@eon", argv[0]) == 0) {
            state->echo_on = 1;
        }
        else if (strcmp("@eoff", argv[0]) == 0) {
            state->echo_on = 0;
        }
        else if (strcmp("@ps", argv[0]) == 0) {
            state->prompt_char = (argc > 1)? argv[1][0] : 0;
        }
        else if (strcmp("help", argv[0]) == 0) {
            print_help(command_list);
        }
        else {
            printf("shell: command not found: %s\n", argv[0]);
        }
    }
}

static inline void print_prompt(struct shell_state *state)
{
    if (state->prompt_char) {
        putchar(state->prompt_char);
        putchar(' ');
    }

    flush_if_needed();
}

static inline void echo_char(struct shell_state *state, char c)
{
    if (state->echo_on) {
        putchar(c);
    }
}

static inline void white_tape(struct shell_state *state)
{
    if (state->echo_on) {
        putchar('\b');
        putchar(' ');
        putchar('\b');
    }
}

static inline void new_line(struct shell_state *state)
{
    if (state->echo_on) {
        putchar('\r');
        putchar('\n');
    }
}

/**
 * Read a single line from standard input into a buffer.
 *
 * In addition to copying characters, this routine echoes the line back to
 * stdout and also supports primitive line editing.
 *
 * If the input line is too long, the input will still be consumed until the end
 * to prevent the next line from containing garbage.
 *
 * We allow Unix (\n), DOS (\r\n), and Mac linebreaks (\r).
 * QEMU transmits only a single '\r' == 13 on hitting enter ("-serial stdio").
 * DOS newlines are handled like hitting enter twice.
 *
 * @param   state   Buffer where the input will be placed.
 * @param   size    Size of the buffer. The maximum line length will be one less
 *                  than size, to accommodate for the null terminator.
 *                  The minimum buffer size is 1.
 *
 * @return  length of the read line, excluding the terminator, if reading was
 *          successful.
 * @return  EOF, if the if the end of the input stream is reached.
 * @return  -READLINE_TOOLONG if the buffer size was exceededsys/shell
 */
static int readline(struct shell_state *state, size_t size)
{
    int curr_pos = 0;

    assert((size_t)size > 0);

    while (1) {
        int c = getchar();

        switch (c) {
            case EOF:
                return EOF;
            /* Ctrl-C cancels the current line. */
            case ETX:
                curr_pos = 0;
            /* fall-thru */
            case '\r':
            /* fall-thru */
            case '\n':
                new_line(state);

                if ((size_t)curr_pos < size) {
                    state->line[curr_pos] = '\0';
                    return curr_pos;
                } else {
                    return -READLINE_TOOLONG;
                }
            case BS:  /* QEMU uses 0x7f (DEL) as backspace, while 0x08 (BS) is for most terminals */
            /* fall-thru */
            case DEL:
                if (curr_pos > 0) {
                    --curr_pos;
                    if ((size_t)curr_pos < size) {
                        state->line[curr_pos] = '\0';
                    }
                    white_tape(state);
                }
                break;
            default:
                /* Always consume characters, but do not not always store them */
                if ((size_t)curr_pos < size - 1) {
                    state->line[curr_pos] = c;
                }
                curr_pos++;
                echo_char(state, c);
                break;
        }
        flush_if_needed();
    }
}

#define TOOLONG_MESSAGE "shell: maximum line length exceeded"

void shell_run(const shell_command_t *shell_commands, char *line_buf, int len)
{
    struct shell_state *state = (void*)line_buf;

    len = shell_state_init(state, len);

    while (1) {
        print_prompt(state);
        int res = readline(state, len);

        switch (res) {
            case EOF:
                goto shell_run_exit;
            case -READLINE_TOOLONG:
                puts(TOOLONG_MESSAGE);
                break;
            default:
                handle_input_line(shell_commands, state);
                break;
        }
    }

shell_run_exit:
    return;
}
