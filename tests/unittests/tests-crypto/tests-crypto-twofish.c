/*
 * Copyright (C) 2015 Nico von Geyso
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>

#include "embUnit.h"
#include "crypto/twofish.h"
#include "tests-crypto.h"

static uint8_t TEST_0_KEY[] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
    0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
};

static uint8_t TEST_0_INP[] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
    0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
};
static uint8_t TEST_0_ENC[] = {
    0x9F, 0xB6, 0x33, 0x37, 0x15, 0x1B, 0xE9, 0xC7,
    0x13, 0x06, 0xD1, 0x59, 0xEA, 0x7A, 0xFA, 0xA4
};

static uint8_t TEST_1_KEY[] = {
    0x23, 0xA0, 0x18, 0x53, 0xFA, 0xB3, 0x89, 0x23,
    0x65, 0x89, 0x2A, 0xBC, 0x43, 0x99, 0xCC, 0x00
};

static uint8_t TEST_1_INP[] = {
    0x11, 0x53, 0x81, 0xE2, 0x5F, 0x33, 0xE7, 0x41,
    0xBB, 0x12, 0x67, 0x38, 0xE9, 0x12, 0x54, 0x23
};
static uint8_t TEST_1_ENC[] = {
    0xEA, 0x27, 0x44, 0xBB, 0x15, 0x56, 0xDB, 0xF3,
    0x33, 0xD4, 0x90, 0x09, 0x44, 0x7B, 0x83, 0x57
};

static void test_crypto_twofish_encrypt(void)
{
    cipher_context_t ctx;
    int err;
    uint8_t data[TWOFISH_BLOCK_SIZE];

    err = twofish_init(&ctx, TEST_0_KEY, TWOFISH_KEY_SIZE);
    TEST_ASSERT_EQUAL_INT(1, err);

    err = twofish_encrypt(&ctx, TEST_0_INP, data);
    TEST_ASSERT_EQUAL_INT(1, err);
    TEST_ASSERT_MESSAGE(1 == compare(TEST_0_ENC, data, TWOFISH_BLOCK_SIZE), "wrong ciphertext");

    err = twofish_init(&ctx, TEST_1_KEY, TWOFISH_KEY_SIZE);
    TEST_ASSERT_EQUAL_INT(1, err);

    err = twofish_encrypt(&ctx, TEST_1_INP, data);
    TEST_ASSERT_EQUAL_INT(1, err);
    TEST_ASSERT_MESSAGE(1 == compare(TEST_1_ENC, data, TWOFISH_BLOCK_SIZE), "wrong ciphertext");
}

static void test_crypto_twofish_decrypt(void)
{

    cipher_context_t ctx;
    int err;
    uint8_t data[TWOFISH_BLOCK_SIZE];

    err = twofish_init(&ctx, TEST_0_KEY, TWOFISH_KEY_SIZE);
    TEST_ASSERT_EQUAL_INT(1, err);

    err = twofish_decrypt(&ctx, TEST_0_ENC, data);
    TEST_ASSERT_EQUAL_INT(1, err);
    TEST_ASSERT_MESSAGE(1 == compare(TEST_0_INP, data, TWOFISH_BLOCK_SIZE), "wrong plaintext");

    err = twofish_init(&ctx, TEST_1_KEY, TWOFISH_KEY_SIZE);
    TEST_ASSERT_EQUAL_INT(1, err);

    err = twofish_decrypt(&ctx, TEST_1_ENC, data);
    TEST_ASSERT_EQUAL_INT(1, err);
    TEST_ASSERT_MESSAGE(1 == compare(TEST_1_INP, data, TWOFISH_BLOCK_SIZE), "wrong plaintext");
}

Test* tests_crypto_twofish_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_twofish_encrypt),
                        new_TestFixture(test_crypto_twofish_decrypt),
    };

    EMB_UNIT_TESTCALLER(crypto_twofish_tests, NULL, NULL, fixtures);

    return (Test*)&crypto_twofish_tests;
}
