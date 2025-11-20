/*
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for ztimer64
 *
 */

#include <stdio.h>
#include <string.h>

#include "ztimer.h"
#include "ztimer64.h"
#include "ztimer/mock.h"

#include "embUnit/embUnit.h"

#include "tests-ztimer64.h"

#define ZTIMER64_CHECKPOINT_INTERVAL  (1LLU << 31)

/**
 * @brief   Simple callback for counting alarms
 */
static void cb_incr(void *arg)
{
    uint32_t *ptr = arg;

    *ptr += 1;
}

static ztimer_mock_t zmock;
static ztimer_clock_t *z = &zmock.super;
static ztimer64_clock_t z64mock;
static ztimer64_clock_t *z64 = &z64mock;

static void setup(void)
{
    memset(&zmock, '\0', sizeof(ztimer_mock_t));
    memset(&z64mock, '\0', sizeof(ztimer64_clock_t));
    /* ztimer base clock is already extended to 32bit */
    ztimer_mock_init(&zmock, 32);
    ztimer64_clock_init(z64, z);
}

/**
 * @brief   Test ztimer64_now()
 */
static void test_ztimer64_now(void)
{
    uint64_t now64 = ztimer64_now(z64);

    TEST_ASSERT_EQUAL_INT(0, now64);

    ztimer_mock_advance(&zmock, 123);
    now64 = ztimer64_now(z64);
    TEST_ASSERT_EQUAL_INT(123, now64);

    ztimer_mock_jump(&zmock, 0x10000000ul);
    now64 = ztimer64_now(z64);
    TEST_ASSERT_EQUAL_INT(0x10000000ul, now64);

    ztimer_mock_advance(&zmock, 0x98765432ul);
    now64 = ztimer64_now(z64);
    TEST_ASSERT_EQUAL_INT(0xa8765432ul, now64);
    /* 32bit overflow */
    ztimer_mock_advance(&zmock, 0x41234567ul);
    ztimer_mock_advance(&zmock, 0x40000000ul);
    now64 = ztimer64_now(z64);
    TEST_ASSERT_EQUAL_INT(0x129999999ul, now64);
}

/**
 * @brief   Testing 32 bit wide mock clock set functionality
 */
static void test_ztimer64_set(void)
{
    uint64_t now64 = ztimer64_now(z64);

    TEST_ASSERT_EQUAL_INT(0, now64);

    uint32_t count = 0;
    ztimer64_t alarm = { .callback = cb_incr, .arg = &count, };

    ztimer64_set(z64, &alarm, 1000);
    ztimer_mock_advance(&zmock,    1);          /* now =    1*/
    TEST_ASSERT_EQUAL_INT(0, count);
    ztimer_mock_advance(&zmock,  100);          /* now =  101 */
    TEST_ASSERT_EQUAL_INT(0, count);
    ztimer_mock_advance(&zmock,  898);          /* now =  999 */
    now64 = ztimer64_now(z64);
    TEST_ASSERT_EQUAL_INT(999, now64);
    TEST_ASSERT_EQUAL_INT(0, count);
    ztimer_mock_advance(&zmock,    1);          /* now = 1000*/
    TEST_ASSERT_EQUAL_INT(1, count);
    ztimer_mock_advance(&zmock,    1);          /* now = 1001*/
    TEST_ASSERT_EQUAL_INT(1, count);
    ztimer_mock_advance(&zmock, 1000);          /* now = 2001*/
    TEST_ASSERT_EQUAL_INT(1, count);
    ztimer64_set(z64, &alarm, 3);
    ztimer_mock_advance(&zmock,  999);          /* now = 3000*/
    TEST_ASSERT_EQUAL_INT(2, count);
    ztimer64_set(z64, &alarm, 4000001000ul);
    ztimer_mock_advance(&zmock, 1000);          /* now = 4000*/
    TEST_ASSERT_EQUAL_INT(2, count);
    ztimer_mock_advance(&zmock, 4000000000ul);  /* now = 4000004000*/
    now64 = ztimer64_now(z64);
    TEST_ASSERT_EQUAL_INT(4000004000ul, now64);
    TEST_ASSERT_EQUAL_INT(3, count);
    /* 32bit overflow */
    ztimer64_set(z64, &alarm, 8000000000ul);
    ztimer_mock_advance(&zmock, 4000000000ul);
    ztimer_mock_advance(&zmock, 3999999999ul);  /* now = 12000003999*/
    TEST_ASSERT_EQUAL_INT(3, count);
    ztimer_mock_advance(&zmock, 1ul);           /* now = 12000004000*/
    TEST_ASSERT_EQUAL_INT(4, count);
    ztimer64_set(z64, &alarm, 15);
    ztimer_mock_advance(&zmock,  14);
    ztimer64_remove(z64, &alarm);
    ztimer_mock_advance(&zmock, 1000);
    TEST_ASSERT_EQUAL_INT(4, count);
}

/**
 * @brief   Testing 32 bit wide mock clock set functionality
 */
static void test_ztimer64_set_0(void)
{
     uint64_t now64 = ztimer64_now(z64);
    TEST_ASSERT_EQUAL_INT(0, now64);

    uint32_t count = 0;
    ztimer64_t alarm = { .callback = cb_incr, .arg = &count, };

    /* should trigger asap, note that for a real timer this will be
       to minimal value that guarantees the ISR not to be missed,
       e.g: CONFIG_ZTIMER_USEC_MIN or RTT_MIN_OFFSET */
    ztimer64_set(z64, &alarm, 0);
    ztimer_mock_advance(&zmock, 1);          /* now =    1*/
    TEST_ASSERT_EQUAL_INT(1, count);
}

/**
 * @brief   Testing timers set in the past
 */
static void test_ztimer64_set_at(void)
{
    uint64_t now64 = ztimer64_now(z64);

    TEST_ASSERT_EQUAL_INT(0, now64);

    uint32_t count = 0;
    ztimer64_t alarm = { .callback = cb_incr, .arg = &count, };

    /* test setting an offset that is now in the past, should trigger asap */
    ztimer_mock_advance(&zmock, 1010);          /* now =    1010 */
    ztimer64_set_at(z64, &alarm, 1000);
    /* should trigger on next tick */
    ztimer_mock_advance(&zmock, 1);             /* now =    1011 */
    TEST_ASSERT_EQUAL_INT(1, count);
}

static void test_ztimer64_checkpoint(void)
{
    uint64_t now64 = ztimer64_now(z64);

    TEST_ASSERT_EQUAL_INT(0, now64);

    /* base_now is 0x7fff_ffff */
    ztimer_mock_jump(&zmock, ZTIMER64_CHECKPOINT_INTERVAL - 1);
    now64 = ztimer64_now(z64);
    TEST_ASSERT_EQUAL_INT(ZTIMER64_CHECKPOINT_INTERVAL - 1, now64);
    /* base_now is 0x8000_0000 */
    ztimer_mock_jump(&zmock, ZTIMER64_CHECKPOINT_INTERVAL);
    now64 = ztimer64_now(z64);
    TEST_ASSERT_EQUAL_INT(ZTIMER64_CHECKPOINT_INTERVAL, now64);
    /* base_now is 0x0000_0000 */
    ztimer_mock_jump(&zmock, 0);
    now64 = ztimer64_now(z64);
    /* overflow is caught in checkpoint */
    TEST_ASSERT_EQUAL_INT(2 * ZTIMER64_CHECKPOINT_INTERVAL, now64);
    /* base_now is 0xffff_ffff, max jump that can be caught */
    ztimer_mock_jump(&zmock, UINT32_MAX);
    now64 = ztimer64_now(z64);
    /* overflow is caught in checkpoint */
    TEST_ASSERT_EQUAL_INT(2 * ZTIMER64_CHECKPOINT_INTERVAL + UINT32_MAX, now64);
    /* overflow is missed, 2**32 ticks elapsed
        - microseconds: 4293 seconds
        - nanoseconds: 4.29 seconds (e.g.: ptp) -> should not use ztimer as base.
    */
    ztimer_mock_jump(&zmock, UINT32_MAX);
    TEST_ASSERT_EQUAL_INT(2 * ZTIMER64_CHECKPOINT_INTERVAL + UINT32_MAX, now64);
}

static void test_ztimer64_set_uninitialized(void)
{
    /* regression test for setting an uninitialized ztimer64 on an empty clock */
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;
    ztimer64_clock_t z64mock;
    ztimer64_clock_t *z64 = &z64mock;

    memset(&zmock, '\0', sizeof(ztimer_mock_t));
    memset(&z64mock, '\0', sizeof(ztimer64_clock_t));
    /* ztimer base clock is already extended to 32bit */
    ztimer_mock_init(&zmock, 32);
    ztimer64_clock_init(z64, z);

    ztimer64_t timer = { .base.target = 1 };
    ztimer64_set(z64, &timer, 0);
}

static void test_ztimer64_remove_clear(void)
{
    /* regression test, testing if a removed timer passes `!(is_set(t)` */
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;
    ztimer64_clock_t z64mock;
    ztimer64_clock_t *z64 = &z64mock;

    memset(&zmock, '\0', sizeof(ztimer_mock_t));
    memset(&z64mock, '\0', sizeof(ztimer64_clock_t));
    /* ztimer base clock is already extended to 32bit */
    ztimer_mock_init(&zmock, 32);
    ztimer64_clock_init(z64, z);

    ztimer64_t timer = { .base.target = 1 };

    ztimer64_set(z64, &timer, 100000000LLU);
    TEST_ASSERT(ztimer64_is_set(&timer));
    ztimer64_remove(z64, &timer);
    TEST_ASSERT(!ztimer64_is_set(&timer));
}

Test *tests_ztimer64_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ztimer64_now),
        new_TestFixture(test_ztimer64_set),
        new_TestFixture(test_ztimer64_set_0),
        new_TestFixture(test_ztimer64_set_at),
        new_TestFixture(test_ztimer64_checkpoint),
        new_TestFixture(test_ztimer64_set_uninitialized),
        new_TestFixture(test_ztimer64_remove_clear),
    };

    EMB_UNIT_TESTCALLER(ztimer64_tests, setup, NULL, fixtures);

    return (Test *)&ztimer64_tests;
}

/** @} */
