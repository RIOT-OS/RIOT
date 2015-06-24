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

#include "crypto/lexv2.h"
#include "tests-crypto.h"
#include "tests-crypto-lexv2.h"

/* XOR output digest for verification of test vectors */
static void xor_digest(uint8_t *stream, int streamsize, uint8_t *out, int outsize)
{
    int i;
    memset(out, 0, outsize);

    for (i = 0; i < streamsize; i++) {
        out[i % outsize] ^= stream[i];
    }
}

/* Test keysetup, ivsetup and cipher */
static int test_key_iv_cipher(char *key, char *iv, char *expected, uint32_t keysize)
{
    /* Temporary variables */
    int i;
    uint8_t buffer[BUFFER_SIZE];
    uint8_t output[BUFFER_SIZE];
    uint8_t digest[DIGEST_SIZE];

    lexv2_ctx ctx;

    /* Zero buffer */
    for (i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }

    /* Do the test */
    lexv2_keysetup(&ctx, (const uint8_t *)key, keysize);
    lexv2_ivsetup(&ctx, (const uint8_t *)iv);
    lexv2_process_bytes(&ctx, buffer, output, BUFFER_SIZE);

    /* XOR to verify test vector */
    xor_digest(output, BUFFER_SIZE, digest, DIGEST_SIZE);

    /* Test the digest */
    if (memcmp(digest, expected, DIGEST_SIZE) == 0) {
        return 1;
    }

    return 0;
}

/* LEXv2 cipher tests 1 */
static void test_crypto_lexv2_cipher_tc1(void)
{
    TEST_ASSERT(test_key_iv_cipher(TV1_0_KEY, NULL_IV, TV1_0_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV1_63_KEY, NULL_IV, TV1_63_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV1_126_KEY, NULL_IV, TV1_126_EXPECT, 128));
}

/* LEXv2 cipher tests 2 */
static void test_crypto_lexv2_cipher_tc2(void)
{
    TEST_ASSERT(test_key_iv_cipher(TV2_0_KEY, NULL_IV, TV2_0_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV2_126_KEY, NULL_IV, TV2_126_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV2_252_KEY, NULL_IV, TV2_252_EXPECT, 128));
}

/* LEXv2 cipher tests 3 */
static void test_crypto_lexv2_cipher_tc3(void)
{
    TEST_ASSERT(test_key_iv_cipher(TV3_0_KEY, NULL_IV, TV3_0_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV3_126_KEY, NULL_IV, TV3_126_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV3_252_KEY, NULL_IV, TV3_252_EXPECT, 128));
}

/* LEXv2 unittest caller */
Test *tests_crypto_lexv2_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_lexv2_cipher_tc1),
        new_TestFixture(test_crypto_lexv2_cipher_tc2),
        new_TestFixture(test_crypto_lexv2_cipher_tc3),
    };

    EMB_UNIT_TESTCALLER(crypto_lexv2_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_lexv2_tests;
}
