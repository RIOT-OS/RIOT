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
#include "crypto/modes/ctr.h"
#include "tests-crypto.h"

/*
 * all test vectors are from "Recommendation for Block Cipher Modes of
 * Operation - Methods and Techniques" by Morris Dworkin / NIST
 *
 *   http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf
 */

/* PACKET VECTOR #1 (Page 55) */
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

static uint8_t TEST_COUNTER[16] = {
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

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
    0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26,
    0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce,
    0x98, 0x06, 0xf6, 0x6b, 0x79, 0x70, 0xfd, 0xff,
    0x86, 0x17, 0x18, 0x7b, 0xb9, 0xff, 0xfd, 0xff,
    0x5a, 0xe4, 0xdf, 0x3e, 0xdb, 0xd5, 0xd3, 0x5e,
    0x5b, 0x4f, 0x09, 0x02, 0x0d, 0xb0, 0x3e, 0xab,
    0x1e, 0x03, 0x1d, 0xda, 0x2f, 0xbe, 0x03, 0xd1,
    0x79, 0x21, 0x70, 0xa0, 0xf3, 0x00, 0x9c, 0xee
};

static uint8_t TEST_2_CIPHER[] = {
    0x1a, 0xbc, 0x93, 0x24, 0x17, 0x52, 0x1c, 0xa2,
    0x4f, 0x2b, 0x04, 0x59, 0xfe, 0x7e, 0x6e, 0x0b,
    0x09, 0x03, 0x39, 0xec, 0x0a, 0xa6, 0xfa, 0xef,
    0xd5, 0xcc, 0xc2, 0xc6, 0xf4, 0xce, 0x8e, 0x94,
    0x1e, 0x36, 0xb2, 0x6b, 0xd1, 0xeb, 0xc6, 0x70,
    0xd1, 0xbd, 0x1d, 0x66, 0x56, 0x20, 0xab, 0xf7,
    0x4f, 0x78, 0xa7, 0xf6, 0xd2, 0x98, 0x09, 0x58,
    0x5a, 0x97, 0xda, 0xec, 0x58, 0xc6, 0xb0, 0x50
};

static uint8_t TEST_3_CIPHER[] = {
    0x60, 0x1e, 0xc3, 0x13, 0x77, 0x57, 0x89, 0xa5,
    0xb7, 0xa7, 0xf5, 0x04, 0xbb, 0xf3, 0xd2, 0x28,
    0xf4, 0x43, 0xe3, 0xca, 0x4d, 0x62, 0xb5, 0x9a,
    0xca, 0x84, 0xe9, 0x90, 0xca, 0xca, 0xf5, 0xc5,
    0x2b, 0x09, 0x30, 0xda, 0xa2, 0x3d, 0xe9, 0x4c,
    0xe8, 0x70, 0x17, 0xba, 0x2d, 0x84, 0x98, 0x8d,
    0xdf, 0xc9, 0xc5, 0x8d, 0xb6, 0x7a, 0xad, 0xa6,
    0x13, 0xc2, 0xdd, 0x08, 0x45, 0x79, 0x41, 0xa6
};
static uint8_t TEST_CIPHER_LEN = 64;

static void test_encrypt_op_128(uint8_t *key, uint8_t key_len, uint8_t ctr[16],
                            uint8_t *input, uint8_t input_len, uint8_t *output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_encrypt_ctr(&cipher, ctr, 0, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");

}

static void test_encrypt_op_192(uint8_t *key, uint8_t key_len, uint8_t ctr[16],
                            uint8_t *input, uint8_t input_len, uint8_t *output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_encrypt_ctr(&cipher, ctr, 0, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");
}

static void test_encrypt_op_256(uint8_t *key, uint8_t key_len, uint8_t ctr[16],
                            uint8_t *input, uint8_t input_len, uint8_t *output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_encrypt_ctr(&cipher, ctr, 0, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");
}

static void test_decrypt_op_128(uint8_t *key, uint8_t key_len, uint8_t ctr[16],
                            uint8_t *input, uint8_t input_len, uint8_t *output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_decrypt_ctr(&cipher, ctr, 0, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");
}

static void test_decrypt_op_192(uint8_t *key, uint8_t key_len, uint8_t ctr[16],
                            uint8_t *input, uint8_t input_len, uint8_t *output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_decrypt_ctr(&cipher, ctr, 0, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");
}

static void test_decrypt_op_256(uint8_t *key, uint8_t key_len, uint8_t ctr[16],
                            uint8_t *input, uint8_t input_len, uint8_t *output,
                            uint8_t output_len)
{
    cipher_t cipher;
    int len, err, cmp;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_decrypt_ctr(&cipher, ctr, 0, input, input_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_len, len);
    cmp = compare(output, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");
}

static void test_crypto_modes_ctr_encrypt(void)
{
    uint8_t ctr[16];

    memcpy(ctr, TEST_COUNTER, 16);
    test_encrypt_op_128(TEST_1_KEY, TEST_1_KEY_LEN, ctr, TEST_PLAIN,
                    TEST_PLAIN_LEN, TEST_1_CIPHER, TEST_CIPHER_LEN);

    memcpy(ctr, TEST_COUNTER, 16);
    test_encrypt_op_192(TEST_2_KEY, TEST_2_KEY_LEN, ctr, TEST_PLAIN,
                    TEST_PLAIN_LEN, TEST_2_CIPHER, TEST_CIPHER_LEN);

    memcpy(ctr, TEST_COUNTER, 16);
    test_encrypt_op_256(TEST_3_KEY, TEST_3_KEY_LEN, ctr, TEST_PLAIN,
                    TEST_PLAIN_LEN, TEST_3_CIPHER, TEST_CIPHER_LEN);
}

static void test_crypto_modes_ctr_decrypt(void)
{
    uint8_t ctr[16];

    memcpy(ctr, TEST_COUNTER, 16);
    test_decrypt_op_128(TEST_1_KEY, TEST_1_KEY_LEN, ctr, TEST_1_CIPHER,
                    TEST_CIPHER_LEN, TEST_PLAIN, TEST_PLAIN_LEN);

    memcpy(ctr, TEST_COUNTER, 16);
    test_decrypt_op_192(TEST_2_KEY, TEST_2_KEY_LEN, ctr, TEST_2_CIPHER,
                    TEST_CIPHER_LEN, TEST_PLAIN, TEST_PLAIN_LEN);

    memcpy(ctr, TEST_COUNTER, 16);
    test_decrypt_op_256(TEST_3_KEY, TEST_3_KEY_LEN, ctr, TEST_3_CIPHER,
                    TEST_CIPHER_LEN, TEST_PLAIN, TEST_PLAIN_LEN);
}

Test *tests_crypto_modes_ctr_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_modes_ctr_encrypt),
        new_TestFixture(test_crypto_modes_ctr_decrypt)
    };

    EMB_UNIT_TESTCALLER(crypto_modes_ctr_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_modes_ctr_tests;
}
