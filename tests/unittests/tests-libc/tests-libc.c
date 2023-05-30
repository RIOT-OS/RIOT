/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <string.h>
#include "string_utils.h"

#include "tests-libc.h"

static void test_libc_strscpy(void)
{
    char buffer[8];

    TEST_ASSERT_EQUAL_INT(strscpy(buffer, "Hello", sizeof(buffer)), 5);
    TEST_ASSERT_EQUAL_INT(strcmp(buffer, "Hello"), 0);
    TEST_ASSERT_EQUAL_INT(strscpy(buffer, "012345678", sizeof(buffer)), -E2BIG);
    TEST_ASSERT_EQUAL_INT(strcmp(buffer, "0123456"), 0);
    memset(buffer, 0, sizeof(buffer));
    TEST_ASSERT_EQUAL_INT(strscpy(buffer, "01234567", sizeof(buffer)), -E2BIG);
    TEST_ASSERT_EQUAL_INT(strcmp(buffer, "0123456"), 0);
    memset(buffer, 0, sizeof(buffer));
    TEST_ASSERT_EQUAL_INT(strscpy(buffer, "0123456", sizeof(buffer)), 7);
    TEST_ASSERT_EQUAL_INT(strcmp(buffer, "0123456"), 0);
    TEST_ASSERT_EQUAL_INT(strscpy(buffer, "empty", 0), -E2BIG);
}

static void test_libc_memchk(void)
{
    char buffer[32];
    memset(buffer, 0xff, sizeof(buffer));
    TEST_ASSERT_NULL(memchk(buffer, 0xff, sizeof(buffer)));

    buffer[5] = 5;
    TEST_ASSERT(memchk(buffer, 0xff, sizeof(buffer)) == &buffer[5]);
}

Test *tests_libc_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_libc_strscpy),
        new_TestFixture(test_libc_memchk),
    };

    EMB_UNIT_TESTCALLER(libc_tests, NULL, NULL, fixtures);

    return (Test *)&libc_tests;
}

void tests_libc(void)
{
    TESTS_RUN(tests_libc_tests());
}
