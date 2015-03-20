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

#include "crypto/sha512.h"
#include "tests-crypto.h"
#include "tests-crypto-sha512.h"

static int gen_testhash(char *input, uint16_t length, char *test)
{
    int i;
    uint8_t hash[SHA512_HASH_SIZE];
    sha512_ctx ctx;

    sha512_init(&ctx);

    for (i = 0; i + SHA512_BLOCK_SIZE <= length; i += SHA512_BLOCK_SIZE) {
        sha512_block(&ctx, (const uint8_t *)input + i);
    }

    sha512_final(&ctx, (const uint8_t *)input + i, length);
    sha512_get(&ctx, hash, 0, SHA512_HASH_SIZE);

    if (memcmp(hash, test, sizeof(hash)) == 0) {
        return 1;
    }

    return 0;
}

static void test_crypto_sha512_hash(void)
{
    TEST_ASSERT(gen_testhash(TV1_TEXT, 0, TV1_HASH));
    TEST_ASSERT(gen_testhash(TV2_TEXT, strlen(TV2_TEXT), TV2_HASH));
    TEST_ASSERT(gen_testhash(TV3_TEXT, strlen(TV3_TEXT), TV3_HASH));
    TEST_ASSERT(gen_testhash(TV4_TEXT, strlen(TV4_TEXT), TV4_HASH));
}

Test *tests_crypto_sha512_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_sha512_hash),
    };

    EMB_UNIT_TESTCALLER(crypto_sha512_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_sha512_tests;
}
