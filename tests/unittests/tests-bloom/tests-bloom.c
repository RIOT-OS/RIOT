/*
 * SPDX-FileCopyrightText: 2014 Philipp Rosenkranz
 * SPDX-FileCopyrightText: 2013 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <string.h>
#include <stdio.h>

#include "tests-bloom.h"

#include "hashes.h"
#include "bloom.h"
#include "bitfield.h"

#include "tests-bloom-sets.h"

#define TESTS_BLOOM_BITS (128)
#define TESTS_BLOOM_HASHF (6)
#define TESTS_BLOOM_PROB_IN_FILTER (4)
#define TESTS_BLOOM_NOT_IN_FILTER (996)
#define TESTS_BLOOM_FALSE_POS_RATE_THR (0.005)

static bloom_t bloom;
BITFIELD(bf, TESTS_BLOOM_BITS);
hashfp_t hashes[TESTS_BLOOM_HASHF] = {
                     (hashfp_t) fnv_hash,
                     (hashfp_t) sax_hash,
                     (hashfp_t) sdbm_hash,
                     (hashfp_t) djb2_hash,
                     (hashfp_t) kr_hash,
                     (hashfp_t) dek_hash,
                    };

static void load_dictionary_fixture(void)
{
    for (int i = 0; i < lenB; i++)
    {
        bloom_add(&bloom, (const uint8_t *) B[i], strlen(B[i]));
    }

}

static void set_up_bloom(void)
{
    bloom_init(&bloom, TESTS_BLOOM_BITS, bf, hashes, TESTS_BLOOM_HASHF);
}

static void tear_down_bloom(void)
{
    bloom_del(&bloom);
}

static void test_bloom_parameters_bytes_hashf(void)
{
    TEST_ASSERT_EQUAL_INT(TESTS_BLOOM_BITS, bloom.m);
    TEST_ASSERT_EQUAL_INT(TESTS_BLOOM_HASHF, bloom.k);
}

static void test_bloom_based_on_dictionary_fixture(void)
{
    int in = 0;
    int not_in = 0;
    int false_positive_rate = 0;

    load_dictionary_fixture();

    for (int i = 0; i < lenA; i++)
    {
        if (bloom_check(&bloom, (const uint8_t *) A[i], strlen(A[i])))
        {
            in++;
        }
        else
        {
            not_in++;
        }
    }
    false_positive_rate = (1000 * in) / lenA;

    TEST_ASSERT_EQUAL_INT(TESTS_BLOOM_PROB_IN_FILTER, in);
    TEST_ASSERT_EQUAL_INT(TESTS_BLOOM_NOT_IN_FILTER, not_in);
    TEST_ASSERT(false_positive_rate < TESTS_BLOOM_FALSE_POS_RATE_THR * 1000);
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
