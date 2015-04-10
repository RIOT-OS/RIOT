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
static char BUFFER[SHELL_BUFFER_SIZE];

#define TEST_STRING "123456789"
#define LEN_TEST_STRING 9

static void set_up(void)
{
    shell.put_char = put_char_dummy;
    shell.readchar = get_char_dummy;
    shell.shell_buffer_size = SHELL_BUFFER_SIZE;
    memset(BUFFER, 0, SHELL_BUFFER_SIZE);
}

static int _test_move_left(const char *buf, int cursor)
{
    strcpy(BUFFER, buf);
    _replace_cursor_marks(BUFFER);
    move_cursor_word_left(&shell, &cursor, BUFFER);
    return cursor;
}

static int _test_move_right(char *buf, int cursor)
{
    strcpy(BUFFER, buf);
    _replace_cursor_marks(BUFFER);
    move_cursor_word_right(&shell, &cursor, BUFFER);
    return cursor;
}

static void test_cursor_move_left(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_move_left("", 0));
    TEST_ASSERT_EQUAL_INT(0, _test_move_left("This is a test", 0));
    TEST_ASSERT_EQUAL_INT(0, _test_move_left("tHis is a test", 1));
    TEST_ASSERT_EQUAL_INT(0, _test_move_left("this_is a test", 4));
    TEST_ASSERT_EQUAL_INT(0, _test_move_left("this Is a test", 5));
    TEST_ASSERT_EQUAL_INT(5, _test_move_left("this iS a test", 6));
    TEST_ASSERT_EQUAL_INT(0, _test_move_left(" _ this is a test", 1));
    TEST_ASSERT_EQUAL_INT(0, _test_move_left("  _this is a test", 2));
    TEST_ASSERT_EQUAL_INT(3, _test_move_left("   This is a test", 4));
    TEST_ASSERT_EQUAL_INT(3, _test_move_left("   this   _  is a test", 10));
    TEST_ASSERT_EQUAL_INT(18, _test_move_left("   this      is a test#", 22));
}

static void test_cursor_move_right(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_move_right("", 0));
    TEST_ASSERT_EQUAL_INT(14, _test_move_right("this is a Test", 10));
    TEST_ASSERT_EQUAL_INT(14, _test_move_right("this is a teSt", 12));
    TEST_ASSERT_EQUAL_INT(10, _test_move_right("this is a_test", 9));
    TEST_ASSERT_EQUAL_INT(5, _test_move_right("This is a test", 0));
    TEST_ASSERT_EQUAL_INT(5, _test_move_right(" _   this is a test", 1));
    TEST_ASSERT_EQUAL_INT(18, _test_move_right("this is a teSt    ", 12));
    TEST_ASSERT_EQUAL_INT(10, _test_move_right("this is A test    ", 8));
    TEST_ASSERT_EQUAL_INT(10, _test_move_right("this is a_test    ", 9));
}

Test *tests_readline_internal_cursor_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_cursor_move_left),
        new_TestFixture(test_cursor_move_right),
    };

    EMB_UNIT_TESTCALLER(readline_internal_cursor_tests, set_up, NULL, fixtures);

    return (Test *)&readline_internal_cursor_tests;
}
