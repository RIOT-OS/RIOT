/*
 * SPDX-FileCopyrightText: 2018 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for ztimer
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include "ztimer.h"
#include "ztimer/mock.h"

#include "embUnit/embUnit.h"

#include "tests-ztimer.h"

#include <stdio.h>
/**
 * @brief   Simple callback for counting alarms
 */
static void cb_incr(void *arg)
{
    uint32_t *ptr = arg;
    *ptr += 1;
}

/**
 * @brief   Testing 32 bit wide mock clock now functionality
 */
static void test_ztimer_mock_now32(void)
{
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;

    /* Basic sanity test of the mock implementation */
    ztimer_mock_init(&zmock, 32);
    uint32_t now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0, now);

    ztimer_mock_advance(&zmock, 123);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(123, now);

    ztimer_mock_jump(&zmock, 0x10000000ul);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0x10000000ul, now);

    ztimer_mock_advance(&zmock, 0x98765432ul);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0xa8765432ul, now);
    ztimer_mock_advance(&zmock, 0x41234567ul);
    ztimer_mock_advance(&zmock, 0x40000000ul);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0x29999999ul, now);
}

/**
 * @brief   Testing 16 bit wide mock clock now functionality
 */
static void test_ztimer_mock_now16(void)
{
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;

    /* testing a 16 bit counter */
    ztimer_mock_init(&zmock, 16);
    uint32_t now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0, now);

    ztimer_mock_advance(&zmock, 123);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(123, now);

    ztimer_mock_advance(&zmock, 30000ul);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(30123ul, now);

    ztimer_mock_advance(&zmock, 0x10000ul);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(30123ul + 0x10000, now);
    ztimer_mock_advance(&zmock, 0x8000ul);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(30123ul + 0x10000 + 0x8000, now);
}

/**
 * @brief   Testing 8 bit wide mock clock now functionality
 */
static void test_ztimer_mock_now8(void)
{
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;

    /* testing a small counter */
    ztimer_mock_init(&zmock, 8);
    uint32_t now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0, now);

    ztimer_mock_advance(&zmock, 123);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(123, now);

    ztimer_mock_advance(&zmock, 0x100);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0x100 + 123, now);

    ztimer_mock_advance(&zmock, 180);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0x100 + 123 + 180, now);
}

/**
 * @brief   Testing 3 bit wide mock clock now functionality
 */
static void test_ztimer_mock_now3(void)
{
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;

    /* testing a tiny counter */
    ztimer_mock_init(&zmock, 3);
    uint32_t now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0, now);

    ztimer_mock_advance(&zmock, 7);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(7, now);

    ztimer_mock_advance(&zmock, 8);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(7 + 8, now);

    ztimer_mock_advance(&zmock, 10);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(7 + 8 + 10, now);
}

/**
 * @brief   Testing 32 bit wide mock clock set functionality
 */
static void test_ztimer_mock_set32(void)
{
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;

    ztimer_mock_init(&zmock, 32);
    uint32_t now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0, now);

    uint32_t count = 0;
    ztimer_t alarm = { .callback = cb_incr, .arg = &count, };
    ztimer_set(z, &alarm, 1000);

    ztimer_mock_advance(&zmock,    1);    /* now =    1*/
    TEST_ASSERT_EQUAL_INT(0, count);
    ztimer_mock_advance(&zmock,  100);    /* now =  101 */
    TEST_ASSERT_EQUAL_INT(0, count);
    ztimer_mock_advance(&zmock,  898);    /* now =  999 */
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(999, now);
    TEST_ASSERT_EQUAL_INT(0, count);
    ztimer_mock_advance(&zmock,    1);    /* now = 1000*/
    TEST_ASSERT_EQUAL_INT(1, count);
    ztimer_mock_advance(&zmock,    1);    /* now = 1001*/
    TEST_ASSERT_EQUAL_INT(1, count);
    ztimer_mock_advance(&zmock, 1000);    /* now = 2001*/
    TEST_ASSERT_EQUAL_INT(1, count);
    ztimer_set(z, &alarm, 3);
    ztimer_mock_advance(&zmock,  999);    /* now = 3000*/
    TEST_ASSERT_EQUAL_INT(2, count);
    ztimer_set(z, &alarm, 4000001000ul);
    ztimer_mock_advance(&zmock, 1000);    /* now = 4000*/
    TEST_ASSERT_EQUAL_INT(2, count);
    ztimer_mock_advance(&zmock, 4000000000ul); /* now = 4000004000*/
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(4000004000ul, now);
    TEST_ASSERT_EQUAL_INT(3, count);
    ztimer_set(z, &alarm, 15);
    ztimer_mock_advance(&zmock,  14);
    ztimer_remove(z, &alarm);
    ztimer_mock_advance(&zmock, 1000);
    TEST_ASSERT_EQUAL_INT(3, count);
}

/**
 * @brief   Testing 16 bit wide mock clock set functionality
 */
static void test_ztimer_mock_set16(void)
{
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;

    ztimer_mock_init(&zmock, 16);

    /* make sure ztimer stays turned on */
    ztimer_acquire(z);

    uint32_t now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0, now);

    uint32_t count = 0;
    ztimer_t alarm = { .callback = cb_incr, .arg = &count, };
    ztimer_set(z, &alarm, 1000);

    ztimer_mock_advance(&zmock,    1);
    TEST_ASSERT_EQUAL_INT(1, ztimer_now(z));
    TEST_ASSERT_EQUAL_INT(0, count);
    ztimer_mock_advance(&zmock,  100);
    TEST_ASSERT_EQUAL_INT(1 + 100, ztimer_now(z));
    TEST_ASSERT_EQUAL_INT(0, count);
    ztimer_mock_advance(&zmock,  898);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(1 + 100 + 898, now);
    TEST_ASSERT_EQUAL_INT(0, count);
    ztimer_mock_advance(&zmock,    1);
    TEST_ASSERT_EQUAL_INT(1 + 100 + 898 + 1, ztimer_now(z));
    TEST_ASSERT_EQUAL_INT(1, count);
    ztimer_mock_advance(&zmock,    1);
    TEST_ASSERT_EQUAL_INT(1 + 100 + 898 + 1 + 1, ztimer_now(z));
    TEST_ASSERT_EQUAL_INT(1, count);
    ztimer_mock_advance(&zmock, 1000);
    TEST_ASSERT_EQUAL_INT(1 + 100 + 898 + 1 + 1 + 1000, ztimer_now(z));
    TEST_ASSERT_EQUAL_INT(1, count);
    ztimer_set(z, &alarm, UINT16_MAX);
    ztimer_mock_advance(&zmock, 0x10000ul);
    /* 1 + 100 + 898 + 1 + 1 + 1000 + 0x10000 = 67537 */
    TEST_ASSERT_EQUAL_INT(67537ul, ztimer_now(z));
    TEST_ASSERT_EQUAL_INT(2, count);
    ztimer_set(z, &alarm, UINT16_MAX);
    ztimer_mock_advance(&zmock, 0x10000000ul);
    TEST_ASSERT_EQUAL_INT(67537ul + 0x10000000ul, ztimer_now(z));
    TEST_ASSERT_EQUAL_INT(3, count);
    ztimer_set(z, &alarm, 0x10001ul);
    ztimer_mock_advance(&zmock, 1);
    TEST_ASSERT_EQUAL_INT(67537ul + 0x10000000ul + 1, ztimer_now(z));
    TEST_ASSERT_EQUAL_INT(3, count);
    ztimer_mock_advance(&zmock, UINT16_MAX);
    TEST_ASSERT_EQUAL_INT(67537ul + 0x10000000ul + 1 + UINT16_MAX, ztimer_now(z));
    TEST_ASSERT_EQUAL_INT(3, count);
    ztimer_mock_advance(&zmock, 0x1);
    TEST_ASSERT_EQUAL_INT(67537ul + 0x10000000ul + 1 + UINT16_MAX + 1, ztimer_now(z));
    TEST_ASSERT_EQUAL_INT(4, count);
    now = ztimer_now(z);
    /* 67537ul + 0x10000000ul + 1 + UINT16_MAX + 1 = 0x100207d2 */
    TEST_ASSERT_EQUAL_INT(0x100207d2, now);
}

/**
 * @brief   Testing ztimer_is_set()
 */
static void test_ztimer_mock_is_set(void)
{
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;

    /* Basic sanity test of the mock implementation */
    ztimer_mock_init(&zmock, 32);

    uint32_t count = 0;
    ztimer_t alarm = { .callback = cb_incr, .arg = &count, };
    ztimer_t alarm2 = { .callback = cb_incr, .arg = &count, };

    ztimer_set(z, &alarm, 1000);
    ztimer_set(z, &alarm2, 2000);

    TEST_ASSERT_EQUAL_INT(0, count);
    TEST_ASSERT(ztimer_is_set(z, &alarm));
    TEST_ASSERT(ztimer_is_set(z, &alarm2));

    ztimer_mock_advance(&zmock, 1000);

    TEST_ASSERT_EQUAL_INT(1, count);
    TEST_ASSERT(!ztimer_is_set(z, &alarm));
    TEST_ASSERT(ztimer_is_set(z, &alarm2));

    ztimer_mock_advance(&zmock, 1000);

    TEST_ASSERT_EQUAL_INT(2, count);
    TEST_ASSERT(!ztimer_is_set(z, &alarm));
    TEST_ASSERT(!ztimer_is_set(z, &alarm2));
}

static uint32_t calc_target_time(ztimer_mock_t *mock, ztimer_t *t)
{
    ztimer_base_t *target = &t->base;
    ztimer_base_t *head = mock->super.list.next;

    uint32_t timeout = mock->now + mock->target;

    if (target == head) {
        return timeout;
    }

    for (ztimer_base_t *i = head->next; i != NULL; i = i->next) {
        timeout += i->offset;
        if (i == target) {
            return timeout;
        }
    }

    return 0;
}

/*
 * Testing that removing timers has no unintended site effects on unrelated
 * timers (e.g. that offsets are correctly updated).
 */
static void test_ztimer_mock_remove(void)
{
    ztimer_mock_t zmock;
    ztimer_clock_t *z = &zmock.super;

    /* Basic sanity test of the mock implementation */
    ztimer_mock_init(&zmock, 32);

    uint32_t count = 0;
    const uint32_t offset = 1000;
    ztimer_t alarms[] = {
        { .callback = cb_incr, .arg = &count },
        { .callback = cb_incr, .arg = &count },
        { .callback = cb_incr, .arg = &count },
        { .callback = cb_incr, .arg = &count },
    };
    uint32_t abs_targets[ARRAY_SIZE(alarms)];

    for (unsigned i = 0; i < ARRAY_SIZE(alarms); i++) {
        ztimer_set(z, &alarms[i], (i + 1) * offset);
    }

    /* target of first timer should be `offset` and it should be armed */
    TEST_ASSERT(zmock.armed);
    TEST_ASSERT_EQUAL_INT(offset, zmock.target - zmock.now);

    /* relative offset from previous timer to alarm should always  be `offset` */
    for (unsigned i = 0; i < ARRAY_SIZE(alarms); i++) {
        TEST_ASSERT_EQUAL_INT(offset, alarms[i].base.offset);
        abs_targets[i] = zmock.now + (i + 1) * offset;
    }

    /* check order is correct */
    for (unsigned i = 0; i < ARRAY_SIZE(alarms) - 1; i++) {
        TEST_ASSERT(alarms[i].base.next == &alarms[i + 1].base);
    }

    /* ensure target time for 3rd and 4th timer are correct */
    TEST_ASSERT_EQUAL_INT(abs_targets[2], calc_target_time(&zmock, &alarms[2]));
    TEST_ASSERT_EQUAL_INT(abs_targets[3], calc_target_time(&zmock, &alarms[3]));

    /* ensure target times are still correct after 2nd timer is removed */
    ztimer_remove(z, &alarms[1]);
    TEST_ASSERT_EQUAL_INT(abs_targets[2], calc_target_time(&zmock, &alarms[2]));
    TEST_ASSERT_EQUAL_INT(abs_targets[3], calc_target_time(&zmock, &alarms[3]));

    /* ensure target times are still correct after 1st timer fired */
    ztimer_mock_advance(&zmock, offset);
    TEST_ASSERT_EQUAL_INT(abs_targets[2], calc_target_time(&zmock, &alarms[2]));
    TEST_ASSERT_EQUAL_INT(abs_targets[3], calc_target_time(&zmock, &alarms[3]));
    TEST_ASSERT_EQUAL_INT(1, count);

    /* ensure that removing an already fired time does not break things */
    ztimer_remove(z, &alarms[0]);
    TEST_ASSERT_EQUAL_INT(abs_targets[2], calc_target_time(&zmock, &alarms[2]));
    TEST_ASSERT_EQUAL_INT(abs_targets[3], calc_target_time(&zmock, &alarms[3]));
    TEST_ASSERT_EQUAL_INT(1, count);

    /* ensure target time of 3rd timer is still correct after 4th timer is removed */
    ztimer_remove(z, &alarms[3]);
    TEST_ASSERT_EQUAL_INT(abs_targets[2], calc_target_time(&zmock, &alarms[2]));

    /* ensure remaining timer still fires */
    ztimer_mock_advance(&zmock, 3 * offset);
    TEST_ASSERT_EQUAL_INT(2, count);
}

Test *tests_ztimer_mock_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ztimer_mock_now32),
        new_TestFixture(test_ztimer_mock_now16),
        new_TestFixture(test_ztimer_mock_now8),
        new_TestFixture(test_ztimer_mock_now3),
        new_TestFixture(test_ztimer_mock_set32),
        new_TestFixture(test_ztimer_mock_set16),
        new_TestFixture(test_ztimer_mock_is_set),
        new_TestFixture(test_ztimer_mock_remove),
    };

    EMB_UNIT_TESTCALLER(ztimer_tests, NULL, NULL, fixtures);

    return (Test *)&ztimer_tests;
}

/** @} */
