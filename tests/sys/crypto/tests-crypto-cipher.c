/*
 * Copyright (C) 2015 Nico von Geyso
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>
#include <string.h>

#include "embUnit.h"
#include "crypto/ciphers.h"
#include "tests-crypto.h"

static uint8_t TEST_KEY[] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
    0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
};

static uint8_t TEST_INP[] = {
    0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7
};

static uint8_t TEST_ENC_AES[] = {
    0x37, 0x29, 0xa3, 0x6c, 0xaf, 0xe9, 0x84, 0xff,
    0x46, 0x22, 0x70, 0x42, 0xee, 0x24, 0x83, 0xf6
};

static void test_crypto_cipher_aes_encrypt(void)
{
    cipher_t cipher;
    int err, cmp;
    uint8_t data[16] = { 0 };

    err = cipher_init(&cipher, CIPHER_AES, TEST_KEY, 16);
    TEST_ASSERT_EQUAL_INT(1, err);

    err = cipher_encrypt(&cipher, TEST_INP, data);
    TEST_ASSERT_EQUAL_INT(1, err);

    cmp = compare(TEST_ENC_AES, data, 16);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");
}

static void test_crypto_cipher_aes_decrypt(void)
{
    cipher_t cipher;
    int err, cmp;
    uint8_t data[16];

    err = cipher_init(&cipher, CIPHER_AES, TEST_KEY, 16);
    TEST_ASSERT_EQUAL_INT(1, err);

    err = cipher_decrypt(&cipher, TEST_ENC_AES, data);
    TEST_ASSERT_EQUAL_INT(1, err);

    cmp = compare(TEST_INP, data, 16);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong plaintext");
}

static void test_crypto_cipher_init_aes_key_length(void)
{
    cipher_t cipher;
    int err;

    /* A keylength of 64 bit is not supported by AES */
    uint8_t unsupported_key_1[8];
    memset(unsupported_key_1, 0, sizeof(unsupported_key_1));

    err =
        cipher_init(&cipher, CIPHER_AES, unsupported_key_1,
                    sizeof(unsupported_key_1));
    TEST_ASSERT_EQUAL_INT(CIPHER_ERR_INVALID_KEY_SIZE, err);
}

Test *tests_crypto_cipher_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_cipher_aes_encrypt),
        new_TestFixture(test_crypto_cipher_aes_decrypt),
        new_TestFixture(test_crypto_cipher_init_aes_key_length),
    };

    EMB_UNIT_TESTCALLER(crypto_cipher_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_cipher_tests;
}
