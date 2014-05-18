/*
 * Copyright (C) 2014 Nico von Geyso
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

#include <limits.h>

#include "embUnit/embUnit.h"
#include "crypto/ciphers.h"
#include "tests-crypto.h"

static uint8_t TEST_INP[] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
    0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
};

static void test_crypto_null_encrypt(void)
{
    cipher_t cipher;
    int err, cmp;
    uint8_t data[16];

    err = cipher_init(&cipher, CIPHER_NULL, NULL, 0);
    TEST_ASSERT_EQUAL_INT(0, err);

    err = cipher_encrypt(&cipher, TEST_INP, data);
    TEST_ASSERT_EQUAL_INT(0, err);

    cmp = compare(TEST_INP, data, 16);
    TEST_ASSERT_MESSAGE(1 == cmp , "wrong ciphertext");

}

//static void test_crypto_null_decrypt(void)
//{
//
//    cipher_context_t ctx;
//    int err;
//    uint8_t data[16];
//
//    err = aes_init(&ctx, AES_BLOCK_SIZE, TEST_0_KEY, AES_KEY_SIZE);
//    TEST_ASSERT_EQUAL_INT(1, err);
//
//    err = aes_decrypt(&ctx, TEST_0_ENC, data);
//    TEST_ASSERT_EQUAL_INT(1, err);
//    TEST_ASSERT_MESSAGE(1 == compare(TEST_0_INP, data, AES_BLOCK_SIZE), "wrong plaintext");
//
//    err = aes_init(&ctx, AES_BLOCK_SIZE, TEST_1_KEY, AES_KEY_SIZE);
//    TEST_ASSERT_EQUAL_INT(1, err);
//
//    err = aes_decrypt(&ctx, TEST_1_ENC, data);
//    TEST_ASSERT_EQUAL_INT(1, err);
//    TEST_ASSERT_MESSAGE(1 == compare(TEST_1_INP, data, AES_BLOCK_SIZE), "wrong plaintext");
//}

Test *tests_crypto_null_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_null_encrypt),
//        new_TestFixture(test_crypto_null_decrypt),
    };

    EMB_UNIT_TESTCALLER(crypto_null_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_null_tests;
}
