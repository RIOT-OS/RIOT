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
#include "readline_history.h"

/*
 * TODO: These tests are made for a history-buffer of 16 characters total.
 */
#define SSHELL_HISTORY_SIZE 16

static char BUFFER[SHELL_BUFFER_SIZE];

static void set_up(void)
{
    memset(BUFFER, 0, SHELL_BUFFER_SIZE);
}

static void test_history_simple(void)
{
    char *buf = BUFFER;
    int len;
    history_reset_internal();

    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(0, len);
    TEST_ASSERT_EQUAL_STRING("", buf);

    history_put("11111");

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("11111", buf);

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("11111", buf);

    history_reset();

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("11111", buf);

}

static void test_history_filling(void)
{
    char BUFFER[SHELL_BUFFER_SIZE];
    char *buf = BUFFER;
    int len;
    history_reset_internal();

    history_put("11111");
    history_put("222");
    history_put("3333");

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(4, len);
    TEST_ASSERT_EQUAL_STRING("3333", buf);

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(3, len);
    TEST_ASSERT_EQUAL_STRING("222", buf);

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("11111", buf);

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("11111", buf);
}

/*
 * Test that the history returns always the last
 * item, when there are no more items to cycle.
 */
static void test_history_multiprev(void)
{
    char BUFFER[SHELL_BUFFER_SIZE];
    char *buf = BUFFER;
    int len;
    history_reset_internal();

    history_put("11111");

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("11111", buf);

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("11111", buf);

}

static void test_history_next_prev(void)
{
    char BUFFER[SHELL_BUFFER_SIZE];
    char *buf = BUFFER;
    int len;
    history_reset_internal();

    *buf = '\0';
    len = history_getnext(buf);
    TEST_ASSERT_EQUAL_INT(0, len);
    TEST_ASSERT_EQUAL_STRING("", buf);

    history_put("11111");

    *buf = '\0';
    len = history_getnext(buf);
    TEST_ASSERT_EQUAL_INT(0, len);
    TEST_ASSERT_EQUAL_STRING("", buf);

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("11111", buf);

    *buf = '\0';
    len = history_getnext(buf);
    TEST_ASSERT_EQUAL_INT(0, len);
    TEST_ASSERT_EQUAL_STRING("", buf);

    history_put("222");

    history_put("33");

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(2, len);
    TEST_ASSERT_EQUAL_STRING("33", buf);

    *buf = '\0';
    len = history_getnext(buf);
    TEST_ASSERT_EQUAL_INT(0, len);
    TEST_ASSERT_EQUAL_STRING("", buf);

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(2, len);
    TEST_ASSERT_EQUAL_STRING("33", buf);

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(3, len);
    TEST_ASSERT_EQUAL_STRING("222", buf);

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("11111", buf);

    *buf = '\0';
    len = history_getprev(buf);
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("11111", buf);

    *buf = '\0';
    len = history_getnext(buf);
    TEST_ASSERT_EQUAL_INT(3, len);
    TEST_ASSERT_EQUAL_STRING("222", buf);

    *buf = '\0';
    len = history_getnext(buf);
    TEST_ASSERT_EQUAL_INT(2, len);
    TEST_ASSERT_EQUAL_STRING("33", buf);

    *buf = '\0';
    len = history_getnext(buf);
    TEST_ASSERT_EQUAL_INT(0, len);
    TEST_ASSERT_EQUAL_STRING("", buf);

}

Test *tests_readline_internal_history_tests_internal(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_history_simple),
        new_TestFixture(test_history_filling),
        new_TestFixture(test_history_next_prev),
        new_TestFixture(test_history_multiprev),
    };

    EMB_UNIT_TESTCALLER(readline_internal_history_tests, set_up, NULL, fixtures);

    return (Test *)&readline_internal_history_tests;
}
