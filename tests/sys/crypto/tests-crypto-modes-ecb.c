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
#include "crypto/modes/ecb.h"
#include "tests-crypto.h"

/*
 * all test vectors are from "Recommendation for Block Cipher Modes of
 * Operation - Methods and Techniques" by Morris Dworkin / NIST
 *
 *   http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf
 */

/* PACKET VECTOR #1 (Page 24) */
static uint8_t TEST_1_KEY[] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};
static uint8_t TEST_1_KEY_LEN = 16;

static uint8_t TEST_2_KEY[] = {
    0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
    0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
    0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
};
static uint8_t TEST_2_KEY_LEN = 24;

static uint8_t TEST_3_KEY[] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};
static uint8_t TEST_3_KEY_LEN = 32;

static uint8_t TEST_PLAIN[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
    0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
    0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};
static uint8_t TEST_PLAIN_LEN = 64;

static uint8_t TEST_1_CIPHER[] = {
    0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
    0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
    0xf5, 0xd3, 0xd5, 0x85, 0x03, 0xb9, 0x69, 0x9d,
    0xe7, 0x85, 0x89, 0x5a, 0x96, 0xfd, 0xba, 0xaf,
    0x43, 0xb1, 0xcd, 0x7f, 0x59, 0x8e, 0xce, 0x23,
    0x88, 0x1b, 0x00, 0xe3, 0xed, 0x03, 0x06, 0x88,
    0x7b, 0x0c, 0x78, 0x5e, 0x27, 0xe8, 0xad, 0x3f,
    0x82, 0x23, 0x20, 0x71, 0x04, 0x72, 0x5d, 0xd4
};

static uint8_t TEST_2_CIPHER[] = {
    0xbd, 0x33, 0x4f, 0x1d, 0x6e, 0x45, 0xf2, 0x5f,
    0xf7, 0x12, 0xa2, 0x14, 0x57, 0x1f, 0xa5, 0xcc,
    0x97, 0x41, 0x04, 0x84, 0x6d, 0x0a, 0xd3, 0xad,
    0x77, 0x34, 0xec, 0xb3, 0xec, 0xee, 0x4e, 0xef,
    0xef, 0x7a, 0xfd, 0x22, 0x70, 0xe2, 0xe6, 0x0a,
    0xdc, 0xe0, 0xba, 0x2f, 0xac, 0xe6, 0x44, 0x4e,
    0x9a, 0x4b, 0x41, 0xba, 0x73, 0x8d, 0x6c, 0x72,
    0xfb, 0x16, 0x69, 0x16, 0x03, 0xc1, 0x8e, 0x0e
};

static uint8_t TEST_3_CIPHER[] = {
    0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c,
    0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8,
    0x59, 0x1c, 0xcb, 0x10, 0xd4, 0x10, 0xed, 0x26,
    0xdc, 0x5b, 0xa7, 0x4a, 0x31, 0x36, 0x28, 0x70,
    0xb6, 0xed, 0x21, 0xb9, 0x9c, 0xa6, 0xf4, 0xf9,
    0xf1, 0x53, 0xe7, 0xb1, 0xbe, 0xaf, 0xed, 0x1d,
    0x23, 0x30, 0x4b, 0x7a, 0x39, 0xf9, 0xf3, 0xff,
    0x06, 0x7d, 0x8d, 0x8f, 0x9e, 0x24, 0xec, 0xc7
};
static uint8_t TEST_CIPHER_LEN = 64;

static void test_encrypt_op_128(uint8_t *key, uint8_t key_len, uint8_t *input,
                            uint8_t input_len, uint8_t *output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_encrypt_ecb(&cipher, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");

}

static void test_encrypt_op_192(uint8_t *key, uint8_t key_len, uint8_t *input,
                            uint8_t input_len, uint8_t *output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_encrypt_ecb(&cipher, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");

}

static void test_encrypt_op_256(uint8_t *key, uint8_t key_len, uint8_t *input,
                            uint8_t input_len, uint8_t *output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_encrypt_ecb(&cipher, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");

}

static void test_decrypt_op_128(uint8_t *key, uint8_t key_len, uint8_t *input,
                            uint8_t input_len, uint8_t *output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_decrypt_ecb(&cipher, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");

}

static void test_decrypt_op_192(uint8_t *key, uint8_t key_len, uint8_t *input,
                            uint8_t input_len, uint8_t *output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_decrypt_ecb(&cipher, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");

}

static void test_decrypt_op_256(uint8_t *key, uint8_t key_len, uint8_t *input,
                            uint8_t input_len, uint8_t *output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_decrypt_ecb(&cipher, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");

}

static void test_crypto_modes_ecb_encrypt(void)
{
    test_encrypt_op_128(TEST_1_KEY, TEST_1_KEY_LEN, TEST_PLAIN, TEST_PLAIN_LEN,
                    TEST_1_CIPHER, TEST_CIPHER_LEN);

    test_encrypt_op_192(TEST_2_KEY, TEST_2_KEY_LEN, TEST_PLAIN, TEST_PLAIN_LEN,
                    TEST_2_CIPHER, TEST_CIPHER_LEN);

    test_encrypt_op_256(TEST_3_KEY, TEST_3_KEY_LEN, TEST_PLAIN, TEST_PLAIN_LEN,
                    TEST_3_CIPHER, TEST_CIPHER_LEN);
}

static void test_crypto_modes_ecb_decrypt(void)
{
    test_decrypt_op_128(TEST_1_KEY, TEST_1_KEY_LEN, TEST_1_CIPHER,
                    TEST_CIPHER_LEN,
                    TEST_PLAIN, TEST_PLAIN_LEN);

    test_decrypt_op_192(TEST_2_KEY, TEST_2_KEY_LEN, TEST_2_CIPHER,
                    TEST_CIPHER_LEN,
                    TEST_PLAIN, TEST_PLAIN_LEN);

    test_decrypt_op_256(TEST_2_KEY, TEST_2_KEY_LEN, TEST_2_CIPHER,
                    TEST_CIPHER_LEN,
                    TEST_PLAIN, TEST_PLAIN_LEN);
}

Test *tests_crypto_modes_ecb_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_modes_ecb_encrypt),
        new_TestFixture(test_crypto_modes_ecb_decrypt)
    };

    EMB_UNIT_TESTCALLER(crypto_modes_ecb_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_modes_ecb_tests;
}
