/*
 * Copyright (C) 2015 Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* clib includes */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "embUnit/embUnit.h"
#include "tests-readline.h"

#include "readline.h"
#include "readline_internal.h"

static shell_t shell;

/* Test-Global variables, which are used to have a cleaner notation of test cases. */
static char _buf[SHELL_BUFFER_SIZE];
static char _exp_buf[SHELL_BUFFER_SIZE];
static char _cut_buf[SHELL_BUFFER_SIZE];
static char _exp_cut_buf[SHELL_BUFFER_SIZE];
static int _cursor; // global cursor
static int _exp_cursor;

#define TEST_STRING "123456789"
#define TEST_STRING2 "abcdefghi"
#define LEN_TEST_STRING 9

static void set_up(void)
{
    shell.put_char = put_char_dummy;
    shell.readchar = get_char_dummy;
    shell.shell_buffer_size = SHELL_BUFFER_SIZE;
    memset(_buf, 0, SHELL_BUFFER_SIZE);
    memset(_exp_buf, 0, SHELL_BUFFER_SIZE);
    memset(_cut_buf, 0, SHELL_BUFFER_SIZE);
    memset(_exp_cut_buf, 0, SHELL_BUFFER_SIZE);
}

void _test_cut_word_before(const char *buf, int cursor, const char *exp_buf, int exp_cursor,
        const char *exp_cut_buf)
{
    strcpy(_buf, buf);
    strcpy(_exp_buf, exp_buf);
    strcpy(_exp_cut_buf, exp_cut_buf);
    _replace_cursor_marks(_buf);
    _replace_cursor_marks(_exp_buf);
    _replace_cursor_marks(_exp_cut_buf);
    _cursor = cursor;
    _exp_cursor = exp_cursor;
    cut_word_before(&shell, &_cursor, _buf, _cut_buf);
}

void _test_cut_word_behind(const char *buf, int cursor, const char *exp_buf, int exp_cursor,
        const char *exp_cut_buf)
{
    strcpy(_buf, buf);
    strcpy(_exp_buf, exp_buf);
    strcpy(_exp_cut_buf, exp_cut_buf);
    _replace_cursor_marks(_buf);
    _replace_cursor_marks(_exp_buf);
    _replace_cursor_marks(_exp_cut_buf);
    _cursor = cursor;
    _exp_cursor = exp_cursor;
    cut_word_behind(&shell, &_cursor, _buf, _cut_buf);
}

static void test_cut_word_before(void)
{
    char *buf = _buf;
    char *cut_buf = _cut_buf;
    char *exp_buf = _exp_buf;
    char *exp_cut_buf = _exp_cut_buf;

    _test_cut_word_before("#", 0, "", 0, "#");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_before("this_is a test", 4, "_is a test", 0, "this");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_before("This is a test", 0, "This is a test", 0, "");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_before("this is a test#", 14, "this is a #", 10, "test");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_before("this longWord test", 9, "this Word test", 5, "long");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_before(" #", 1, "#", 0, " ");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_before("_", 0, "_", 0, "");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_before(" _ ", 1, "  ", 0, " ");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_before("   thiS    ", 6, "   S    ", 3, "thi");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_before("   this  _ ", 9, "   _ ", 3, "this  ");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);
}

static void test_cut_word_behind(void)
{
    char *buf = _buf;
    char *cut_buf = _cut_buf;
    char *exp_buf = _exp_buf;
    char *exp_cut_buf = _exp_cut_buf;

    _test_cut_word_behind("#", 0, "#", 0, "");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_behind("this_is a test", 4, "this_a test", 4, " is");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_behind("This is a test", 0, "_is a test", 0, "this");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_behind("tHis     is a test", 1, "t_    is a test", 1, "his");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_behind("this is a test#", 14, "this is a test#", 14, "");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_behind("this longWord test", 9, "this long_test", 9, "word");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_behind(" #", 1, " #", 1, "");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_behind("_", 0, "#", 0, " ");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_behind(" _ ", 1, " #", 1, "  ");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_behind("   tHis    ", 4, "   t_   ", 4, "his");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);

    _test_cut_word_behind("  _ this    ", 2, "  _   ", 2, "  this");
    TEST_ASSERT_EQUAL_INT(_exp_cursor, _cursor);
    TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
    TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);
}

Test *tests_readline_internal_cut_words_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
    new_TestFixture(test_cut_word_behind),
    new_TestFixture(test_cut_word_before),
};

EMB_UNIT_TESTCALLER(readline_internal_cut_words_tests, set_up, NULL, fixtures);

return (Test *)&readline_internal_cut_words_tests;
}
