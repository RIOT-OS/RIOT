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

#include "crypto/hc128.h"
#include "tests-crypto.h"
#include "tests-crypto-hc128.h"

/* Test keysetup, ivsetup and cipher */
static int test_key_iv_cipher(char *key, char *iv, char *expected)
{
    /* Temporary variables */
    int i;
    uint8_t buffer[BUFFER_SIZE];

    hc128_ctx ctx;

    /* Zero buffer */
    for (i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }

    /* Do the test */
    hc128_keysetup(&ctx, (const uint8_t *)key, 128, 128);
    hc128_ivsetup(&ctx, (const uint8_t *)iv);
    hc128_process_bytes(&ctx, buffer, buffer, BUFFER_SIZE);

    /* Test the block */
    if (memcmp(buffer, expected, BUFFER_SIZE) == 0) {
        return 1;
    }

    return 0;
}

/* HC-128 cipher tests */
static void test_crypto_hc128(void)
{
    TEST_ASSERT(test_key_iv_cipher(TV1_KEY, TV1_IV, TV1_EXPECT));
    TEST_ASSERT(test_key_iv_cipher(TV2_KEY, TV2_IV, TV2_EXPECT));
    TEST_ASSERT(test_key_iv_cipher(TV3_KEY, TV3_IV, TV3_EXPECT));
    TEST_ASSERT(test_key_iv_cipher(TV4_KEY, TV4_IV, TV4_EXPECT));
    TEST_ASSERT(test_key_iv_cipher(TV5_KEY, TV5_IV, TV5_EXPECT));
}

/* HC-128 unittest caller */
Test *tests_crypto_hc128_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_hc128),
    };

    EMB_UNIT_TESTCALLER(crypto_hc128_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_hc128_tests;
}
