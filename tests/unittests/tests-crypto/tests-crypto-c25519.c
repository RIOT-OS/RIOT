/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 * Copyright (C) 2014 Daniel Beer <dlbeer@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "embUnit/embUnit.h"

#include "crypto/c25519.h"
#include "tests-crypto.h"
#include "tests-crypto-c25519.h"

static int gen_testdh(void)
{
    uint8_t e1[C25519_EXPONENT_SIZE];
    uint8_t e2[C25519_EXPONENT_SIZE];
    uint8_t q1[F25519_SIZE];
    uint8_t q2[F25519_SIZE];
    uint8_t r1[F25519_SIZE];
    uint8_t r2[F25519_SIZE];

    f25519_copy(e1, (const uint8_t *)TV1_E);
    f25519_copy(e2, (const uint8_t *)TV2_E);

    /* Create private keys */
    c25519_prepare(e1);
    c25519_prepare(e2);

    /* Create public keys */
    c25519_smult(q1, c25519_base_x, e1);
    c25519_smult(q2, c25519_base_x, e2);

    /* Diffie-Hellman exchange */
    c25519_smult(r1, q2, e1);
    c25519_smult(r2, q1, e2);

    if (memcmp(r1, r2, sizeof(r1)) == 0) {
        return 1;
    }

    return 0;
}

static int gen_testvector(char *P, char *E, char *R)
{
    uint8_t r[F25519_SIZE];
    uint8_t e[F25519_SIZE];

    f25519_copy(e, (const uint8_t *)E);
    c25519_prepare(e);
    c25519_smult(r, (const uint8_t *)P, e);

    if (memcmp(r, R, sizeof(r)) == 0) {
        return 1;
    }

    return 0;
}

static void test_crypto_c25519_vectors(void)
{
    TEST_ASSERT(gen_testvector(TV1_P, TV1_E, TV1_R));
    TEST_ASSERT(gen_testvector(TV2_P, TV2_E, TV2_R));
    TEST_ASSERT(gen_testvector(TV3_P, TV3_E, TV3_R));
    TEST_ASSERT(gen_testvector(TV4_P, TV4_E, TV4_R));
    TEST_ASSERT(gen_testvector(TV5_P, TV5_E, TV5_R));
    TEST_ASSERT(gen_testvector(TV6_P, TV6_E, TV6_R));
    TEST_ASSERT(gen_testvector(TV7_P, TV7_E, TV7_R));
    TEST_ASSERT(gen_testvector(TV8_P, TV8_E, TV8_R));
    TEST_ASSERT(gen_testvector(TV9_P, TV9_E, TV9_R));
    TEST_ASSERT(gen_testvector(TV10_P, TV10_E, TV10_R));
    TEST_ASSERT(gen_testvector(TV11_P, TV11_E, TV11_R));
    TEST_ASSERT(gen_testvector(TV12_P, TV12_E, TV12_R));
    TEST_ASSERT(gen_testvector(TV13_P, TV13_E, TV13_R));
    TEST_ASSERT(gen_testvector(TV14_P, TV14_E, TV14_R));
    TEST_ASSERT(gen_testvector(TV15_P, TV15_E, TV15_R));
    TEST_ASSERT(gen_testvector(TV16_P, TV16_E, TV16_R));
    TEST_ASSERT(gen_testvector(TV17_P, TV17_E, TV17_R));
    TEST_ASSERT(gen_testvector(TV18_P, TV18_E, TV18_R));
    TEST_ASSERT(gen_testvector(TV19_P, TV19_E, TV19_R));
    TEST_ASSERT(gen_testvector(TV20_P, TV20_E, TV20_R));
    TEST_ASSERT(gen_testvector(TV21_P, TV21_E, TV21_R));
    TEST_ASSERT(gen_testvector(TV22_P, TV22_E, TV22_R));
    TEST_ASSERT(gen_testvector(TV23_P, TV23_E, TV23_R));
    TEST_ASSERT(gen_testvector(TV24_P, TV24_E, TV24_R));
    TEST_ASSERT(gen_testvector(TV25_P, TV25_E, TV25_R));
    TEST_ASSERT(gen_testvector(TV26_P, TV26_E, TV26_R));
    TEST_ASSERT(gen_testvector(TV27_P, TV27_E, TV27_R));
    TEST_ASSERT(gen_testvector(TV28_P, TV28_E, TV28_R));
    TEST_ASSERT(gen_testvector(TV29_P, TV29_E, TV29_R));
    TEST_ASSERT(gen_testvector(TV30_P, TV30_E, TV30_R));
    TEST_ASSERT(gen_testvector(TV31_P, TV31_E, TV31_R));
    TEST_ASSERT(gen_testvector(TV32_P, TV32_E, TV32_R));
}

static void test_crypto_c25519_dhs(void)
{
    TEST_ASSERT(gen_testdh());
}

Test *tests_crypto_c25519_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_c25519_vectors),
        new_TestFixture(test_crypto_c25519_dhs),
    };

    EMB_UNIT_TESTCALLER(crypto_c25519_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_c25519_tests;
}
