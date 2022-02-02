/*
 * Copyright (C) 2022 SSV Software Systems GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

Test *tests_ztimer_ondemand_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ztimer_ondemand_acquire_release),
        new_TestFixture(test_ztimer_ondemand_acquire_ops_unsupported),
        new_TestFixture(test_ztimer_ondemand_acquire_release_extend),
    };

    EMB_UNIT_TESTCALLER(ztimer_tests, NULL, NULL, fixtures);

    return (Test *)&ztimer_tests;
}

/** @} */
