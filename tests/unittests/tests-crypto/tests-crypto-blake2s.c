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

#include "crypto/blake2s.h"
#include "tests-crypto.h"
#include "tests-crypto-blake2s.h"

static int gen_testhash(char *input, char *key, uint16_t input_length, char *test)
{
    uint8_t hash[BLAKE2S_OUTBYTES];

    blake2s(hash, input, key, BLAKE2S_OUTBYTES, input_length, BLAKE2S_KEYBYTES);

    if (memcmp(hash, test, sizeof(hash)) == 0) {
        return 1;
    }

    return 0;
}

static void test_crypto_blake2s_hash(void)
{
    TEST_ASSERT(gen_testhash(TV1_TEXT, TV1_KEY, TV1_TEXT_LEN, TV1_HASH));
    TEST_ASSERT(gen_testhash(TV2_TEXT, TV2_KEY, TV2_TEXT_LEN, TV2_HASH));
    TEST_ASSERT(gen_testhash(TV3_TEXT, TV3_KEY, TV3_TEXT_LEN, TV3_HASH));
}


Test *tests_crypto_blake2s_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_blake2s_hash),
    };

    EMB_UNIT_TESTCALLER(crypto_blake2s_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_blake2s_tests;
}
