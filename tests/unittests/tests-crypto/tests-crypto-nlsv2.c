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

#include "crypto/nlsv2.h"
#include "tests-crypto.h"
#include "tests-crypto-nlsv2.h"

/* Test keysetup, ivsetup and cipher */
static int test_key_iv_cipher(char *key, char *iv, char *expected, uint32_t keysize,
                              uint32_t ivsize)
{
    /* Temporary variables */
    int i;
    uint8_t buffer[BUFFER_SIZE];
    uint8_t output[BUFFER_SIZE];

    nlsv2_ctx ctx;

    /* Zero buffer */
    for (i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }

    /* Do the test */
    nlsv2_keysetup(&ctx, (const uint8_t *)key, keysize, ivsize);
    nlsv2_ivsetup(&ctx, (const uint8_t *)iv);
    nlsv2_process_bytes(&ctx, buffer, output, BUFFER_SIZE);

    /* Test the block */
    if (memcmp(output, expected, (BUFFER_SIZE / 5)) == 0) {
        return 1;
    }

    return 0;
}

/* NLSv2 cipher tests */
static void test_crypto_nlsv2_cipher(void)
{
    TEST_ASSERT(test_key_iv_cipher(TV1_KEY, TV1_NONCE, TV1_EXPECT, 128, 32));
}

/* NLSv2 unittest caller */
Test *tests_crypto_nlsv2_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_nlsv2_cipher),
    };

    EMB_UNIT_TESTCALLER(crypto_nlsv2_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_nlsv2_tests;
}
