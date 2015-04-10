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

#include "readline.h"
#include "include/readline_history.h"

#if SHELL_HISTORY_SIZE > 0

#define BUFLEN (SHELL_BUFFER_SIZE * SHELL_HISTORY_SIZE)
static char HISTORY[BUFLEN];
static char *i_history = NULL;
static char *i_cycl = NULL;

#define DEC(ptr, inc) ((ptr) - (inc) < HISTORY ? (ptr) - (inc) + BUFLEN : (ptr) - (inc))
#define INC(ptr, inc) ((ptr) + (inc) >= HISTORY + BUFLEN ? (ptr) + (inc) - BUFLEN : (ptr) + (inc))

static int cpyword(char *to, char *from)
{
    int count = 0;
    while (*from != '\0') {
        *(to++) = *from;
        from = INC(from, 1);
        count++;
    }
    *to = '\0';
    return count;
}

static char *find_prev_word(char *from) {
    do {
        from = DEC(from, 1);
    } while (*from != '\0');
    return INC(from, 1);
}

static char *find_next_word(char *from)
{
    do {
        from = INC(from, 1);
    } while (*from != '\0');
    return INC(from, 1);
}

/*
 * 4 cases:
 * case 1: empty history: i_history == NULL
 * case 2: first cycle
 *          [aaa_bbbb_cccccc_dddddddd*.....]    (* = +# = .)
 * case 3: cycling in progress (last cycle was "cccccc")
 *          [aaa_bbbb+cccccc_dddddddd#.....]    (+ = ., # = .)
 * case 4: cycling finished (last cycle was
 *          [aaa_bbbb_cccccc_dddddddd#....+]    (+ = ., # = .)
 */
int history_getprev(char *buf)
{
    if (i_history == NULL || i_cycl == NULL) {
        // empty history (i_cycl == NULL is always the case when i_history == NULL)
        return 0;
    }
    char *prev = find_prev_word(i_cycl);
    if (*prev == '\0') {
        // End of cycle, return current word again
        prev = INC(i_cycl, 1);
    }
    else {
        i_cycl = DEC(prev, 1);
    }
    // Copy the word into the output buffer
    return cpyword(buf, prev);
}

int history_getnext(char *buf)
{
    if (i_history == NULL || i_cycl == NULL) {
        // empty history (i_cycl == NULL is always the case when i_history == NULL)
        return 0;
    }
    char *next = find_next_word(i_cycl);
    if (*next == '\0') {
        // End of cycle, return null
        i_cycl = i_history;
    }
    else {
        i_cycl = DEC(next, 1);
    }
    // Copy the word into the output buffer
    return cpyword(buf, next);
}

void history_put(char *input)
{
    if (i_history == NULL) { // history is empty
        i_history = HISTORY;
    }
    else if (!strncmp(input, find_prev_word(i_cycl), BUFLEN)) {
        // Check if input is the most recent word
        return;
    }
    int len = strlen(input);
    if (!len || len >= BUFLEN) {
        // input empty or doesn't fit
        return;
    }
    i_history = INC(i_history, 1);
    int avail = HISTORY + BUFLEN - i_history;
    if (len <= avail) { // input fits behind cursor
        strncpy(i_history, input, len);
    }
    else { // input doesn't fit, has to be split
        strncpy(i_history, input, avail);
        strncpy(HISTORY, input + avail, len - avail);
    }
    // Mark the end of the word by erasing the next word
    i_history = INC(i_history, len);
    char *tmp = i_history;
    do {
        *tmp = '\0';
        tmp = INC(tmp, 1);
    } while (*tmp != '\0');
    i_cycl = i_history;
}

/*
 * Reset the prev/next cycling.
 */
void history_reset(void)
{
    i_cycl = i_history;
}

/*
 * For unit-tests. Reset the whole history.
 */
void history_reset_internal(void)
{
    memset(HISTORY, '\0', BUFLEN);
    i_history = NULL;
    i_cycl = NULL;
}

void history_printbuf(void)
{
    char *i;
    putchar('[');
    for (i = HISTORY; i < HISTORY + BUFLEN; i++) {
        if (i == i_history && i == i_cycl) {
            putchar('*');
        }
        else if (i == i_history) {
            putchar('#');
        }
        else if (i == i_cycl) {
            putchar('+');
        }
        else if (*i == '\0') {
            putchar('.');
        }
        else {
            putchar(*i);
        }
    }
    putchar(']');
    printf(" i:%02d cycl:%02d", i_history == NULL ? -1 : i_history - HISTORY, i_cycl == NULL ? -1 : i_cycl - HISTORY);
    putchar('\n');
}

#endif
