/*
 * Copyright (C) 2014 Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "tests-gtimer.h"
/**
 * @ingroup tests
 * @{
 *
 * @brief   tests gtimer implementation
 *
 * @author  Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author  Daniel Jentsch <d.jentsch@fu-berlin.de>
 *
 * @}
 */

#include "timex.h"

static void test_gtimer_initial_state(void)
{
    gtimer_test_reset();
    timex_t now;

    gtimer_now(&now);
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(now, timex_set(0, 0)));

    gtimer_test_wait(timex_set(2, 0));
    gtimer_now(&now);
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(now, timex_set(2, 0)));
}

static void test_gtimer_rate(void)
{
    gtimer_test_reset();

    timex_t now;
    gtimer_sync_set_relative_rate(0.5);
    gtimer_test_wait(timex_set(4, 0));
    gtimer_now(&now);
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(now, timex_set(6, 0)));

    gtimer_sync_set_relative_rate(0);
    gtimer_test_wait(timex_set(6, 0));
    gtimer_now(&now);
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(now, timex_set(12, 0)));
}

static void test_gtimer_offset(void)
{
    gtimer_test_reset();

    gtimer_sync_set_global_offset(1000000);
    timex_t now;
    gtimer_test_wait(timex_set(2, 0));
    gtimer_now(&now);
    TEST_ASSERT_EQUAL_INT(0, timex_cmp(now, timex_set(3, 0)));
}

static void test_gtimer_rate_and_offset(void)
{
    gtimer_test_reset();

    gtimer_test_wait(timex_set(10, 0));

    gtimer_sync_set_global_offset(1000000);
    gtimer_sync_set_relative_rate(0.5);

    gtimer_test_wait(timex_set(10, 0));

    timex_t now;
    gtimer_now(&now);

    TEST_ASSERT_EQUAL_INT(0, timex_cmp(now, timex_set(26, 0)));
}

Test *tests_gtimer_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_gtimer_initial_state),
                        new_TestFixture(test_gtimer_rate),
                        new_TestFixture(test_gtimer_offset),
                        new_TestFixture(test_gtimer_rate_and_offset),
    };

    EMB_UNIT_TESTCALLER(gtimer_tests, NULL, NULL, fixtures);

    return (Test *)&gtimer_tests;
}

void tests_gtimer(void)
{
    TESTS_RUN(tests_gtimer_tests());
}
