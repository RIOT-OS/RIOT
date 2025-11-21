/*
 * SPDX-FileCopyrightText: 2018 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for ztimer_convert_muldiv64
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include "ztimer.h"
#include "ztimer/mock.h"
#include "ztimer/convert_muldiv64.h"

#include "embUnit/embUnit.h"

#include "tests-ztimer.h"

#include <stdio.h>

static void test_ztimer_convert_muldiv64_now_helper(uint32_t div, uint32_t mul)
{
    ztimer_mock_t zmock;
    ztimer_convert_muldiv64_t zc;
    ztimer_clock_t *z = &zc.super.super;

    ztimer_mock_init(&zmock, 16);
    ztimer_convert_muldiv64_init(&zc, &zmock.super, div, mul);

    uint32_t last = 0;
    for (uint32_t i = 0; i <= 0xfffff; i++) {
        uint32_t now = ztimer_now(z);
        uint64_t should = (uint64_t)i * div;
        should /= mul;

        TEST_ASSERT(now >= last);

        TEST_ASSERT_EQUAL_INT(should, now);
        ztimer_mock_advance(&zmock, 1);
        last = now;
    }
}

/**
 * @brief   Basic checks for ztimer_convert_muldiv64
 */
static void test_ztimer_convert_muldiv64_now(void)
{
    test_ztimer_convert_muldiv64_now_helper(15625, 512);
}

static void _set_cb(void *arg)
{
    int *val = arg;
    *val = 1;
}

static void test_ztimer_convert_muldiv64_set_speedup(void)
{
    ztimer_mock_t zmock;
    ztimer_convert_muldiv64_t zc;
    ztimer_clock_t *z = &zc.super.super;
    unsigned val = 0;

    /* initialize 32bit mock timer */
    ztimer_mock_init(&zmock, 32);

    /* initialize convert to do "speedup" of the mock timer
     * (all now() values multiplied by 1000, all set() will be divided by
     * the same) */
    ztimer_convert_muldiv64_init(&zc, &zmock.super, 1000, 1);

    ztimer_t t = { .callback=_set_cb, .arg=&val };

    /* mock now() starts at 0, convert at 0 * 1000 = 0 */
    TEST_ASSERT_EQUAL_INT(0, ztimer_now(&zmock.super));
    TEST_ASSERT_EQUAL_INT(0, ztimer_now(z));

    /* set convert to 1 (integer divide would set this to 0 on lower timer, but
     * convert is supposed to do divide with ceiling) */
    ztimer_set(z, &t, 1);

    /* advance mock to 1 (convert should be at 1000) */
    ztimer_mock_advance(&zmock, 1);
    TEST_ASSERT_EQUAL_INT(1, ztimer_now(&zmock.super));
    TEST_ASSERT_EQUAL_INT(1000, ztimer_now(z));

    /* convert must have triggered t, as 1000 is > 1 */
    TEST_ASSERT_EQUAL_INT(1, val);

    /* reset helper variable */
    val = 0;

    /* set t to +500 (absolute: 1500) */
    ztimer_set(z, &t, 500);

    /* advance mock to 2 (convert: 2000) */
    ztimer_mock_advance(&zmock, 1);
    TEST_ASSERT_EQUAL_INT(2, ztimer_now(&zmock.super));
    TEST_ASSERT_EQUAL_INT(2000, ztimer_now(z));

    /* assert that timer triggered */
    TEST_ASSERT_EQUAL_INT(1, val);
    val = 0;

    /* set t to 4294967000 (absolute: 4294969000) */
    /* 4294967000 == (UINT32_MAX // 1000 * 1000) */
    ztimer_set(z, &t, 4294967000);

    /* advance mock to just before t's trigger time */
    /* this has overflowed convert */
    ztimer_mock_advance(&zmock, 4294966LU);
    TEST_ASSERT_EQUAL_INT(4294968LU, ztimer_now(&zmock.super));

    /* convert has overflowed ((2000 + 4294966000) % 2**32 = 704U)*/
    /* cast in case ztimer_now_t is uint64_t */
    TEST_ASSERT_EQUAL_INT(704LU, (uint32_t) ztimer_now(z));

    /* assert t hasn't triggered yet */
    TEST_ASSERT_EQUAL_INT(0, val);

    /* advance mock to 4294969 (convert to 4294969000) */
    ztimer_mock_advance(&zmock, 0x1);

    /* assert t has triggered */
    TEST_ASSERT_EQUAL_INT(1, val);
}

static void test_ztimer_convert_muldiv64_set_slowdown(void)
{
    ztimer_mock_t zmock;
    ztimer_convert_muldiv64_t zc;
    ztimer_clock_t *z = &zc.super.super;
    unsigned val = 0;

    /* initialize 32bit mock timer */
    ztimer_mock_init(&zmock, 32);

    /* initialize convert to do "slowdown" of the mock timer
     * (all now() values divided by 1000, all set() will be multiplied by
     * the same) */
    ztimer_convert_muldiv64_init(&zc, &zmock.super, 1, 1000);

    ztimer_t t = { .callback=_set_cb, .arg=&val };

    /* mock now() starts at 0, convert at 0 / 1000 = 0 */
    TEST_ASSERT_EQUAL_INT(0, ztimer_now(&zmock.super));
    TEST_ASSERT_EQUAL_INT(0, ztimer_now(z));

    /* set t on convert to 1 (should be 1000 on mock) */
    ztimer_set(z, &t, 1);

    /* advance mock to 999 (convert should be at (999/1000)==1 */
    ztimer_mock_advance(&zmock, 999);
    TEST_ASSERT_EQUAL_INT(999, ztimer_now(&zmock.super));
    TEST_ASSERT_EQUAL_INT(0, ztimer_now(z));

    /* convert must not have triggered */
    TEST_ASSERT_EQUAL_INT(0, val);

    /* advance mock to 1000 (convert: 1) */
    ztimer_mock_advance(&zmock, 1);
    TEST_ASSERT_EQUAL_INT(1000, ztimer_now(&zmock.super));
    TEST_ASSERT_EQUAL_INT(1, ztimer_now(z));
    /* assert that timer triggered */
    TEST_ASSERT_EQUAL_INT(1, val);
    val = 0;

    /* testing intermediate timer and (lack of) quantization */
    /* max value mock overflows at 4294967.xxx * 1000, so there will
     * be some intermediate timers.
     * when setting z at (mock=1000, z=1) to +4294968,
     * it should trigger at (mock=1704) */
    ztimer_set(z, &t, 4294968);

    ztimer_mock_advance(&zmock, UINT32_MAX);
    /* cast in case ztimer_now_t is uint64_t */
    TEST_ASSERT_EQUAL_INT(999, (uint32_t) ztimer_now(&zmock.super));

    /* ztimer_now(z) is now at (UINT32_MAX + 1000)/1000) == 4294968 */
    TEST_ASSERT_EQUAL_INT(4294968, ztimer_now(z));
    TEST_ASSERT_EQUAL_INT(0, val);

    ztimer_mock_advance(&zmock, 704);
    /* cast in case ztimer_now_t is uint64_t */
    TEST_ASSERT_EQUAL_INT(1703, (uint32_t) ztimer_now(&zmock.super));
    TEST_ASSERT_EQUAL_INT(4294969, ztimer_now(z));

    /* assert that this has not triggered yet. */
    TEST_ASSERT_EQUAL_INT(0, val);

    ztimer_mock_advance(&zmock, 1);
    /* cast in case ztimer_now_t is uint64_t */
    TEST_ASSERT_EQUAL_INT(1704, (uint32_t) ztimer_now(&zmock.super));
    TEST_ASSERT_EQUAL_INT(4294969, ztimer_now(z));
    TEST_ASSERT_EQUAL_INT(1, val);
    val = 0;
}

Test *tests_ztimer_convert_muldiv64_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ztimer_convert_muldiv64_set_speedup),
        new_TestFixture(test_ztimer_convert_muldiv64_set_slowdown),
        new_TestFixture(test_ztimer_convert_muldiv64_now),
    };

    EMB_UNIT_TESTCALLER(ztimer_tests, NULL, NULL, fixtures);

    return (Test *)&ztimer_tests;
}

/** @} */
