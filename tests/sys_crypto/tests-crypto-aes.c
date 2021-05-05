/*
 * Copyright (C) 2015 Nico von Geyso
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>

#include "embUnit.h"
#include "crypto/aes.h"
#include "tests-crypto.h"

static uint8_t TEST_0_KEY[] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
    0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
};

static uint8_t TEST_0_INP[] = {
    0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
};
static uint8_t TEST_0_ENC[] = {
    0x37, 0x29, 0xa3, 0x6c, 0xaf, 0xe9, 0x84, 0xff,
    0x46, 0x22, 0x70, 0x42, 0xee, 0x24, 0x83, 0xf6
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
    0xD7, 0x9A, 0x54, 0x0E, 0x61, 0x33, 0x03, 0x72,
    0x59, 0x0f, 0x87, 0x91, 0xEF, 0xB0, 0xF8, 0x16
};

static void test_crypto_aes_encrypt(void)
{
    cipher_context_t ctx;
    int err;
    uint8_t data[AES_BLOCK_SIZE];

    err = aes_init(&ctx, TEST_0_KEY, sizeof(TEST_0_KEY));
    TEST_ASSERT_EQUAL_INT(1, err);

    err = aes_encrypt(&ctx, TEST_0_INP, data);
    TEST_ASSERT_EQUAL_INT(1, err);
    TEST_ASSERT_MESSAGE(1 == compare(TEST_0_ENC, data,
                                     AES_BLOCK_SIZE), "wrong ciphertext");

    err = aes_init(&ctx, TEST_1_KEY, sizeof(TEST_1_KEY));
    TEST_ASSERT_EQUAL_INT(1, err);

    err = aes_encrypt(&ctx, TEST_1_INP, data);
    TEST_ASSERT_EQUAL_INT(1, err);
    TEST_ASSERT_MESSAGE(1 == compare(TEST_1_ENC, data,
                                     AES_BLOCK_SIZE), "wrong ciphertext");
}

static void test_crypto_aes_decrypt(void)
{
    cipher_context_t ctx;
    int err;
    uint8_t data[AES_BLOCK_SIZE];

    err = aes_init(&ctx, TEST_0_KEY, sizeof(TEST_0_KEY));
    TEST_ASSERT_EQUAL_INT(1, err);

    err = aes_decrypt(&ctx, TEST_0_ENC, data);
    TEST_ASSERT_EQUAL_INT(1, err);
    TEST_ASSERT_MESSAGE(1 == compare(TEST_0_INP, data,
                                     AES_BLOCK_SIZE), "wrong plaintext");

    err = aes_init(&ctx, TEST_1_KEY, sizeof(TEST_1_KEY));
    TEST_ASSERT_EQUAL_INT(1, err);

    err = aes_decrypt(&ctx, TEST_1_ENC, data);
    TEST_ASSERT_EQUAL_INT(1, err);
    TEST_ASSERT_MESSAGE(1 == compare(TEST_1_INP, data,
                                     AES_BLOCK_SIZE), "wrong plaintext");
}

static void test_crypto_aes_init_key_length(void)
{
    cipher_context_t ctx;
    int err;

    /* A keylength of 64 bit is not supported by AES */
    uint8_t unsupported_key_1[8];
    memset(unsupported_key_1, 0, sizeof(unsupported_key_1));

    err = aes_init(&ctx, unsupported_key_1, sizeof(unsupported_key_1));
    TEST_ASSERT_EQUAL_INT(CIPHER_ERR_INVALID_KEY_SIZE, err);
}

Test *tests_crypto_aes_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_aes_encrypt),
        new_TestFixture(test_crypto_aes_decrypt),
        new_TestFixture(test_crypto_aes_init_key_length),
    };

    EMB_UNIT_TESTCALLER(crypto_aes_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_aes_tests;
}
