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

#include "crypto/dragon.h"
#include "tests-crypto.h"
#include "tests-crypto-dragon.h"

/* Test keysetup, ivsetup and cipher */
static int test_key_iv_cipher(char *key, char *iv, char *expected, uint32_t keysize)
{
    /* Temporary variables */
    int i;
    uint8_t buffer[BUFFER_SIZE];
    uint8_t output[BUFFER_SIZE];

    dragon_ctx ctx;

    /* Zero buffer */
    for (i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }

    /* Do the test */
    dragon_keysetup(&ctx, (const uint8_t *)key, keysize);
    dragon_ivsetup(&ctx, (const uint8_t *)iv);
    dragon_process_bytes(&ctx, buffer, output, BUFFER_SIZE);

    /* Test the block */
    if (memcmp(output, expected, BUFFER_SIZE) == 0) {
        return 1;
    }

    return 0;
}

/* Dragon cipher tests 1 */
static void test_crypto_dragon_cipher_tc1(void)
{
    TEST_ASSERT(test_key_iv_cipher(TV1_KEY, TV1_IV, TV1_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV2_KEY, TV2_IV, TV2_EXPECT, 128));
}

/* Dragon cipher tests 2 */
static void test_crypto_dragon_cipher_tc2(void)
{
    TEST_ASSERT(test_key_iv_cipher(TV3_KEY, TV3_IV, TV3_EXPECT, 256));
    TEST_ASSERT(test_key_iv_cipher(TV4_KEY, TV4_IV, TV4_EXPECT, 256));
}

/* Dragon unittest caller */
Test *tests_crypto_dragon_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_dragon_cipher_tc1),
        new_TestFixture(test_crypto_dragon_cipher_tc2),
    };

    EMB_UNIT_TESTCALLER(crypto_dragon_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_dragon_tests;
}
