/*
 * Copyright (C) 2015 Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* clib includes */

#include <string.h>
#include <stdlib.h>

#include "embUnit/embUnit.h"
#include "tests-readline.h"

#include "readline.h"
#include "readline_internal.h"
#include "readline_history.h"

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

static void test_history(void)
{
    char *buf = BUFFER;
    int cursor = 0;

    strcpy(buf, "");
    history_cycle_up(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_INT(0, cursor);
    TEST_ASSERT_EQUAL_STRING("", buf);

    strcpy(buf, "test input");
    cursor = 10;
    history_cycle_up(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_INT(0, cursor);
    TEST_ASSERT_EQUAL_STRING("", buf);

    history_put("first history entry");
    strcpy(buf, "test input");
    cursor = 10;
    history_cycle_up(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_INT(19, cursor);
    TEST_ASSERT_EQUAL_STRING("first history entry", buf);
    history_cycle_up(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_INT(19, cursor);
    TEST_ASSERT_EQUAL_STRING("first history entry", buf);
    history_cycle_down(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_INT(0, cursor);
    TEST_ASSERT_EQUAL_STRING("", buf);
    history_cycle_down(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_INT(0, cursor);
    TEST_ASSERT_EQUAL_STRING("", buf);

    history_put("second entry");
    cursor = 10;
    history_cycle_up(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_INT(12, cursor);
    TEST_ASSERT_EQUAL_STRING("second entry", buf);
    history_cycle_up(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_INT(19, cursor);
    TEST_ASSERT_EQUAL_STRING("first history entry", buf);
    history_cycle_down(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_INT(12, cursor);
    TEST_ASSERT_EQUAL_STRING("second entry", buf);
    history_cycle_down(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_INT(0, cursor);
    TEST_ASSERT_EQUAL_STRING("", buf);
}

static void test_history_reset(void)
{
    char *buf = BUFFER;
    int cursor = 0;

    strcpy(buf, "");
    history_put("first history entry");
    history_put("second entry");
    history_put("last one");

    history_cycle_up(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_STRING("last one", buf);
    TEST_ASSERT_EQUAL_INT(strlen(buf), cursor);

    history_cycle_up(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_STRING("second entry", buf);
    TEST_ASSERT_EQUAL_INT(strlen(buf), cursor);

    history_reset();

    history_cycle_up(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_STRING("last one", buf);
    TEST_ASSERT_EQUAL_INT(strlen(buf), cursor);

    history_reset();

    history_cycle_down(&shell, &cursor, buf);
    TEST_ASSERT_EQUAL_INT(0, cursor);
    TEST_ASSERT_EQUAL_STRING("", buf);
}

Test *tests_readline_internal_history_tests(void)
{
    (void) test_history;
    (void) test_history_reset;
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_history),
//        new_TestFixture(test_history_reset),
    };

    EMB_UNIT_TESTCALLER(readline_internal_history_tests, set_up, NULL, fixtures);

    return (Test *)&readline_internal_history_tests;
}
