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

#include "crypto/groestl256.h"
#include "tests-crypto.h"
#include "tests-crypto-groestl256.h"

static int gen_testhash(char *input, uint64_t input_length, char *test)
{
    uint8_t hash[CRYPTO_BYTES];

    groestl256(hash, (const uint8_t *)input, input_length);

    if (memcmp(hash, test, sizeof(hash)) == 0) {
        return 1;
    }

    return 0;
}

static void test_crypto_groestl256_hash(void)
{
    TEST_ASSERT(gen_testhash(TV1_TEXT, TV1_TEXT_LEN, TV1_HASH));
    TEST_ASSERT(gen_testhash(TV2_TEXT, TV2_TEXT_LEN, TV2_HASH));
}


Test *tests_crypto_groestl256_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_groestl256_hash),
    };

    EMB_UNIT_TESTCALLER(crypto_groestl256_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_groestl256_tests;
}
