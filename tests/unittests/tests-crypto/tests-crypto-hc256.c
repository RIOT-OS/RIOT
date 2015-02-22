/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "embUnit/embUnit.h"

#include "crypto/hc256.h"
#include "tests-crypto.h"
#include "tests-crypto-hc256.h"

/* Test keysetup, ivsetup and cipher */
static int test_key_iv_cipher(char *key, char *iv, char *expected, uint32_t keysize)
{
    /* Temporary variables */
    int i;
    uint8_t buffer[BUFFER_SIZE];

    hc256_ctx ctx;

    /* Zero buffer */
    for (i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }

    /* Do the test */
    hc256_keysetup(&ctx, (const uint8_t *)key, keysize, keysize);
    hc256_ivsetup(&ctx, (const uint8_t *)iv);
    hc256_process_bytes(&ctx, buffer, buffer, BUFFER_SIZE);

    /* Test the block */
    if (memcmp(buffer, expected, BUFFER_SIZE) == 0) {
        return 1;
    }

    return 0;
}

/* HC-256 cipher tests 128bit */
static void test_crypto_hc256_tc1(void)
{
    TEST_ASSERT(test_key_iv_cipher(TV1_KEY, TV1_IV, TV1_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV2_KEY, TV2_IV, TV2_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV3_KEY, TV3_IV, TV3_EXPECT, 128));
}

/* HC-256 cipher tests 256bit */
static void test_crypto_hc256_tc2(void)
{
    TEST_ASSERT(test_key_iv_cipher(TV4_KEY, TV4_IV, TV4_EXPECT, 256));
    TEST_ASSERT(test_key_iv_cipher(TV5_KEY, TV5_IV, TV5_EXPECT, 256));
    TEST_ASSERT(test_key_iv_cipher(TV5_KEY, TV5_IV, TV5_EXPECT, 256));
    TEST_ASSERT(test_key_iv_cipher(TV5_KEY, TV5_IV, TV5_EXPECT, 256));
}

/* HC-256 unittest caller */
Test *tests_crypto_hc256_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_hc256_tc1),
                        new_TestFixture(test_crypto_hc256_tc2),
    };

    EMB_UNIT_TESTCALLER(crypto_hc256_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_hc256_tests;
}
