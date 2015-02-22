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

#include "crypto/rabbit.h"
#include "tests-crypto.h"
#include "tests-crypto-rabbit.h"

/* Test keysetup and cipher */
static int test_key_cipher(char *key, char *expected)
{
    /* Temporary variables */
    int i;
    uint8_t buffer[BUFFER_SIZE];

    rabbit_ctx ctx;

    /* Zero buffer */
    for (i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }

    /* Do the test */
    rabbit_keysetup(&ctx, (const uint8_t *)key);
    rabbit_process_bytes(&ctx, buffer, buffer, BUFFER_SIZE);

    /* Test the block */
    if (memcmp(buffer, expected, EXPECT_SIZE) == 0) {
        return 1;
    }

    return 0;
}

/* Test keysetup, ivsetup and cipher */
static int test_key_iv_cipher(char *key, char *iv, char *expected)
{
    /* Temporary variables */
    int i;
    uint8_t buffer[BUFFER_SIZE];

    rabbit_ctx master_ctx, ctx;

    /* Zero buffer */
    for (i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }

    /* Do the test */
    rabbit_keysetup(&master_ctx, (const uint8_t *)key);
    rabbit_ivsetup(&master_ctx, &ctx, (const uint8_t *)iv);
    rabbit_process_bytes(&ctx, buffer, buffer, BUFFER_SIZE);

    /* Test the block */
    if (memcmp(buffer, expected, BUFFER_SIZE) == 0) {
        return 1;
    }

    return 0;
}


/* Test keysetup and prng */
static int test_key_prng(char *key, char *expected)
{
    /* Temporary variables */
    int i;
    uint8_t buffer[BUFFER_SIZE];

    rabbit_ctx ctx;

    /* Zero buffer */
    for (i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }

    /* Do the test */
    rabbit_keysetup(&ctx, (const uint8_t *)key);
    rabbit_prng(&ctx, buffer, BUFFER_SIZE);

    /* Test the block */
    if (memcmp(buffer, expected, EXPECT_SIZE) == 0) {
        return 1;
    }

    return 0;
}

/* Test keysetup, ivsetup and prng */
static int test_key_iv_prng(char *key, char *iv, char *expected)
{
    /* Temporary variables */
    int i;
    uint8_t buffer[BUFFER_SIZE];

    rabbit_ctx master_ctx, ctx;

    /* Zero buffer */
    for (i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }

    /* Do the test */
    rabbit_keysetup(&master_ctx, (const uint8_t *)key);
    rabbit_ivsetup(&master_ctx, &ctx, (const uint8_t *)iv);
    rabbit_prng(&ctx, buffer, BUFFER_SIZE);

    /* Test the block */
    if (memcmp(buffer, expected, BUFFER_SIZE) == 0) {
        return 1;
    }

    return 0;
}

/* Rabbit cipher tests 1 */
static void test_crypto_rabbit_cipher_tc1(void)
{
    TEST_ASSERT(test_key_cipher(TV1_KEY, TV1_EXPECT));
    TEST_ASSERT(test_key_cipher(TV2_KEY, TV2_EXPECT));
    TEST_ASSERT(test_key_cipher(TV3_KEY, TV3_EXPECT));
}

/* Rabbit cipher tests 2 */
static void test_crypto_rabbit_cipher_tc2(void)
{
    TEST_ASSERT(test_key_iv_cipher(TV4_KEY, TV4_IV, TV4_EXPECT));
    TEST_ASSERT(test_key_iv_cipher(TV5_KEY, TV5_IV, TV5_EXPECT));
    TEST_ASSERT(test_key_iv_cipher(TV6_KEY, TV6_IV, TV6_EXPECT));
}

/* Rabbit cipher tests 3 */
static void test_crypto_rabbit_cipher_tc3(void)
{
    TEST_ASSERT(test_key_prng(TV1_KEY, TV1_EXPECT));
    TEST_ASSERT(test_key_prng(TV2_KEY, TV2_EXPECT));
    TEST_ASSERT(test_key_prng(TV3_KEY, TV3_EXPECT));
}

/* Rabbit cipher tests 4 */
static void test_crypto_rabbit_cipher_tc4(void)
{
    TEST_ASSERT(test_key_iv_prng(TV1_KEY, TV4_IV, TV4_EXPECT));
    TEST_ASSERT(test_key_iv_prng(TV1_KEY, TV5_IV, TV5_EXPECT));
    TEST_ASSERT(test_key_iv_prng(TV1_KEY, TV6_IV, TV6_EXPECT));
}

/* Rabbit unittest caller */
Test *tests_crypto_rabbit_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_rabbit_cipher_tc1),
                        new_TestFixture(test_crypto_rabbit_cipher_tc2),
                        new_TestFixture(test_crypto_rabbit_cipher_tc3),
                        new_TestFixture(test_crypto_rabbit_cipher_tc4),
    };

    EMB_UNIT_TESTCALLER(crypto_rabbit_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_rabbit_tests;
}
