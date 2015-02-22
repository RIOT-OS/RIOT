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

#include "crypto/salsa20.h"
#include "tests-crypto.h"
#include "tests-crypto-salsa20.h"

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
    uint8_t digest[DIGEST_SIZE];

    salsa20_ctx ctx;

    /* Zero buffer */
    for (i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }

    /* Do the test */
    salsa20_keysetup(&ctx, (const uint8_t *)key, keysize);
    salsa20_ivsetup(&ctx, (const uint8_t *)iv);
    salsa20_process_bytes(&ctx, buffer, buffer, BUFFER_SIZE);

    /* XOR to verify test vector */
    xor_digest(buffer, BUFFER_SIZE, digest, DIGEST_SIZE);

    /* Test the digest */
    if (memcmp(digest, expected, DIGEST_SIZE) == 0) {
        return 1;
    }

    return 0;
}

/* Salsa20 cipher tests 128bit */
static void test_crypto_salsa20_crypto128(void)
{
    TEST_ASSERT(test_key_iv_cipher(TV128_1_0_KEY, NULL_IV, TV128_1_0_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV128_1_63_KEY, NULL_IV, TV128_1_63_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV128_1_126_KEY, NULL_IV, TV128_1_126_EXPECT, 128));

    TEST_ASSERT(test_key_iv_cipher(TV128_2_0_KEY, NULL_IV, TV128_2_0_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV128_2_126_KEY, NULL_IV, TV128_2_126_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV128_2_252_KEY, NULL_IV, TV128_2_252_EXPECT, 128));

    TEST_ASSERT(test_key_iv_cipher(TV128_3_0_KEY, NULL_IV, TV128_3_0_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV128_3_126_KEY, NULL_IV, TV128_3_126_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV128_3_252_KEY, NULL_IV, TV128_3_252_EXPECT, 128));

    TEST_ASSERT(test_key_iv_cipher(TV128_5_0_KEY, TV128_5_0_IV, TV128_5_0_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV128_5_36_KEY, TV128_5_36_IV, TV128_5_36_EXPECT, 128));
    TEST_ASSERT(test_key_iv_cipher(TV128_5_63_KEY, TV128_5_63_IV, TV128_5_63_EXPECT, 128));
}

/* Salsa20 cipher tests 256bit */
static void test_crypto_salsa20_crypto256(void)
{
    TEST_ASSERT(test_key_iv_cipher(TV256_1_0_KEY, NULL_IV, TV256_1_0_EXPECT, 256));
    TEST_ASSERT(test_key_iv_cipher(TV256_1_126_KEY, NULL_IV, TV256_1_126_EXPECT, 256));
    TEST_ASSERT(test_key_iv_cipher(TV256_1_252_KEY, NULL_IV, TV256_1_252_EXPECT, 256));


    TEST_ASSERT(test_key_iv_cipher(TV256_2_0_KEY, NULL_IV, TV256_2_0_EXPECT, 256));
    TEST_ASSERT(test_key_iv_cipher(TV256_2_126_KEY, NULL_IV, TV256_2_126_EXPECT, 256));
    TEST_ASSERT(test_key_iv_cipher(TV256_2_252_KEY, NULL_IV, TV256_2_252_EXPECT, 256));

    TEST_ASSERT(test_key_iv_cipher(TV256_3_0_KEY, NULL_IV, TV256_3_0_EXPECT, 256));
    TEST_ASSERT(test_key_iv_cipher(TV256_3_126_KEY, NULL_IV, TV256_3_126_EXPECT, 256));
    TEST_ASSERT(test_key_iv_cipher(TV256_3_252_KEY, NULL_IV, TV256_3_252_EXPECT, 256));

    TEST_ASSERT(test_key_iv_cipher(TV256_5_0_KEY, TV256_5_0_IV, TV256_5_0_EXPECT, 256));
    TEST_ASSERT(test_key_iv_cipher(TV256_5_36_KEY, TV256_5_36_IV, TV256_5_36_EXPECT, 256));
    TEST_ASSERT(test_key_iv_cipher(TV256_5_63_KEY, TV256_5_63_IV, TV256_5_63_EXPECT, 256));
}

/* Salsa20 unittest caller */
Test *tests_crypto_salsa20_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_salsa20_crypto128),
                        new_TestFixture(test_crypto_salsa20_crypto256),
    };

    EMB_UNIT_TESTCALLER(crypto_salsa20_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_salsa20_tests;
}
