/*
 * SPDX-FileCopyrightText: 2022 SSV Software Systems GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Unit tests for ztimer_ondemand
 *
 * @author      Juergen Fitschen <me@jue.yt>
 */

#include "ztimer.h"
#include "ztimer/mock.h"
#include "ztimer/convert_frac.h"

#include "embUnit/embUnit.h"

#include "tests-ztimer.h"

#include <stdio.h>

static void test_ztimer_ondemand_acquire_release(void)
{
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;

    /* initialize 32bit mock timer */
    ztimer_mock_init(&zmock, 32);

    TEST_ASSERT_EQUAL_INT(0, zmock.running);

    ztimer_acquire(z);
    TEST_ASSERT_EQUAL_INT(1, zmock.running);
    TEST_ASSERT_EQUAL_INT(0, zmock.armed);
    TEST_ASSERT_EQUAL_INT(1, zmock.calls.start);

    ztimer_acquire(z);
    TEST_ASSERT_EQUAL_INT(1, zmock.running);
    TEST_ASSERT_EQUAL_INT(1, zmock.calls.start);

    ztimer_release(z);
    TEST_ASSERT_EQUAL_INT(1, zmock.running);

    ztimer_release(z);
    TEST_ASSERT_EQUAL_INT(0, zmock.running);
    TEST_ASSERT_EQUAL_INT(0, zmock.armed);
    TEST_ASSERT_EQUAL_INT(1, zmock.calls.stop);
}

static void test_ztimer_ondemand_acquire_ops_unsupported(void)
{
    ztimer_mock_t zmock;
    ztimer_ops_t zmock_ops;
    ztimer_clock_t *z = &zmock.super;

    /* initialize mock timer without start and stop ops */
    ztimer_mock_init(&zmock, 24);
    zmock_ops = *zmock.super.ops;
    zmock_ops.start = NULL;
    zmock_ops.stop = NULL;
    zmock.super.ops = &zmock_ops;

    ztimer_acquire(z);
    TEST_ASSERT_EQUAL_INT(0, zmock.calls.start);
    TEST_ASSERT_EQUAL_INT(1, zmock.armed);

    ztimer_release(z);
    TEST_ASSERT_EQUAL_INT(0, zmock.calls.stop);
    TEST_ASSERT_EQUAL_INT(0, zmock.armed);
}

static void test_ztimer_ondemand_acquire_release_extend(void)
{
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;

    /* initialize 24bit mock timer that must be extended */
    ztimer_mock_init(&zmock, 24);

    TEST_ASSERT_EQUAL_INT(0, zmock.running);

    ztimer_acquire(z);
    TEST_ASSERT_EQUAL_INT(1, zmock.running);
    TEST_ASSERT_EQUAL_INT(1, zmock.armed);

    ztimer_release(z);
    TEST_ASSERT_EQUAL_INT(0, zmock.running);
    TEST_ASSERT_EQUAL_INT(0, zmock.armed);
}

#define HAS_BEEN_EXEC (1)

static void _set_cb(void *arg)
{
    int *val = arg;
    *val = HAS_BEEN_EXEC;
}

static void test_ztimer_ondemand_timers(void)
{
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;

    ztimer_mock_init(&zmock, 32);
    z->adjust_clock_start = 10;

    /* set first timer */
    int a_arg = 0;
    ztimer_t a = {.callback = _set_cb, .arg = &a_arg};
    const uint32_t a_val = 100;
    ztimer_set(z, &a, a_val);
    TEST_ASSERT_EQUAL_INT(a_val - z->adjust_clock_start, zmock.target);
    TEST_ASSERT_EQUAL_INT(1, zmock.running);
    TEST_ASSERT_EQUAL_INT(1, zmock.armed);

    /* set second timer */
    int b_arg = 0;
    ztimer_t b = {.callback = _set_cb, .arg = &b_arg};
    const uint32_t b_val = 200;
    uint32_t diff = zmock.target;
    ztimer_set(z, &b, b_val);

    /* let the first timer fire */
    ztimer_mock_advance(&zmock, zmock.target);
    TEST_ASSERT_EQUAL_INT(HAS_BEEN_EXEC, a_arg);
    TEST_ASSERT_EQUAL_INT(b_val - diff, zmock.target);
    TEST_ASSERT_EQUAL_INT(1, zmock.running);
    TEST_ASSERT_EQUAL_INT(1, zmock.armed);

    /* let the second timer fire -> no users left! */
    ztimer_mock_advance(&zmock, zmock.target);
    TEST_ASSERT_EQUAL_INT(HAS_BEEN_EXEC, b_arg);
    TEST_ASSERT_EQUAL_INT(0, zmock.running);
    TEST_ASSERT_EQUAL_INT(0, zmock.armed);

    /* set and remove timer */
    ztimer_set(z, &a, a_val);
    TEST_ASSERT_EQUAL_INT(1, zmock.running);
    TEST_ASSERT_EQUAL_INT(1, zmock.armed);
    ztimer_remove(z, &a);
    TEST_ASSERT_EQUAL_INT(0, zmock.running);
    TEST_ASSERT_EQUAL_INT(0, zmock.armed);
}

static void test_ztimer_ondemand_timers_converted(void)
{
    ztimer_mock_t zmock;
    ztimer_convert_frac_t zc;
    ztimer_clock_t *z = &zc.super.super;

    /* the base timer must be extended */
    ztimer_mock_init(&zmock, 24);
    const uint32_t factor = 10;
    ztimer_convert_frac_init(&zc, &zmock.super, 1, factor);

    z->adjust_clock_start = 10;

    int a_arg = 0;
    ztimer_t a = {.callback = _set_cb, .arg = &a_arg};
    const uint32_t a_val = 100;
    ztimer_set(z, &a, a_val);
    TEST_ASSERT_EQUAL_INT((a_val - z->adjust_clock_start) * factor, zmock.target);
    TEST_ASSERT_EQUAL_INT(1, zmock.running);
    TEST_ASSERT_EQUAL_INT(1, zmock.armed);

    ztimer_mock_advance(&zmock, zmock.target);
    TEST_ASSERT_EQUAL_INT(HAS_BEEN_EXEC, a_arg);
    TEST_ASSERT_EQUAL_INT(0, zmock.running);
    TEST_ASSERT_EQUAL_INT(0, zmock.armed);
}

Test *tests_ztimer_ondemand_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ztimer_ondemand_acquire_release),
        new_TestFixture(test_ztimer_ondemand_acquire_ops_unsupported),
        new_TestFixture(test_ztimer_ondemand_acquire_release_extend),
        new_TestFixture(test_ztimer_ondemand_timers),
        new_TestFixture(test_ztimer_ondemand_timers_converted),
    };

    EMB_UNIT_TESTCALLER(ztimer_tests, NULL, NULL, fixtures);

    return (Test *)&ztimer_tests;
}

/** @} */
