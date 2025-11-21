/*
 * SPDX-FileCopyrightText: 2014 Philipp Rosenkranz, Daniel Jentsch
 * SPDX-FileCopyrightText: 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "tests-timex.h"

#include "timex.h"

static void test_timex_set(void)
{
    timex_t time;
    time = timex_set(1, 0);
    TEST_ASSERT_EQUAL_INT(1, time.seconds);
    TEST_ASSERT_EQUAL_INT(0, time.microseconds);
}

static void test_timex_add(void)
{
    timex_t time;
    time = timex_add(timex_set(100, 100), timex_set(40, 10));
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(time, timex_set(140, 110)));
    time = timex_add(timex_set(100, 700000), timex_set(40, 800000));
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(time, timex_set(141, 500000)));
}

static void test_timex_sub(void)
{
    timex_t time;
    time = timex_sub(timex_set(100, 100), timex_set(40, 10));
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(time, timex_set(60, 90)));
    time = timex_sub(timex_set(100, 100), timex_set(40, 200));
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(time, timex_set(59, 999900)));
}

static void test_timex_from_uint64(void)
{
    timex_t time;
    time = timex_from_uint64(1001000);
    TEST_ASSERT(time.seconds == 1);
    TEST_ASSERT(time.microseconds == 1000);
}

static void test_timex_to_str(void)
{
    timex_t t = { 0, 0 };
    char t_str[TIMEX_MAX_STR_LEN];

    TEST_ASSERT_EQUAL_STRING("0.000000 s", timex_to_str(t, t_str));
    t.seconds = 1;
    TEST_ASSERT_EQUAL_STRING("1.000000 s", timex_to_str(t, t_str));
    t.seconds = 9;
    t.microseconds = 2640288149;
    TEST_ASSERT_EQUAL_STRING("2649.288149 s", timex_to_str(t, t_str));
    t.seconds = 1689940699;
    t.microseconds = 4361;
    TEST_ASSERT_EQUAL_STRING("1689940699.004361 s", timex_to_str(t, t_str));
    t.seconds = 100;
    t.microseconds = 101010;
    TEST_ASSERT_EQUAL_STRING("100.101010 s", timex_to_str(t, t_str));
    t.seconds = UINT32_MAX;
    t.microseconds = 999999;        /* should be .999999 */
    TEST_ASSERT_EQUAL_STRING("4294967295.999999 s", timex_to_str(t, t_str));
}

Test *tests_timex_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_timex_set),
        new_TestFixture(test_timex_add),
        new_TestFixture(test_timex_sub),
        new_TestFixture(test_timex_from_uint64),
        new_TestFixture(test_timex_to_str),
    };

    EMB_UNIT_TESTCALLER(timex_tests, NULL, NULL, fixtures);

    return (Test *)&timex_tests;
}

void tests_timex(void)
{
    TESTS_RUN(tests_timex_tests());
}
