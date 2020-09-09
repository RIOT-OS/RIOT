/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>
#include <stdint.h>
#include <stdatomic.h>

#include "embUnit.h"

#include "tests-core.h"

/* Test atomic_flag back and forth */
static void test_atomic_flag(void)
{
    atomic_flag flag = ATOMIC_FLAG_INIT;

    TEST_ASSERT_EQUAL_INT(0, atomic_flag_test_and_set(&flag));
    TEST_ASSERT_EQUAL_INT(1, atomic_flag_test_and_set(&flag));
    atomic_flag_clear(&flag);
    TEST_ASSERT_EQUAL_INT(0, atomic_flag_test_and_set(&flag));
}

/* Prevent compiler optimization for SAML1X because of gcc internal bug */
#ifdef CPU_CORE_CORTEX_M23
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif
/* Test atomic_fetch_add */
static void test_atomic_inc_positive(void)
{
    atomic_int res = ATOMIC_VAR_INIT(0);

    TEST_ASSERT_EQUAL_INT(0, atomic_fetch_add(&res, 1));
    TEST_ASSERT_EQUAL_INT(1, atomic_load(&res));
    TEST_ASSERT_EQUAL_INT(1, atomic_fetch_add(&res, 1));
    TEST_ASSERT_EQUAL_INT(2, atomic_load(&res));
    atomic_store(&res, 0);
    for (int i = 0; i < 512; ++i) {
        TEST_ASSERT_EQUAL_INT(i, atomic_fetch_add(&res, 1));
        TEST_ASSERT_EQUAL_INT(i + 1, atomic_load(&res));
    }
}

static void test_atomic_inc_negative(void)
{
    atomic_int res = ATOMIC_VAR_INIT(-99);

    for (int i = -99; i < 123; ++i) {
        TEST_ASSERT_EQUAL_INT(i, atomic_fetch_add(&res, 1));
        TEST_ASSERT_EQUAL_INT(i + 1, atomic_load(&res));
    }
}

static void test_atomic_inc_rollover(void)
{
    atomic_int res = ATOMIC_VAR_INIT(INT_MAX - 30);

    for (int i = 0; i < 30; ++i) {
        TEST_ASSERT_EQUAL_INT(INT_MAX - 30 + i, atomic_fetch_add(&res, 1));
        TEST_ASSERT_EQUAL_INT(INT_MAX - 30 + i + 1, atomic_load(&res));
    }
    TEST_ASSERT_EQUAL_INT(INT_MAX, atomic_fetch_add(&res, 1));
    TEST_ASSERT_EQUAL_INT(INT_MIN, atomic_load(&res));
    TEST_ASSERT_EQUAL_INT(INT_MIN, atomic_fetch_add(&res, 1));
    TEST_ASSERT_EQUAL_INT(INT_MIN + 1, atomic_load(&res));
    TEST_ASSERT_EQUAL_INT(INT_MIN + 1, atomic_fetch_add(&res, 1));
    TEST_ASSERT_EQUAL_INT(INT_MIN + 2, atomic_load(&res));
}
#ifdef CPU_SAML1X
#pragma GCC pop_options
#endif
/* Test atomic_fetch_sub */
static void test_atomic_dec_negative(void)
{
    atomic_int res = ATOMIC_VAR_INIT(0);

    TEST_ASSERT_EQUAL_INT(0, atomic_fetch_sub(&res, 1));
    TEST_ASSERT_EQUAL_INT(-1, atomic_load(&res));
    TEST_ASSERT_EQUAL_INT(-1, atomic_fetch_sub(&res, 1));
    TEST_ASSERT_EQUAL_INT(-2, atomic_load(&res));
    atomic_store(&res, 0);
    for (int i = 0; i < 512; ++i) {
        TEST_ASSERT_EQUAL_INT(-i, atomic_fetch_sub(&res, 1));
        TEST_ASSERT_EQUAL_INT(-i - 1, atomic_load(&res));
    }
}

static void test_atomic_dec_positive(void)
{
    atomic_int res = ATOMIC_VAR_INIT(99);

    for (int i = 99; i > -123; --i) {
        TEST_ASSERT_EQUAL_INT(i, atomic_fetch_sub(&res, 1));
        TEST_ASSERT_EQUAL_INT(i - 1, atomic_load(&res));
    }
}

static void test_atomic_dec_rollover(void)
{
    atomic_int res = ATOMIC_VAR_INIT(INT_MIN + 30);

    for (int i = 0; i < 30; ++i) {
        TEST_ASSERT_EQUAL_INT(INT_MIN + 30 - i, atomic_fetch_sub(&res, 1));
        TEST_ASSERT_EQUAL_INT(INT_MIN + 30 - i - 1, atomic_load(&res));
    }
    TEST_ASSERT_EQUAL_INT(INT_MIN, atomic_fetch_sub(&res, 1));
    TEST_ASSERT_EQUAL_INT(INT_MAX, atomic_load(&res));
    TEST_ASSERT_EQUAL_INT(INT_MAX, atomic_fetch_sub(&res, 1));
    TEST_ASSERT_EQUAL_INT(INT_MAX - 1, atomic_load(&res));
    TEST_ASSERT_EQUAL_INT(INT_MAX - 1, atomic_fetch_sub(&res, 1));
    TEST_ASSERT_EQUAL_INT(INT_MAX - 2, atomic_load(&res));
}

/* Test atomic_cas with a correct old value */
static void test_atomic_cas_same(void)
{
    atomic_int res = ATOMIC_VAR_INIT(0);
    int expected;

    expected = 0;
    TEST_ASSERT_EQUAL_INT(1, atomic_compare_exchange_weak(&res, &expected, 12345));
    TEST_ASSERT_EQUAL_INT(12345, atomic_load(&res));
    TEST_ASSERT_EQUAL_INT(0, expected);
    expected = 12345;
    TEST_ASSERT_EQUAL_INT(1, atomic_compare_exchange_weak(&res, &expected, -9876));
    TEST_ASSERT_EQUAL_INT(-9876, atomic_load(&res));
    TEST_ASSERT_EQUAL_INT(12345, expected);
    expected = -9876;
    TEST_ASSERT_EQUAL_INT(1, atomic_compare_exchange_weak(&res, &expected, -9876));
    TEST_ASSERT_EQUAL_INT(-9876, atomic_load(&res));
    TEST_ASSERT_EQUAL_INT(-9876, expected);
    expected = -9876;
    TEST_ASSERT_EQUAL_INT(1, atomic_compare_exchange_weak(&res, &expected, 0));
    TEST_ASSERT_EQUAL_INT(0, atomic_load(&res));
}

/* Test atomic_cas with a non-matching old value */
static void test_atomic_cas_diff(void)
{
    atomic_int res = ATOMIC_VAR_INIT(32767);
    int expected;

    expected = 22222;
    TEST_ASSERT_EQUAL_INT(0, atomic_compare_exchange_weak(&res, &expected, 12345));
    TEST_ASSERT_EQUAL_INT(32767, expected);
    TEST_ASSERT_EQUAL_INT(32767, atomic_load(&res));

    atomic_store(&res, -12345);
    expected = 12345;
    TEST_ASSERT_EQUAL_INT(0, atomic_compare_exchange_weak(&res, &expected, 12345));
    TEST_ASSERT_EQUAL_INT(-12345, expected);
    TEST_ASSERT_EQUAL_INT(-12345, atomic_load(&res));

    expected = 12345;
    TEST_ASSERT_EQUAL_INT(0, atomic_compare_exchange_weak(&res, &expected, 12345));
    TEST_ASSERT_EQUAL_INT(-12345, expected);
    TEST_ASSERT_EQUAL_INT(-12345, atomic_load(&res));

    expected = 12345;
    TEST_ASSERT_EQUAL_INT(0, atomic_compare_exchange_weak(&res, &expected, -12345));
    TEST_ASSERT_EQUAL_INT(-12345, expected);
    TEST_ASSERT_EQUAL_INT(-12345, atomic_load(&res));
}

/* Test atomic_load, atomic_store */
static void test_atomic_value(void)
{
    atomic_int res = ATOMIC_VAR_INIT(12345);

    TEST_ASSERT_EQUAL_INT(12345, atomic_load(&res));
    atomic_store(&res, 24332);
    TEST_ASSERT_EQUAL_INT(24332, atomic_load(&res));
}

/* ATOMIC_VAR_INIT is implicitly tested by the other tests */

Test *tests_core_atomic_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_atomic_flag),
        new_TestFixture(test_atomic_inc_positive),
        new_TestFixture(test_atomic_inc_negative),
        new_TestFixture(test_atomic_inc_rollover),
        new_TestFixture(test_atomic_dec_positive),
        new_TestFixture(test_atomic_dec_negative),
        new_TestFixture(test_atomic_dec_rollover),
        new_TestFixture(test_atomic_cas_same),
        new_TestFixture(test_atomic_cas_diff),
        new_TestFixture(test_atomic_value),
    };

    EMB_UNIT_TESTCALLER(core_atomic_tests, NULL, NULL,
                        fixtures);

    return (Test *)&core_atomic_tests;
}
