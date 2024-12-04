/*
 * Copyright (C) 2009, 2020 Freie Universität Berlin
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
 * @brief       Implementation of a simple command interpreter.
 *              For each command (i.e. "echo"), a handler can be specified.
 *              If the first word of a user-entered command line matches the
 *              name of a handler, the handler will be called with the remaining
 *              arguments passed in a manner similar to `main()`'s argc/argv
 *              parameters.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>

#include "xfa.h"
#include "shell.h"
#include "shell_lock.h"

#ifdef MODULE_VFS
#include <fcntl.h>
#include "vfs.h"
#endif

/* define shell command cross file array */
XFA_INIT_CONST(shell_command_xfa_t, shell_commands_xfa_v2);

#define ETX '\x03'  /** ASCII "End-of-Text", or Ctrl-C */
#define EOT '\x04'  /** ASCII "End-of-Transmission", or Ctrl-D */
#define BS  '\x08'  /** ASCII "Backspace" */
#define DEL '\x7f'  /** ASCII "Delete" */

#if defined(MODULE_NEWLIB) || defined(MODULE_PICOLIBC)
    #define flush_if_needed() fflush(stdout)
#else
    #define flush_if_needed()
#endif /* MODULE_NEWLIB || MODULE_PICOLIBC */

#define SQUOTE '\''
#define DQUOTE '"'
#define ESCAPECHAR '\\'
#define SPACE ' '
#define TAB '\t'

#define PARSE_ESCAPE_MASK 0x4;

extern void shell_lock_checkpoint(char *line_buf, int len);
extern bool shell_lock_is_locked(void);
extern void shell_lock_auto_lock_refresh(void);

enum parse_state {
    PARSE_BLANK             = 0x0,

    PARSE_UNQUOTED          = 0x1,
    PARSE_SINGLEQUOTE       = 0x2,
    PARSE_DOUBLEQUOTE       = 0x3,

    PARSE_UNQUOTED_ESC      = 0x5,
    PARSE_SINGLEQUOTE_ESC   = 0x6,
    PARSE_DOUBLEQUOTE_ESC   = 0x7,
};

static enum parse_state escape_toggle(enum parse_state s)
{
    return s ^ PARSE_ESCAPE_MASK;
}

static shell_command_handler_t search_commands(const shell_command_t *entry,
                                               const char *command)
{
    for (; entry->name != NULL; entry++) {
        if (strcmp(entry->name, command) == 0) {
            return entry->handler;
        }
    }
    return NULL;
}

static shell_command_handler_t search_commands_xfa(const char *command)
{
    unsigned n = XFA_LEN(shell_command_t, shell_commands_xfa_v2);

    for (unsigned i = 0; i < n; i++) {
        const volatile shell_command_xfa_t *entry = &shell_commands_xfa_v2[i];
        if (flash_strcmp(command, entry->name) == 0) {
            return entry->handler;
        }
    }
    return NULL;
}

shell_command_handler_t shell_find_handler(const shell_command_t *command_list,
                                           const char *command)
{
    shell_command_handler_t handler = NULL;

    assert(command);

    if (command_list != NULL) {
        handler = search_commands(command_list, command);
    }

    if (handler == NULL) {
        handler = search_commands_xfa(command);
    }

    return handler;
}

static void print_commands_json(const shell_command_t *cmd_list)
{
    bool first = true;

    printf("{\"cmds\": [");

    if (cmd_list) {
        for (const shell_command_t *entry = cmd_list; entry->name != NULL; entry++) {
            if (first) {
                first = false;
            }
            else {
                printf(", ");
            }
            printf("{\"cmd\": \"%s\", \"desc\": \"%s\"}", entry->name, entry->desc);
        }
    }

    unsigned n = XFA_LEN(shell_command_xfa_t, shell_commands_xfa_v2);
    for (unsigned i = 0; i < n; i++) {
        if (first) {
            first = false;
        }
        else {
            printf(", ");
        }
        const volatile shell_command_xfa_t *entry = &shell_commands_xfa_v2[i];
        printf("{\"cmd\": \"%s\", \"desc\": \"%s\"}", entry->name, entry->desc);
    }
    puts("]}");
}

static void print_commands(const shell_command_t *entry)
{
    for (; entry->name != NULL; entry++) {
        printf("%-20s %s\n", entry->name, entry->desc);
    }
}

static void print_commands_xfa(void)
{
    unsigned n = XFA_LEN(shell_command_xfa_t, shell_commands_xfa_v2);
    for (unsigned i = 0; i < n; i++) {
        const volatile shell_command_xfa_t *entry = &shell_commands_xfa_v2[i];
        printf("%-20" PRIsflash " %" PRIsflash "\n",
                     entry->name, entry->desc);
    }
}

static void print_help(const shell_command_t *command_list)
{
    printf("Command              Description\n"
           "---------------------------------------\n");
    if (command_list != NULL) {
        print_commands(command_list);
    }

    print_commands_xfa();
}

/**
 * Break input line into words, create argv and call the command handler.
 *
 * Words are broken up at spaces. A backslash escapes the character that comes
 * after (meaning if it is taken literally and if it is a space it does not break
 * the word). Spaces can also be protected by quoting with double or single
 * quotes.
 *
 * There are two unquoted states (PARSE_BLANK and PARSE_UNQUOTED) and two quoted
 * states (PARSE_SINGLEQUOTE and PARSE_DOUBLEQUOTE). In addition, every state
 * (except PARSE_BLANK) has an escaped pair state (e.g PARSE_SINGLEQUOTE and
 * PARSE_SINGLEQUOTE_ESC).
 *
 * For the following let's define some things
 *      - Function transit(character, state) to change to 'state' after
 *        'character' was read. The order of a list of transit-functions matters.
 *      - A BLANK is either SPACE or TAB
 *      - '*' means any character
 *
 *      PARSE_BLANK
 *          transit(SQUOTE, PARSE_SINGLEQUOTE)
 *          transit(DQUOTE, PARSE_DOUBLEQUOTE)
 *          transit(ESCAPECHAR, PARSE_UNQUOTED_ESC)
 *          transit(BLANK, PARSE_BLANK)
 *          transit(*, PARSE_UNQUOTED) -> store character
 *
 *      PARSE_UNQUOTED
 *          transit(SQUOTE, PARSE_SINGLEQUOTE)
 *          transit(DQUOTE, PARSE_DOUBLEQUOTE)
 *          transit(BLANK, PARSE_BLANK)
 *          transit(ESCAPECHAR, PARSE_UNQUOTED_ESC)
 *          transit(*, PARSE_UNQUOTED) -> store character
 *
 *      PARSE_UNQUOTED_ESC
 *          transit(*, PARSE_UNQUOTED) -> store character
 *
 *      PARSE_SINGLEQUOTE
 *          transit(SQUOTE, PARSE_UNQUOTED)
 *          transit(ESCAPECHAR, PARSE_SINGLEQUOTE_ESC)
 *          transit(*, PARSE_SINGLEQUOTE) -> store character
 *
 *      PARSE_SINGLEQUOTE_ESC
 *          transit(*, PARSE_SINGLEQUOTE) -> store character
 *
 *      PARSE_DOUBLEQUOTE
 *          transit(DQUOTE, PARSE_UNQUOTED)
 *          transit(ESCAPECHAR, PARSE_DOUBLEQUOTE_ESC)
 *          transit(*, PARSE_DOUBLEQUOTE) -> store character
 *
 *      PARSE_DOUBLEQUOTE_ESC
 *          transit(*, PARSE_DOUBLEQUOTE) -> store character
 *
 *
 */
int shell_handle_input_line(const shell_command_t *command_list, char *line)
{
    /* first we need to calculate the number of arguments */
    int argc = 0;
    char *readpos = line;
    char *writepos = readpos;

    uint8_t pstate = PARSE_BLANK;
    if (IS_USED(MODULE_SHELL_HOOKS)) {
        shell_post_readline_hook();
    }
    for (; *readpos != '\0'; readpos++) {

        char wordbreak = SPACE;
        bool is_wordbreak = false;

        switch (pstate) {

            case PARSE_BLANK:
                if (*readpos != SPACE && *readpos != TAB) {
                    argc++;
                }

                if (*readpos == SQUOTE) {
                    pstate = PARSE_SINGLEQUOTE;
                }
                else if (*readpos == DQUOTE) {
                    pstate = PARSE_DOUBLEQUOTE;
                }
                else if (*readpos == ESCAPECHAR) {
                    pstate = PARSE_UNQUOTED_ESC;
                }
                else if (*readpos != SPACE && *readpos != TAB) {
                    pstate = PARSE_UNQUOTED;
                    *writepos++ = *readpos;
                }
                break;

            case PARSE_UNQUOTED:
                if (*readpos == SQUOTE) {
                    pstate = PARSE_SINGLEQUOTE;
                }
                else if (*readpos == DQUOTE) {
                    pstate = PARSE_DOUBLEQUOTE;
                }
                else if (*readpos == ESCAPECHAR) {
                    pstate = escape_toggle(pstate);
                }
                else if (*readpos == SPACE || *readpos == TAB) {
                    pstate = PARSE_BLANK;
                    *writepos++ = '\0';
                }
                else {
                    *writepos++ = *readpos;
                }
                break;

            case PARSE_SINGLEQUOTE:
                wordbreak = SQUOTE;
                is_wordbreak = true;
                break;

            case PARSE_DOUBLEQUOTE:
                wordbreak = DQUOTE;
                is_wordbreak = true;
                break;

            default: /* QUOTED state */
                pstate = escape_toggle(pstate);
                *writepos++ = *readpos;
                break;
        }

        if (is_wordbreak) {
            if (*readpos == wordbreak) {
                if (wordbreak == SQUOTE || wordbreak == DQUOTE) {
                    pstate = PARSE_UNQUOTED;
                }
            }
            else if (*readpos == ESCAPECHAR) {
                pstate = escape_toggle(pstate);
            }
            else {
                *writepos++ = *readpos;
            }
        }
    }
    *writepos = '\0';

    if (pstate != PARSE_BLANK && pstate != PARSE_UNQUOTED) {
        printf("shell: incorrect quoting\n");
        return -EINVAL;
    }

    if (argc == 0) {
        return 0;
    }

    /* then we fill the argv array */
    int collected;

    /* allocate argv on the stack leaving space for NULL termination */
    char *argv[argc + 1];

    readpos = line;
    for (collected = 0; collected < argc; collected++) {
        argv[collected] = readpos;
        readpos += strlen(readpos) + 1;
    }

    /* NULL terminate argv. See `shell_command_handler_t` doc in shell.h for
       rationale. */
    argv[argc] = NULL;

    /* then we call the appropriate handler */
    shell_command_handler_t handler = shell_find_handler(command_list, argv[0]);
    if (handler != NULL) {
        if (IS_USED(MODULE_SHELL_HOOKS)) {
            shell_pre_command_hook(argc, argv);
            int res = handler(argc, argv);
            shell_post_command_hook(res, argc, argv);
            return res;
        }
        else {
            return handler(argc, argv);
        }
    }
    else {
        if (strcmp("help", argv[0]) == 0) {
            print_help(command_list);
            return 0;
        }
        else if (IS_USED(MODULE_SHELL_BUILTIN_CMD_HELP_JSON)
                 && !strcmp("help_json", argv[0])) {
            print_commands_json(command_list);
        }
        else {
            printf("shell: command not found: %s\n", argv[0]);
        }
    }

    return -ENOEXEC;
}

__attribute__((weak)) void shell_post_readline_hook(void)
{

}

__attribute__((weak)) void shell_pre_command_hook(int argc, char **argv)
{
    (void)argv;
    (void)argc;
}

__attribute__((weak)) void shell_post_command_hook(int ret, int argc,
                                                   char **argv)
{
    (void)ret;
    (void)argv;
    (void)argc;
}

static inline void print_prompt(void)
{
    if (!IS_ACTIVE(CONFIG_SHELL_NO_PROMPT) && !IS_ACTIVE(SHELL_NO_PROMPT)) {
        putchar('>');
        putchar(' ');
    }

    flush_if_needed();
}

static inline void echo_char(char c)
{
    if (!IS_ACTIVE(CONFIG_SHELL_NO_ECHO) && !IS_ACTIVE(SHELL_NO_ECHO)) {
        putchar(c);
    }
}

static inline void white_tape(void)
{
    if (!IS_ACTIVE(CONFIG_SHELL_NO_ECHO) && !IS_ACTIVE(SHELL_NO_ECHO)) {
        putchar('\b');
        putchar(' ');
        putchar('\b');
    }
}

static inline void new_line(void)
{
    if (!IS_ACTIVE(CONFIG_SHELL_NO_ECHO) && !IS_ACTIVE(SHELL_NO_ECHO)) {
        putchar('\r');
        putchar('\n');
    }
}

/**
 * @brief   Read a single line from standard input into a buffer.
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
 * @param   buf     Buffer where the input will be placed.
 * @param   size    Size of the buffer. The maximum line length will be one less
 *                  than size, to accommodate for the null terminator.
 *                  The minimum buffer size is 1.
 *
 * @return  length of the read line, excluding the terminator, if reading was
 *          successful.
 * @return  EOF, if the end of the input stream was reached.
 * @return  -ENOBUFS if the buffer size was exceeded.
 */
int readline(char *buf, size_t size) /* needed externally by module shell_lock */
{
    int curr_pos = 0;
    bool length_exceeded = false;

    assert((size_t) size > 0);

    while (1) {
        assert((size_t) curr_pos < size);

        int c = getchar();

        switch (c) {

            case EOT:
                /* Ctrl-D terminates the current shell instance. */
                /* fall-thru */
            case EOF:
                return EOF;

            case ETX:
                /* Ctrl-C cancels the current line. */
                curr_pos = 0;
                length_exceeded = false;
                /* fall-thru */
            case '\r':
                /* fall-thru */
            case '\n':
                buf[curr_pos] = '\0';

                new_line();

                return (length_exceeded) ? -ENOBUFS : curr_pos;

            /* check for backspace: */
            case BS:    /* 0x08 (BS) for most terminals */
                /* fall-thru */
            case DEL:   /* 0x7f (DEL) when using QEMU */
                if (curr_pos > 0) {
                    curr_pos--;
                    if ((size_t) curr_pos < size) {
                        buf[curr_pos] = '\0';
                        length_exceeded = false;
                    }
                    white_tape();
                }
                break;

            default:
                /* Always consume characters, but do not not always store them */
                if ((size_t) curr_pos < size - 1) {
                    buf[curr_pos++] = c;
                }
                else {
                    length_exceeded = true;
                }
                echo_char(c);
                break;
        }

        flush_if_needed();
    }
}

void shell_run_once(const shell_command_t *shell_commands,
                    char *line_buf, int len)
{
    if (IS_USED(MODULE_SHELL_LOCK)) {
        shell_lock_checkpoint(line_buf, len);
    }

    print_prompt();

    while (1) {
        int res = readline(line_buf, len);

        if (IS_USED(MODULE_SHELL_LOCK)) {
            if (shell_lock_is_locked()) {
                break;
            }
        }

        if (IS_USED(MODULE_SHELL_LOCK_AUTO_LOCKING)) {
            /* reset lock countdown in case of new input */
            shell_lock_auto_lock_refresh();
        }

        switch (res) {

            case EOF:
                if (IS_USED(MODULE_SHELL_LOCK)) {
                    shell_lock_do_lock();
                }
                return;

            case -ENOBUFS:
                printf("shell: maximum line length exceeded\n");
                break;

            default:
                shell_handle_input_line(shell_commands, line_buf);
                break;
        }

        print_prompt();
    }
}

#ifdef MODULE_VFS
int shell_parse_file(const shell_command_t *shell_commands,
                     const char *filename, unsigned *line_nr)
{
    char buffer[SHELL_DEFAULT_BUFSIZE];

    if (line_nr) {
        *line_nr = 0;
    }

    int res, fd = vfs_open(filename, O_RDONLY, 0);
    if (fd < 0) {
        printf("Can't open %s\n", filename);
        return fd;
    }

    while (1) {
        res = vfs_readline(fd, buffer, sizeof(buffer));
        if (line_nr) {
            *line_nr += 1;
        }
        /* error reading line */
        if (res < 0) {
            break;
        }
        /* skip comment and empty lines */
        if (buffer[0] == '#') {
            continue;
        }
        res = shell_handle_input_line(shell_commands, buffer);
        if (res) {
            break;
        }
    }

    vfs_close(fd);

    return res;
}
#endif
