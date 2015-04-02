/*
 * Copyright (C) 2015 Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "shell.h"
#include "shell_print.h"
#include "shell_colors.h"

#include "readline.h"
#include "include/readline_internal.h"
#include "include/readline_scan.h"

#if SHELL_HISTORY_SIZE > 0
#include "include/readline_history.h"
#endif

static char cut_buffer[SHELL_BUFFER_SIZE];

// TODO: Introduce definition PROMPT_LENGTH (instead of "2")
#define PROMPT_LENGTH 2

/*
 * Create a terminal command to set the cursor to the specified position, plus the size of
 * the prompt (e.g. "> ").
 */
void set_screen_cursor(shell_t *shell, int cursor)
{
    char buf[4];
    shell_print(shell, "\r\x1b[");
    snprintf(buf, 4, "%d", cursor + PROMPT_LENGTH);
    shell_print(shell, buf);
    shell_putchar(shell, 'C');
}

/*
 * Cut 'count' characters behind the cursor position and store in
 * cut-buffer, if 'cutText' is set.
 * If the count value is larger than there are remaining characters, only
 * the remaining characters are cut
 *
 * ``cursor`` is required to be smaller than strlen(buf)
 */
void cut(shell_t *shell, int *cursor, char *buf, char *cut_buf, int count)
{
    int len = strlen(buf);
    if (len == 0 || *cursor > len) {
        return;
    }
    if (*cursor + count > len) {
        count = len - *cursor;
    }
    if (cut_buf) {
        strncpy(cut_buf, buf + *cursor, count);
        cut_buf[count] = '\0';         // strncpy doesn't terminate a null-terminated string
    }
    memmove(buf + *cursor, buf + *cursor + count, len - *cursor - count + 1);
    shell_print(shell, "\x1b[K");      // Erase to the right
    shell_echo_string(shell, buf + *cursor);
    set_screen_cursor(shell, *cursor);
}

/*
 * Caller has to make shure that 'paste_buf' has at least 'count' size.
 */
void paste(shell_t *shell, int *cursor, char *buf, char *paste_buf, int count)
{
    int len = strlen(buf);
    if (len + count >= shell->shell_buffer_size || *cursor > len) {
        return;
    }
    int len_paste_buf = strlen(paste_buf);
    if (count > len_paste_buf) {
        count = len_paste_buf;
    }
    memmove(buf + *cursor + count, buf + *cursor, len - *cursor);
    memcpy(buf + *cursor, paste_buf, count);
    buf[len + count] = '\0';
    shell_echo_string(shell, buf + *cursor);
    *cursor += count;
    set_screen_cursor(shell, *cursor);
}

char* find_word_before(int *cursor, char *buf)
{
    /* Skip all preceeding whitespaces */
    char *tmp_buf = buf + *cursor;
    while (tmp_buf > buf && *(--tmp_buf) == ' ')
        ;
    while (tmp_buf > buf && *tmp_buf != ' ') {
        tmp_buf--;
    }
    if (tmp_buf == buf) {
        return buf;
    }
    return tmp_buf + 1;
}

char* find_word_behind(int *cursor, char *buf)
{
    char *end = strlen(buf) + buf;
    char *tmp_buf = buf + *cursor;
    while (tmp_buf < end && *(tmp_buf++) != ' ')
        ;
    while (tmp_buf < end && *(tmp_buf++) == ' ')
        ;
    if (tmp_buf == buf || tmp_buf == end) {
        return tmp_buf;
    }
    return tmp_buf - 1;
}

void cut_word_before(shell_t *shell, int *cursor, char *buf, char *cut_buf)
{
    char *word = find_word_before(cursor, buf);
    int count = *cursor - (word - buf);
    *cursor = word - buf;
    set_screen_cursor(shell, *cursor);
    cut(shell, cursor, buf, cut_buf, count);
}

void cut_word_behind(shell_t *shell, int *cursor, char *buf, char *cut_buf)
{
    char *word = buf + *cursor;
    if (*word == ' ') {
        word = find_word_behind(cursor, buf);
    }
    while (*word && *word != ' ') {
        word++;
    }
    cut(shell, cursor, buf, cut_buf, (word - buf) - *cursor);
}

void move_cursor_word_left(shell_t *shell, int *cursor, char *buf)
{
    char *word = find_word_before(cursor, buf);
    *cursor = word - buf;
    set_screen_cursor(shell, *cursor);
}

void move_cursor_word_right(shell_t *shell, int *cursor, char *buf)
{
    char *word = find_word_behind(cursor, buf);
    *cursor = word - buf;
    set_screen_cursor(shell, *cursor);
}

#if SHELL_HISTORY_SIZE > 0
static void history_cycle(shell_t *shell, int *cursor, char *buf, bool dir)
{
    set_screen_cursor(shell, *cursor = 0);
    cut(shell, cursor, buf, NULL, shell->shell_buffer_size);
    if (dir) {
        history_getprev(buf);
    }
    else {
        history_getnext(buf);
    }
    *cursor = strlen(buf);
    shell_echo_string(shell, buf);
}

void history_cycle_up(shell_t *shell, int *cursor, char *buf)
{
    history_cycle(shell, cursor, buf, true);
}

void history_cycle_down(shell_t *shell, int *cursor, char *buf)
{
    history_cycle(shell, cursor, buf, false);
}
#endif

/*
 * In case this returns newline, the cursor will be set at the end of the line, no matter
 * where it was before.
 *
 * c is the last character that was received.
 */
control_e readline_control_process(shell_t *shell, int *cursor, char *buf, control_e control)
{
    char space = ' ';
    switch (control) {
        case CONTROL_NL:
#if SHELL_HISTORY_SIZE > 0
            history_reset();
#endif
            set_screen_cursor(shell, *cursor = strlen(buf));
            return CONTROL_NL;
        case CONTROL_TAB:
            for (int i = 0; i < SHELL_TAB_SIZE; i++) {
                paste(shell, cursor, buf, &space, 1);
            }
            break;
        case CONTROL_LEFT:
            if (*cursor > 0) {
                set_screen_cursor(shell, --(*cursor));
            }
            break;
        case CONTROL_RIGHT:
            if (*cursor < (int) strlen(buf)) {
                set_screen_cursor(shell, ++(*cursor));
            }
            break;
        case CONTROL_HOME:
            set_screen_cursor(shell, *cursor = 0);
            break;
        case CONTROL_END:
            set_screen_cursor(shell, *cursor = strlen(buf));
            break;
        case CONTROL_BS:
            if (*cursor > 0) {
                set_screen_cursor(shell, --(*cursor));
                cut(shell, cursor, buf, NULL, 1);
            }
            break;
        case CONTROL_DEL:
            cut(shell, cursor, buf, NULL, 1);
            break;
#if SHELL_HISTORY_SIZE > 0
        case CONTROL_UP:
        case CONTROL_PGUP:
        case CONTROL_CTRL_UP:
            history_cycle_up(shell, cursor, buf);
            break;
        case CONTROL_DOWN:
        case CONTROL_PGDWN:
        case CONTROL_CTRL_DOWN:
            history_cycle_down(shell, cursor, buf);
            break;
#endif
        case CONTROL_CUT_ALL:
            set_screen_cursor(shell, *cursor = 0);
            cut(shell, cursor, buf, cut_buffer, shell->shell_buffer_size);
            break;
        case CONTROL_CUT_ALL_BEHIND:
            cut(shell, cursor, buf, cut_buffer, shell->shell_buffer_size);
            break;
        case CONTROL_CUT_WORD_BEFORE:
            cut_word_before(shell, cursor, buf, cut_buffer);
            break;
        case CONTROL_CUT_WORD_AFTER:
            cut_word_behind(shell, cursor, buf, cut_buffer);
            break;
        case CONTROL_PASTE:
            paste(shell, cursor, buf, cut_buffer, strlen(cut_buffer));
            break;
        case CONTROL_CTRL_RIGHT:
            move_cursor_word_right(shell, cursor, buf);
            break;
        case CONTROL_CTRL_LEFT:
            move_cursor_word_left(shell, cursor, buf);
            break;
        case CONTROL_CTRL_DEL: {
            cut_word_behind(shell, cursor, buf, NULL);
            break;
        }
        case CONTROL_UMLAUT:
        case CONTROL_INS:
            // Not implemented yet
            break;
        default:
            break;
    }
    return CONTROL_NONE;
}

int readline(shell_t *shell, char *buf, size_t size)
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

        control_e ctrl = readline_scan_char(c);
        if (ctrl != CONTROL_NONE) {
            control_e control = readline_control_process(shell, &cursor, buf, ctrl);
            switch (control) {
                case CONTROL_NL:
#if SHELL_HISTORY_SIZE > 0
                    history_put(buf);
#endif
                    return cursor;
                default:
                    break;
            }
            continue;
        }
        // Any other character
        paste(shell, &cursor, buf, (char*) &c, 1);
    }
}
