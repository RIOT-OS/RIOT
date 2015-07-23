/*
 * Copyright (C) 2015 Nico von Geyso
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>

#include "embUnit.h"
#include "crypto/3des.h"
#include "tests-crypto.h"

static uint8_t TEST_0_KEY[] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
    0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
};

static uint8_t TEST_0_INP[] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7
};
static uint8_t TEST_0_ENC[] = {
    0xDF, 0x0B, 0x6C, 0x9C, 0x31, 0xCD, 0x0C, 0xE4
};

static uint8_t TEST_1_KEY[] = {
    0x23, 0xA0, 0x18, 0x53, 0xFA, 0xB3, 0x89, 0x23,
    0x65, 0x89, 0x2A, 0xBC, 0x43, 0x99, 0xCC, 0x00
};

static uint8_t TEST_1_INP[] = {
    0x11, 0x53, 0x81, 0xE2, 0x5F, 0x33, 0xE7, 0x41
};
static uint8_t TEST_1_ENC[] = {
    0x6C, 0x71, 0x5C, 0xC2, 0x58, 0x13, 0xEC, 0x6E
};

static void test_crypto_tripledes_encrypt(void)
{
    cipher_context_t ctx;
    int err;
    uint8_t data[THREEDES_BLOCK_SIZE];

    err = tripledes_init(&ctx, TEST_0_KEY, 16);
    TEST_ASSERT_EQUAL_INT(1, err);

    err = tripledes_encrypt(&ctx, TEST_0_INP, data);
    TEST_ASSERT_EQUAL_INT(1, err);
    TEST_ASSERT_MESSAGE(1 == compare(TEST_0_ENC, data, THREEDES_BLOCK_SIZE), "wrong ciphertext");

    err = tripledes_init(&ctx, TEST_1_KEY, 16);
    TEST_ASSERT_EQUAL_INT(1, err);

    err = tripledes_encrypt(&ctx, TEST_1_INP, data);
    TEST_ASSERT_EQUAL_INT(1, err);
    TEST_ASSERT_MESSAGE(1 == compare(TEST_1_ENC, data, THREEDES_BLOCK_SIZE), "wrong ciphertext");
}

static void test_crypto_tripledes_decrypt(void)
{

    cipher_context_t ctx;
    int err;
    uint8_t data[THREEDES_BLOCK_SIZE];

    err = tripledes_init(&ctx, TEST_0_KEY, 16);
    TEST_ASSERT_EQUAL_INT(1, err);

    err = tripledes_decrypt(&ctx, TEST_0_ENC, data);
    TEST_ASSERT_EQUAL_INT(1, err);
    TEST_ASSERT_MESSAGE(1 == compare(TEST_0_INP, data, THREEDES_BLOCK_SIZE), "wrong plaintext");

    err = tripledes_init(&ctx, TEST_1_KEY, 16);
    TEST_ASSERT_EQUAL_INT(1, err);

    err = tripledes_decrypt(&ctx, TEST_1_ENC, data);
    TEST_ASSERT_EQUAL_INT(1, err);
    TEST_ASSERT_MESSAGE(1 == compare(TEST_1_INP, data, THREEDES_BLOCK_SIZE), "wrong plaintext");
}

Test* tests_crypto_3des_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_tripledes_encrypt),
                        new_TestFixture(test_crypto_tripledes_decrypt),
    };

    EMB_UNIT_TESTCALLER(crypto_tripledes_tests, NULL, NULL, fixtures);

    return (Test*)&crypto_tripledes_tests;
}
