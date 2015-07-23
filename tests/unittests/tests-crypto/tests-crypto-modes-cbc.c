/*
 * Copyright (C) 2015 Nico von Geyso
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "embUnit.h"
#include "crypto/ciphers.h"
#include "crypto/modes/cbc.h"
#include "tests-crypto.h"

/*
 * all test vectors are from "Recommendation for Block Cipher Modes of
 * Operation - Methods and Techniques" by Morris Dworkin / NIST
 *
 *   http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf
 */


/* PACKET VECTOR #1 (Page 27) */
static uint8_t TEST_1_KEY[] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};
static uint8_t TEST_1_KEY_LEN = 16;

static uint8_t TEST_1_IV[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

static uint8_t TEST_1_PLAIN[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
    0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
    0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};
static uint8_t TEST_1_PLAIN_LEN = 64;

static uint8_t TEST_1_CIPHER[] = {
    0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
    0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
    0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee,
    0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
    0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b,
    0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
    0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09,
    0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7
};
static uint8_t TEST_1_CIPHER_LEN = 64;

static void test_encrypt_op(uint8_t* key, uint8_t key_len, uint8_t iv[16],
                            uint8_t* input, uint8_t input_len, uint8_t* output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES_128, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_encrypt_cbc(&cipher, iv, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp , "wrong ciphertext");

}

static void test_decrypt_op(uint8_t* key, uint8_t key_len, uint8_t iv[16],
                            uint8_t* input, uint8_t input_len, uint8_t* output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES_128, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_decrypt_cbc(&cipher, iv, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp , "wrong ciphertext");

}

static void test_crypto_modes_cbc_encrypt(void)
{
    test_encrypt_op(TEST_1_KEY, TEST_1_KEY_LEN, TEST_1_IV, TEST_1_PLAIN,
                    TEST_1_PLAIN_LEN, TEST_1_CIPHER, TEST_1_CIPHER_LEN);
}

static void test_crypto_modes_cbc_decrypt(void)
{
    test_decrypt_op(TEST_1_KEY, TEST_1_KEY_LEN, TEST_1_IV, TEST_1_CIPHER,
                    TEST_1_CIPHER_LEN, TEST_1_PLAIN, TEST_1_PLAIN_LEN);
}


Test* tests_crypto_modes_cbc_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_modes_cbc_encrypt),
                        new_TestFixture(test_crypto_modes_cbc_decrypt)
    };

    EMB_UNIT_TESTCALLER(crypto_modes_cbc_tests, NULL, NULL, fixtures);

    return (Test*)&crypto_modes_cbc_tests;
}
