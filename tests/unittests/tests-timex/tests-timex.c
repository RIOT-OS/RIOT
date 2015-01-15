/*
 * Copyright (C) 2014 Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
    timex_t a = timex_set(100, 100);
    timex_t b = timex_set(40, 10);
    timex_add(&a, &b, &time);
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(time, timex_set(140, 110)));
    a = timex_set(100, 700000);
    b = timex_set(40, 800000);
    timex_add(&a, &b, &time);
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(time, timex_set(141, 500000)));
}

static void test_timex_sub(void)
{
    timex_t time, a, b;
    a = timex_set(100, 100);
    b = timex_set(40, 10);
    timex_sub(&a, &b, &time);
    a = timex_set(60,90);
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(time, a));
    a = timex_set(100, 100);
    b = timex_set(40, 200);
    timex_sub(&a, &b, &time);
    a = timex_set(59,999900);
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(time, a));
}

static void test_timex_from_uint64(void)
{
    timex_t time;
    time = timex_from_uint64(1001000);
    TEST_ASSERT(time.seconds == 1);
    TEST_ASSERT(time.microseconds == 1000);
}

Test *tests_timex_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_timex_set),
        new_TestFixture(test_timex_add),
        new_TestFixture(test_timex_sub),
        new_TestFixture(test_timex_from_uint64),
    };

    EMB_UNIT_TESTCALLER(timex_tests, NULL, NULL, fixtures);

    return (Test *)&timex_tests;
}

void tests_timex(void)
{
    TESTS_RUN(tests_timex_tests());
}
