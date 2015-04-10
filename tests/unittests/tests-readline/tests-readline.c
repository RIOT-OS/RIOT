/*
 * Copyright (C) 2015 Janos Kutscherauer <noshky@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <ctype.h>

#include "tests-readline.h"

void tests_readline(void)
{
    TESTS_RUN(tests_readline_internal_cut_paste_tests());
    TESTS_RUN(tests_readline_internal_cursor_tests());
    TESTS_RUN(tests_readline_internal_cut_words_tests());
    TESTS_RUN(tests_readline_internal_history_tests());
    TESTS_RUN(tests_readline_internal_history_tests_internal());
}

void put_char_dummy(int c)
{
    (void) c;
}

int get_char_dummy(void)
{
    return 0;
}

/*
 * In these tests, the current cursor position is marked with
 * an uppercase character or with '_', if the cursor is at
 * a whitespace position. The cursor being at the end of the
 * string is denoted with a '#' character.
 * The test-wrapper function replaces the '_' with a ' ' and
 * the '#' character with the '\0' to have a propper test.
 */
void _replace_cursor_marks(char *buf)
{
    while (*buf) {
        if (*buf == '_') {
            *buf = ' ';
        }
        else if (*buf == '#') {
            *buf = '\0';
        }
        else if (isupper(*buf)) {
            *buf = tolower(*buf);
        }
        buf++;
    }
}
