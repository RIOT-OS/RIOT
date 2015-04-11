/*
 * Copyright (C) 2014 Philipp Rosenkranz
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <string.h>
#include <stdio.h>

#include "tests-bloom.h"

#include "hashes.h"
#include "bloom.h"

#include "tests-bloom-sets.h"

#define TESTS_BLOOM_BITS (128)
#define TESTS_BLOOM_HASHF (6)
#define TESTS_BLOOM_PROB_IN_FILTER (4)
#define TESTS_BLOOM_NOT_IN_FILTER (996)
#define TESTS_BLOOM_FALSE_POS_RATE_THR (0.005)

static bloom_t *bloom;

static void load_dictionary_fixture(void)
{
    for (int i = 0; i < lenB; i++)
    {
        bloom_add(bloom, (const uint8_t *) B[i], strlen(B[i]));
    }

}

static void set_up_bloom(void)
{
    bloom = bloom_new(TESTS_BLOOM_BITS, TESTS_BLOOM_HASHF, fnv_hash, sax_hash,
            sdbm_hash, djb2_hash, kr_hash, dek_hash, rotating_hash,
            one_at_a_time_hash);
}

static void tear_down_bloom(void)
{
    bloom_del(bloom);
}

static void test_bloom_parameters_bytes_hashf(void)
{
    TEST_ASSERT_EQUAL_INT(TESTS_BLOOM_BITS, bloom->m);
    TEST_ASSERT_EQUAL_INT(TESTS_BLOOM_HASHF, bloom->k);
}

static void test_bloom_based_on_dictionary_fixture(void)
{
    int in = 0;
    int not_in = 0;
    double false_positive_rate = 0;

    load_dictionary_fixture();

    for (int i = 0; i < lenA; i++)
    {
        if (bloom_check(bloom, (const uint8_t *) A[i], strlen(A[i])))
        {
            in++;
        }
        else
        {
            not_in++;
        }
    }
    false_positive_rate = (double) in / (double) lenA;

    TEST_ASSERT_EQUAL_INT(TESTS_BLOOM_PROB_IN_FILTER, in);
    TEST_ASSERT_EQUAL_INT(TESTS_BLOOM_NOT_IN_FILTER, not_in);
    TEST_ASSERT(false_positive_rate < TESTS_BLOOM_FALSE_POS_RATE_THR);
}

Test *tests_bloom_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_bloom_parameters_bytes_hashf),
        new_TestFixture(test_bloom_based_on_dictionary_fixture),
    };

    EMB_UNIT_TESTCALLER(bloom_tests, set_up_bloom, tear_down_bloom, fixtures);

    return (Test *)&bloom_tests;
}

void tests_bloom(void)
{
    TESTS_RUN(tests_bloom_tests());
}
