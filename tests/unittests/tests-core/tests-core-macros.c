/*
 * SPDX-FileCopyrightText: 2022 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include "embUnit.h"
#include "tests-core.h"
#include "macros/math.h"
#include "macros/utils.h"

static void test_max(void)
{
    TEST_ASSERT_EQUAL_INT(10, MAX(5, 10));
    TEST_ASSERT_EQUAL_INT(10, MAX(10, 5));

    // prove it works with non-integer types
    TEST_ASSERT(22.1 == MAX(22.1, 5.5));
}

static void test_min(void)
{
    TEST_ASSERT_EQUAL_INT(5, MIN(5, 10));
    TEST_ASSERT_EQUAL_INT(5, MIN(10, 5));

    // prove it works with non-integer types
    TEST_ASSERT(5.5 == MIN(22.1, 5.5));
}

static void test_abs(void)
{
    TEST_ASSERT_EQUAL_INT(22, ABS(22));
    TEST_ASSERT_EQUAL_INT(22, ABS(-22));

    // prove it works with non-integer types
    TEST_ASSERT(300.7 == ABS(-300.7));
}

static void test_limit(void)
{
    TEST_ASSERT_EQUAL_INT(5, LIMIT(5, -10, 10));
    TEST_ASSERT_EQUAL_INT(10, LIMIT(10, -10, 10));
    TEST_ASSERT_EQUAL_INT(10, LIMIT(22, -10, 10));
    TEST_ASSERT_EQUAL_INT(-10, LIMIT(-10, -10, 10));
    TEST_ASSERT_EQUAL_INT(-10, LIMIT(-22, -10, 10));

    // prove it works with non-integer types
    TEST_ASSERT(10.2 == LIMIT(22.2, -10.1, 10.2));
}

static void test_math_signof(void)
{
    TEST_ASSERT_EQUAL_INT(1, SIGNOF(3));
    TEST_ASSERT_EQUAL_INT(-1, SIGNOF(-3));
    TEST_ASSERT_EQUAL_INT(-1, SIGNOF(INT32_MIN));
    TEST_ASSERT_EQUAL_INT(1, SIGNOF(UINT32_MAX));
}

static void test_math_div_round(void)
{
    size_t a = 20;

    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND(a, 7));
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND(21, 7));
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND(22, 7));
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND(-20, -7));
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND(-21, -7));
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND(-22, -7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND(20, -7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND(21, -7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND(22, -7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND(-20, 7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND(-21, 7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND(-22, 7));
    TEST_ASSERT_EQUAL_INT(0, DIV_ROUND(1, (long long) UINT32_MAX));

}

static void test_math_div_round_up(void)
{
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND_UP(20, 7));
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND_UP(21, 7));
    TEST_ASSERT_EQUAL_INT(4, DIV_ROUND_UP(22, 7));
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND_UP(-20, -7));
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND_UP(-21, -7));
    TEST_ASSERT_EQUAL_INT(4, DIV_ROUND_UP(-22, -7));
    TEST_ASSERT_EQUAL_INT(-2, DIV_ROUND_UP(20, -7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND_UP(21, -7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND_UP(22, -7));
    TEST_ASSERT_EQUAL_INT(-2, DIV_ROUND_UP(-20, 7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND_UP(-21, 7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND_UP(-22, 7));
    TEST_ASSERT_EQUAL_INT(1, DIV_ROUND_UP(1, (long long) UINT32_MAX));
}

static void test_math_div_round_inf(void)
{
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND_INF(20, 7));
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND_INF(21, 7));
    TEST_ASSERT_EQUAL_INT(4, DIV_ROUND_INF(22, 7));
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND_INF(-20, -7));
    TEST_ASSERT_EQUAL_INT(3, DIV_ROUND_INF(-21, -7));
    TEST_ASSERT_EQUAL_INT(4, DIV_ROUND_INF(-22, -7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND_INF(20, -7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND_INF(21, -7));
    TEST_ASSERT_EQUAL_INT(-4, DIV_ROUND_INF(22, -7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND_INF(-20, 7));
    TEST_ASSERT_EQUAL_INT(-3, DIV_ROUND_INF(-21, 7));
    TEST_ASSERT_EQUAL_INT(-4, DIV_ROUND_INF(-22, 7));
    TEST_ASSERT_EQUAL_INT(1, DIV_ROUND_INF(1, UINT32_MAX));
    TEST_ASSERT_EQUAL_INT(-1, DIV_ROUND_INF(1, (long long) INT32_MIN));

    TEST_ASSERT_EQUAL_INT(1536, MATH_ALIGN(1514, 64));
}

Test *tests_core_macros_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_max),
        new_TestFixture(test_min),
        new_TestFixture(test_abs),
        new_TestFixture(test_limit),
        new_TestFixture(test_math_signof),
        new_TestFixture(test_math_div_round),
        new_TestFixture(test_math_div_round_up),
        new_TestFixture(test_math_div_round_inf),
   };

    EMB_UNIT_TESTCALLER(core_macros_tests, NULL, NULL, fixtures);

    return (Test *)&core_macros_tests;
}
