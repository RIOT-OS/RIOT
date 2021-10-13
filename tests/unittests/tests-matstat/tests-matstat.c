/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>
#include "embUnit.h"
#include "tests-matstat.h"

#include "matstat.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* White box testing of matstat library */

static void test_matstat_basic(void)
{
    /* nothing special, only verifying the basic functionality */
    matstat_state_t state = MATSTAT_STATE_INIT;
    matstat_add(&state, 10);
    TEST_ASSERT_EQUAL_INT(10, state.min);
    TEST_ASSERT_EQUAL_INT(10, state.max);
    TEST_ASSERT_EQUAL_INT(1, state.count);
    matstat_add(&state, 20);
    TEST_ASSERT_EQUAL_INT(10, state.min);
    TEST_ASSERT_EQUAL_INT(20, state.max);
    TEST_ASSERT_EQUAL_INT(2, state.count);
    matstat_add(&state, 30);
    TEST_ASSERT_EQUAL_INT(10, state.min);
    TEST_ASSERT_EQUAL_INT(30, state.max);
    TEST_ASSERT_EQUAL_INT(3, state.count);
    matstat_add(&state, 40);
    TEST_ASSERT_EQUAL_INT(10, state.min);
    TEST_ASSERT_EQUAL_INT(40, state.max);
    TEST_ASSERT_EQUAL_INT(4, state.count);
    int32_t mean = matstat_mean(&state);
    TEST_ASSERT_EQUAL_INT(25, mean);
    uint64_t var = matstat_variance(&state);
    TEST_ASSERT_EQUAL_INT(166, var);
    matstat_clear(&state);
    TEST_ASSERT_EQUAL_INT(0, state.count);
}

static void test_matstat_var_stability(void)
{
    /* This test is designed to detect stability errors where the values are
     * located very close together, which should yield a very low variance. */
    /* The initial implementation of the variance algorithm resulted in a very
     * large variance in this test, due to cancellation problems */
    matstat_state_t state = MATSTAT_STATE_INIT;
    matstat_add(&state,  999999);
    matstat_add(&state, 1000000);
    matstat_add(&state, 1000000);
    matstat_add(&state, 1000000);
    matstat_add(&state, 1000000);
    matstat_add(&state, 1000000);
    matstat_add(&state, 1000000);
    matstat_add(&state, 1000000);
    matstat_add(&state, 1000000);
    matstat_add(&state, 1000000);
    int32_t mean = matstat_mean(&state);
    TEST_ASSERT(mean >=  999999);
    TEST_ASSERT(mean <= 1000000);
    uint64_t var = matstat_variance(&state);
    TEST_ASSERT(var <= 1);
}

static void test_matstat_negative_variance(void)
{
    /* This is a regression test for two related problems where the truncation
     * in the mean computation (integer division) causes the sum_sq value to become
     * negative, or the variance itself to become negative */
    matstat_state_t state = MATSTAT_STATE_INIT;
    matstat_add(&state, -1);
    matstat_add(&state, 0);
    uint64_t var = matstat_variance(&state);
    TEST_ASSERT_EQUAL_INT(0, var);
    matstat_clear(&state);
    matstat_add(&state, 1);
    matstat_add(&state, 0);
    matstat_add(&state, 0);
    matstat_add(&state, 0);
    var = matstat_variance(&state);
    TEST_ASSERT_EQUAL_INT(0, var);
    matstat_clear(&state);
    matstat_add(&state, 1234567);
    for (unsigned int k = 0; k < 9999; ++k) {
        matstat_add(&state, 1234567);
        matstat_add(&state, 1234566);
    }
    var = matstat_variance(&state);
    TEST_ASSERT_EQUAL_INT(0, var);
}

static void test_matstat_merge_basic(void)
{
    /* This is a basic test of the merging functionality without any "special" cases */
    matstat_state_t state1 = MATSTAT_STATE_INIT;
    matstat_state_t state2 = MATSTAT_STATE_INIT;
    matstat_state_t state_ref = MATSTAT_STATE_INIT;
    matstat_add(&state1,    2000);
    matstat_add(&state_ref, 2000);
    matstat_add(&state1,    1000);
    matstat_add(&state_ref, 1000);
    matstat_add(&state1,    2000);
    matstat_add(&state_ref, 2000);
    matstat_add(&state2,    2000);
    matstat_add(&state_ref, 2000);
    matstat_add(&state2,    2456);
    matstat_add(&state_ref, 2456);
    matstat_add(&state2,    1234);
    matstat_add(&state_ref, 1234);
    matstat_add(&state2,    5678);
    matstat_add(&state_ref, 5678);
    matstat_add(&state2,    9999);
    matstat_add(&state_ref, 9999);
    matstat_merge(&state1, &state2);
    TEST_ASSERT_EQUAL_INT(state_ref.min, state1.min);
    TEST_ASSERT_EQUAL_INT(state_ref.max, state1.max);
    TEST_ASSERT_EQUAL_INT(state_ref.count, state1.count);
    TEST_ASSERT_EQUAL_INT(state_ref.sum, state1.sum);
    int32_t mean = matstat_mean(&state1);
    int32_t mean_ref = matstat_mean(&state_ref);
    TEST_ASSERT_EQUAL_INT(mean_ref, mean);
}

static void test_matstat_merge_empty(void)
{
    /* Testing merging with one or more empty states */
    matstat_state_t state1 = MATSTAT_STATE_INIT;
    matstat_state_t state2 = MATSTAT_STATE_INIT;
    matstat_merge(&state1, &state2);
    TEST_ASSERT_EQUAL_INT(0, state1.count);
    TEST_ASSERT_EQUAL_INT(0, state2.count);
    TEST_ASSERT_EQUAL_INT(0, state1.sum);
    TEST_ASSERT_EQUAL_INT(0, state2.sum);
    TEST_ASSERT_EQUAL_INT(0, state1.sum_sq);
    TEST_ASSERT_EQUAL_INT(0, state2.sum_sq);
    matstat_add(&state1, 2000);
    matstat_add(&state1, 1000);
    matstat_add(&state1, 2000);
    matstat_merge(&state1, &state2);
    TEST_ASSERT_EQUAL_INT(1000, state1.min);
    TEST_ASSERT_EQUAL_INT(2000, state1.max);
    TEST_ASSERT_EQUAL_INT(3, state1.count);
    TEST_ASSERT_EQUAL_INT(0, state2.count);
    matstat_clear(&state1);
    TEST_ASSERT_EQUAL_INT(0, state1.count);
    matstat_add(&state2, 2000);
    matstat_add(&state2, 1000);
    matstat_add(&state2, 2000);
    TEST_ASSERT_EQUAL_INT(3, state2.count);
    matstat_merge(&state1, &state2);
    TEST_ASSERT_EQUAL_INT(1000, state1.min);
    TEST_ASSERT_EQUAL_INT(2000, state1.max);
    TEST_ASSERT_EQUAL_INT(3, state1.count);
}

static void test_matstat_merge_variance(void)
{
    /* This test should ensure that merging states from separate sequences will
     * yield correct results for the variance computation */
    matstat_state_t state1 = MATSTAT_STATE_INIT;
    matstat_state_t state2 = MATSTAT_STATE_INIT;
    matstat_state_t state_ref = MATSTAT_STATE_INIT;
    matstat_add(&state1,    2000);
    matstat_add(&state_ref, 2000);
    matstat_add(&state1,    1000);
    matstat_add(&state_ref, 1000);
    matstat_add(&state1,    2000);
    matstat_add(&state_ref, 2000);
    matstat_add(&state2,    9999);
    matstat_add(&state_ref, 9999);
    matstat_add(&state2,    2456);
    matstat_add(&state_ref, 2456);
    matstat_add(&state2,    1234);
    matstat_add(&state_ref, 1234);
    matstat_add(&state2,    5678);
    matstat_add(&state_ref, 5678);
    matstat_add(&state2,    9999);
    matstat_add(&state_ref, 9999);
    matstat_merge(&state1, &state2);
    uint64_t var = matstat_variance(&state1);
    uint64_t var_ref = matstat_variance(&state_ref);
    int64_t var_diff = var - var_ref;
    /* There will invariably be some loss of accuracy because of the integer
     * operations involved in the variance computation. */
    TEST_ASSERT(var_diff <  1000);
    TEST_ASSERT(var_diff > -1000);
    TEST_ASSERT_EQUAL_INT(state_ref.mean, state1.mean);
}

static void test_matstat_merge_variance_regr1(void)
{
    /* This is a regression check for an issue where the sum_sq variable became
     * negative after merging a sequence of states with different means, and
     * small but non-zero sum_sq values. */
    /* Numbers were taken from a stats dump from the bench_timers application */
    matstat_state_t inputs[] = {
        { .count = 2686, .sum = 5414, .sum_sq = 1380, .min = 1, .max = 3, .mean = 2 },
        { .count = 2643, .sum = 5272, .sum_sq = 3263, .min = 1, .max = 3, .mean = 1 },
        { .count = 2650, .sum = 5328, .sum_sq =  719, .min = 1, .max = 3, .mean = 2 },
        { .count = 2562, .sum = 5117, .sum_sq = 2756, .min = 1, .max = 3, .mean = 1 },
        { .count = 2579, .sum = 5157, .sum_sq =  635, .min = 1, .max = 3, .mean = 1 },
        { .count = 2533, .sum = 5050, .sum_sq = 2944, .min = 1, .max = 3, .mean = 1 },
        { .count = 2630, .sum = 5276, .sum_sq = 1078, .min = 1, .max = 3, .mean = 2 },
        { .count = 2667, .sum = 5333, .sum_sq =  974, .min = 1, .max = 3, .mean = 1 },
        { .count = 2414, .sum = 4859, .sum_sq = 1074, .min = 1, .max = 3, .mean = 2 },
    };
    matstat_state_t merged = MATSTAT_STATE_INIT;
    for (unsigned k = 0; k < ARRAY_SIZE(inputs); ++k) {
        matstat_merge(&merged, &inputs[k]);
    }
    int64_t var = (int64_t)matstat_variance(&merged);
    /* Expected variance for this input is 0, because of integer truncation of the result.
     * The bug gave the following result instead:
     * count = 23364, sum = 46806, sum_sq = 18446744073709540510, mean = 2, var = 789570863061659
     */
    /* Left here for debugging test case failures: */
    /* printf("\nmerged: count = %" PRIu32 ", sum = %" PRId64 ", sum_sq = %" PRIu64 ", "
        "mean = %" PRId32 ", var = %" PRIu64 "\n", merged.count, merged.sum,
        merged.sum_sq, merged.mean, var); */
    TEST_ASSERT((int64_t)merged.sum_sq > 0);
    TEST_ASSERT(var >= 0);
}

static void test_matstat_accuracy(void)
{
    /* This test verifies that the numeric accuracy is "good enough" */
    matstat_state_t state = MATSTAT_STATE_INIT;
    /*
     * The test values below were sampled from a normal distribution with
     * mean = 12345
     * standard deviation = 10000 => variance = 100000000
     *
     * The sample distribution, when computed with double precision floating
     * point values, is:
     * sample variance = 115969073.207895
     * sample mean = 12293.05
     */
    /* This test will fail unless the library adaptively adjusts the offset to
     * reduce the error in the variance */
    matstat_add(&state,    -9228);
    matstat_add(&state,     6225);
    matstat_add(&state,    15935);
    matstat_add(&state,     1171);
    matstat_add(&state,     9500);
    matstat_add(&state,    22805);
    matstat_add(&state,     6484);
    matstat_add(&state,    10157);
    matstat_add(&state,    23870);
    matstat_add(&state,     9010);
    matstat_add(&state,    16093);
    matstat_add(&state,    20969);
    matstat_add(&state,    18077);
    matstat_add(&state,     9202);
    matstat_add(&state,    20074);
    matstat_add(&state,    19236);
    matstat_add(&state,    32276);
    matstat_add(&state,     6342);
    matstat_add(&state,    18759);
    matstat_add(&state,   -11096);
    int32_t mean = matstat_mean(&state);
    uint64_t var = matstat_variance(&state);
    int64_t var_diff = var - 115969073;
    TEST_ASSERT(var_diff <  10000);
    TEST_ASSERT(var_diff > -10000);
    TEST_ASSERT_EQUAL_INT(12293, mean);
}

Test *tests_matstat_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_matstat_basic),
        new_TestFixture(test_matstat_var_stability),
        new_TestFixture(test_matstat_merge_basic),
        new_TestFixture(test_matstat_merge_empty),
        new_TestFixture(test_matstat_merge_variance),
        new_TestFixture(test_matstat_merge_variance_regr1),
        new_TestFixture(test_matstat_accuracy),
        new_TestFixture(test_matstat_negative_variance),
    };

    EMB_UNIT_TESTCALLER(matstat_tests, NULL, NULL, fixtures);

    return (Test *)&matstat_tests;
}

void tests_matstat(void)
{
    TESTS_RUN(tests_matstat_tests());
}
