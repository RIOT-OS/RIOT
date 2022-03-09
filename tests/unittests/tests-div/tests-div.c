/*
* Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

#include <string.h>
#include "embUnit.h"
#include "tests-div.h"

#include "div.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static const uint32_t u32_test_values[] = {
    0,
    1,
    10,
    32,
    15625,
    15625LU*5,
    (15625LU*5)+1,
    0xffff,
    0xffff<<10,
    0xffffffff,
};

static const uint64_t u64_test_values[] = {
    11111111111ull,
    0xffffffffull+1,
    16383999997ull,
    16383999998ull,
    16383999999ull,
    16384000000ull,
    1048575999807ull,
    1048575999808ull,
    1048575999809ull,
    0xffffffffffffeeull,
    0x777777777777777ull,
    0x1111111111111111ull,
    0xffffffffffffffffull,
    0x8000000000000000ull,
};

/* These expected values for test_div_u64_by_15625div512
 * were computed from the expression (u64_test_values * 512) / 15625 using 128
 * bit integers. */
static const uint64_t u64_15625_512_expected_values[] = {
    364088888,
    140737488,
    536870911,
    536870911,
    536870911,
    536870912,
    34359738361,
    34359738361,
    34359738361,
    2361183241434822,
    17630168202713342,
    40297527320487639,
    604462909807314587,
    302231454903657293,
};

#define N_U32_VALS ARRAY_SIZE(u32_test_values)
#define N_U64_VALS ARRAY_SIZE(u64_test_values)

static void test_div_u64_by_15625(void)
{
    for (unsigned i = 0; i < N_U32_VALS; i++) {
        DEBUG("Dividing %12"PRIu32" by 15625...\n", u32_test_values[i]);
        TEST_ASSERT_EQUAL_INT(
            (uint64_t)u32_test_values[i] / 15625,
            div_u64_by_15625(u32_test_values[i]));
    }

    for (unsigned i = 0; i < N_U64_VALS; i++) {
        DEBUG("Dividing %12"PRIu64" by 15625...\n", u64_test_values[i]);
        TEST_ASSERT_EQUAL_INT(
            (uint64_t)u64_test_values[i] / 15625,
            div_u64_by_15625(u64_test_values[i]));
    }
}

static void test_div_u32_by_15625div512(void)
{
    for (unsigned i = 0; i < N_U32_VALS; i++) {
        DEBUG("Dividing %"PRIu32" by (15625/512)...\n", u32_test_values[i]);
        TEST_ASSERT_EQUAL_INT(
            (uint64_t)u32_test_values[i] * 512lu / 15625,
            div_u32_by_15625div512(u32_test_values[i]));
    }
}

static void test_div_u64_by_1000000(void)
{
    for (unsigned i = 0; i < N_U32_VALS; i++) {
        DEBUG("Dividing %"PRIu32" by 1000000...\n", u32_test_values[i]);
        TEST_ASSERT_EQUAL_INT(
            (uint64_t)u32_test_values[i] / 1000000lu,
            div_u64_by_1000000(u32_test_values[i]));
    }

    for (unsigned i = 0; i < N_U64_VALS; i++) {
        DEBUG("Dividing %"PRIu64" by 1000000...\n", u64_test_values[i]);
        TEST_ASSERT_EQUAL_INT(
            u64_test_values[i] / 1000000lu,
            div_u64_by_1000000(u64_test_values[i]));
    }
}

static void test_div_u64_by_15625div512(void)
{
    for (unsigned i = 0; i < N_U32_VALS; i++) {
        DEBUG("Dividing %"PRIu32" by (15625/512)...\n", u32_test_values[i]);
        TEST_ASSERT_EQUAL_INT(
            (uint64_t)u32_test_values[i] * 512lu / 15625,
            div_u64_by_15625div512(u32_test_values[i]));
    }

    for (unsigned i = 0; i < N_U64_VALS; i++) {
        DEBUG("Dividing %"PRIu64" by (15625/512)...\n", u64_test_values[i]);
        TEST_ASSERT_EQUAL_INT(
            u64_15625_512_expected_values[i],
            div_u64_by_15625div512(u64_test_values[i]));
    }
}

Test *tests_div_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_div_u64_by_15625),
        new_TestFixture(test_div_u32_by_15625div512),
        new_TestFixture(test_div_u64_by_15625div512),
        new_TestFixture(test_div_u64_by_1000000),
    };

    EMB_UNIT_TESTCALLER(div_tests, NULL, NULL, fixtures);

    return (Test *)&div_tests;
}

void tests_div(void)
{
    TESTS_RUN(tests_div_tests());
}
