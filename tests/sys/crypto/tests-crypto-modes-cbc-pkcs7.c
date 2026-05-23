/*
 * Copyright (C) 2022 Lena Boeckmann / HAW Hamburg
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
 * Keys and input test vectors are from "Recommendation for Block Cipher Modes of
 * Operation - Methods and Techniques" by Morris Dworkin / NIST
 *
 *   http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf
 *
 * They were slightly modified to test the PKCS7 padding implementation.
 */

/* PACKET VECTOR #1 (Page 27) */
static uint8_t TEST_KEY_128[] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};
static uint8_t TEST_KEY_128_LEN = 16;

static uint8_t TEST_KEY_192[] = {
    0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
    0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
    0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
};
static uint8_t TEST_KEY_192_LEN = 24;

static uint8_t TEST_KEY_256[] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};
static uint8_t TEST_KEY_256_LEN = 32;

static uint8_t TEST_IV[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

/* Test vector with the last 5 bytes removed */
static uint8_t TEST_1_PLAIN[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
    0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
    0xad, 0x2b, 0x41
};
static uint8_t TEST_1_PLAIN_LEN = 59;

/* Test vector that is a multiple of the block size */
static uint8_t TEST_2_PLAIN[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
    0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef
};
static uint8_t TEST_2_PLAIN_LEN = 48;

/* Cipher of TEST_1_PLAIN, padded with 5 x 0x05 and encrypted with 128 AES key */
static uint8_t TEST_1_128_CIPHER[] = {
    0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
    0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
    0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee,
    0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
    0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b,
    0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
    0x92, 0xb2, 0x67, 0x09, 0x6b, 0x3a, 0x0a, 0x05,
    0xa3, 0x22, 0xa7, 0xf5, 0xdc, 0x6e, 0xa5, 0x48
};

/* Cipher of TEST_1_PLAIN, padded with 5 x 0x05 and encrypted with 192 AES key */
static uint8_t TEST_1_192_CIPHER[] = {
    0x4f, 0x02, 0x1d, 0xb2, 0x43, 0xbc, 0x63, 0x3d,
    0x71, 0x78, 0x18, 0x3a, 0x9f, 0xa0, 0x71, 0xe8,
    0xb4, 0xd9, 0xad, 0xa9, 0xad, 0x7d, 0xed, 0xf4,
    0xe5, 0xe7, 0x38, 0x76, 0x3f, 0x69, 0x14, 0x5a,
    0x57, 0x1b, 0x24, 0x20, 0x12, 0xfb, 0x7a, 0xe0,
    0x7f, 0xa9, 0xba, 0xac, 0x3d, 0xf1, 0x02, 0xe0,
    0xa0, 0x1c, 0x96, 0x60, 0x59, 0x2f, 0x9d, 0x79,
    0x52, 0x40, 0xd5, 0x51, 0x76, 0xc5, 0xe0, 0x0b
};

/* Cipher of TEST_1_PLAIN, padded with 5 x 0x05 and encrypted with 256 AES key */
static uint8_t TEST_1_256_CIPHER[] = {
    0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba,
    0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6,
    0x9c, 0xfc, 0x4e, 0x96, 0x7e, 0xdb, 0x80, 0x8d,
    0x67, 0x9f, 0x77, 0x7b, 0xc6, 0x70, 0x2c, 0x7d,
    0x39, 0xf2, 0x33, 0x69, 0xa9, 0xd9, 0xba, 0xcf,
    0xa5, 0x30, 0xe2, 0x63, 0x04, 0x23, 0x14, 0x61,
    0xf0, 0x20, 0x7c, 0xf0, 0xe3, 0x02, 0x85, 0x65,
    0x15, 0x89, 0xd8, 0x01, 0x8e, 0xb0, 0x75, 0xc8
};

/* Cipher of TEST_2_PLAIN, padded with 16 x 0x10 and encrypted with 128 AES key */
static uint8_t TEST_2_128_CIPHER[] = {
    0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
    0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
    0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee,
    0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
    0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b,
    0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
    0xf6, 0xec, 0xcd, 0xa3, 0x27, 0xbf, 0x8e, 0x5e,
    0xc4, 0x37, 0x18, 0xb0, 0x03, 0x9a, 0xdc, 0xeb
};

/* Cipher of TEST_2_PLAIN, padded with 16 x 0x10 and encrypted with 192 AES key */
static uint8_t TEST_2_192_CIPHER[] = {
    0x4f, 0x02, 0x1d, 0xb2, 0x43, 0xbc, 0x63, 0x3d,
    0x71, 0x78, 0x18, 0x3a, 0x9f, 0xa0, 0x71, 0xe8,
    0xb4, 0xd9, 0xad, 0xa9, 0xad, 0x7d, 0xed, 0xf4,
    0xe5, 0xe7, 0x38, 0x76, 0x3f, 0x69, 0x14, 0x5a,
    0x57, 0x1b, 0x24, 0x20, 0x12, 0xfb, 0x7a, 0xe0,
    0x7f, 0xa9, 0xba, 0xac, 0x3d, 0xf1, 0x02, 0xe0,
    0xc5, 0x4f, 0xcb, 0xc6, 0xdb, 0x74, 0x24, 0xcb,
    0x26, 0x8f, 0x58, 0x8f, 0x83, 0x29, 0x20, 0x23
};

/* Cipher of TEST_2_PLAIN, padded with 16 x 0x10 and encrypted with 256 AES key */
static uint8_t TEST_2_256_CIPHER[] = {
    0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba,
    0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6,
    0x9c, 0xfc, 0x4e, 0x96, 0x7e, 0xdb, 0x80, 0x8d,
    0x67, 0x9f, 0x77, 0x7b, 0xc6, 0x70, 0x2c, 0x7d,
    0x39, 0xf2, 0x33, 0x69, 0xa9, 0xd9, 0xba, 0xcf,
    0xa5, 0x30, 0xe2, 0x63, 0x04, 0x23, 0x14, 0x61,
    0x2f, 0x8d, 0xa7, 0x07, 0x64, 0x3c, 0x90, 0xa6,
    0xf7, 0x32, 0xb3, 0xde, 0x1d, 0x3f, 0x5c, 0xee
};

/* Cipher of TEST_1_PLAIN, with wrong padding and encrypted with 128 AES key */
static uint8_t TEST_3_CIPHER_WRONG_PADDING[] = {
    0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
    0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
    0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee,
    0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
    0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b,
    0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
    0x8e, 0x90, 0xc5, 0x9e, 0xed, 0xeb, 0x87, 0x9e,
    0x53, 0xf7, 0xea, 0x33, 0x04, 0xb0, 0xb1, 0xf8
};

static uint8_t TEST_CIPHER_LEN = 64;

static const char * err_to_str(int err)
{
    switch(err) {
        case CIPHER_ERR_BAD_CONTEXT_SIZE:
            return "CIPHER_ERR_BAD_CONTEXT_SIZE";
        case CIPHER_ERR_INVALID_KEY_SIZE:
            return "CIPHER_ERR_INVALID_KEY_SIZE";
        case CIPHER_ERR_INVALID_LENGTH:
            return "CIPHER_ERR_INVALID_LENGTH";
        case CIPHER_ERR_ENC_FAILED:
            return "CIPHER_ERR_ENC_FAILED";
        case CIPHER_ERR_DEC_FAILED:
            return "CIPHER_ERR_DEC_FAILED";
        case CIPHER_ERR_PADDING_ERROR:
            return "CIPHER_ERR_PADDING_ERROR";
        case CIPHER_ERR_BUFFER_TOO_SMALL:
            return "CIPHER_ERR_BUFFER_TOO_SMALL";
        default:
            return "Unknown error value";
    }
}

/*
 * Encrypts an input with PKCS7 padding and checks for correctness of output data.
 */
static void test_encrypt(char * test_name, uint8_t *key, uint8_t key_len, uint8_t iv[16],
                         uint8_t *input, uint8_t input_len, uint8_t *output,
                         uint8_t output_len)
{
    cipher_t cipher;
    int len, err;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    if (err != 1) {
        printf("%s: Cipher Init: Expected Success, was %s\n", test_name, err_to_str(err));
        return;
    }

    len = cipher_encrypt_cbc_pkcs7(&cipher, iv, input, input_len, data, sizeof(data));
    if (len <= 0 ) {
        printf("%s: Encryption: Expected %d, was %s\n", test_name, output_len, err_to_str(err));
        return;
    }
    if (len != output_len) {
        printf("%s: Encryption Output Len: Expected %d, was %d\n", test_name, output_len, len);
        return;
    }

    if (compare(output, data, len) != 1) {
        printf("%s: Wrong Ciphertext\n", test_name);
        return;
    }
}

static void test_decrypt(char * test_name, uint8_t *key, uint8_t key_len, uint8_t iv[16],
                         uint8_t *input, uint8_t input_len, uint8_t *output,
                         uint8_t output_len)
{
    cipher_t cipher;
    int len, err;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    if (err != 1) {
        printf("%s: Cipher Init: Expected Success, was %s\n", test_name, err_to_str(err));
        return;
    }

    len = cipher_decrypt_cbc_pkcs7(&cipher, iv, input, input_len, data, sizeof(data));
    if (len <= 0 ) {
        printf("%s: Decryption: Expected %d, was %s\n", test_name, output_len, err_to_str(err));
        return;
    }
    if (len != output_len) {
        printf("%s: Decryption Output Len: Expected %d, was %d\n", test_name, output_len, len);
        return;
    }

    if (compare(output, data, len) != 1) {
        printf("%s: Wrong Ciphertext\n", test_name);
        return;
    }
}

/*
 * Negative test:
 * Decrypts a cipher text with one wrong byte of padding
 * and expects the decryption function to fail.
 */
static void test_decrypt_negative(char * test_name, uint8_t *key, uint8_t key_len, uint8_t iv[16],
                         uint8_t *input, uint8_t input_len)
{
    cipher_t cipher;
    int err;
    uint8_t data[64];

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    if (err != 1) {
        printf("%s: Cipher Init: Expected Success, was %s\n", test_name, err_to_str(err));
        return;
    }

    err = cipher_decrypt_cbc_pkcs7(&cipher, iv, input, input_len, data, sizeof(data));
    if (err != CIPHER_ERR_PADDING_ERROR) {
        printf("%s: Decryption: Expected CIPHER_ERR_PADDING_ERROR, was %s\n",
                                                            test_name, err_to_str(err));
        return;
    }
}

static void test_crypto_modes_cbc_pkcs7_encrypt(void)
{
    test_encrypt("Test 1: Encryption with PKCS7: 5 bytes: AES 128\0",
                    TEST_KEY_128, TEST_KEY_128_LEN, TEST_IV, TEST_1_PLAIN,
                    TEST_1_PLAIN_LEN, TEST_1_128_CIPHER, TEST_CIPHER_LEN);

    test_encrypt("Test 2: Encryption with PKCS7: 5 bytes: AES 192\0",
                    TEST_KEY_192, TEST_KEY_192_LEN, TEST_IV, TEST_1_PLAIN,
                    TEST_1_PLAIN_LEN, TEST_1_192_CIPHER, TEST_CIPHER_LEN);

    test_encrypt("Test 3: Encryption with PKCS7: 5 bytes: AES 256\0",
                    TEST_KEY_256, TEST_KEY_256_LEN, TEST_IV, TEST_1_PLAIN,
                    TEST_1_PLAIN_LEN, TEST_1_256_CIPHER, TEST_CIPHER_LEN);

    test_encrypt("Test 4: Encryption with PKCS7: 16 bytes: AES 128\0",
                    TEST_KEY_128, TEST_KEY_128_LEN, TEST_IV, TEST_2_PLAIN,
                    TEST_2_PLAIN_LEN, TEST_2_128_CIPHER, TEST_CIPHER_LEN);

    test_encrypt("Test 5: Encryption with PKCS7: 16 bytes: AES 192\0",
                    TEST_KEY_192, TEST_KEY_192_LEN, TEST_IV, TEST_2_PLAIN,
                    TEST_2_PLAIN_LEN, TEST_2_192_CIPHER, TEST_CIPHER_LEN);

    test_encrypt("Test 6: Encryption with PKCS7: 16 bytes: AES 256\0",
                    TEST_KEY_256, TEST_KEY_256_LEN, TEST_IV, TEST_2_PLAIN,
                    TEST_2_PLAIN_LEN, TEST_2_256_CIPHER, TEST_CIPHER_LEN);
}

static void test_crypto_modes_cbc_pkcs7_decrypt(void)
{
    test_decrypt("Test 7: Decryption with PKCS7: 5 bytes: AES 128\0",
                    TEST_KEY_128, TEST_KEY_128_LEN, TEST_IV, TEST_1_128_CIPHER,
                    TEST_CIPHER_LEN, TEST_1_PLAIN, TEST_1_PLAIN_LEN);

    test_decrypt("Test 8: Decryption with PKCS7: 5 bytes: AES 192\0",
                    TEST_KEY_192, TEST_KEY_192_LEN, TEST_IV, TEST_1_192_CIPHER,
                    TEST_CIPHER_LEN, TEST_1_PLAIN, TEST_1_PLAIN_LEN);

    test_decrypt("Test 9: Decryption with PKCS7: 5 bytes: AES 256\0",
                    TEST_KEY_256, TEST_KEY_256_LEN, TEST_IV, TEST_1_256_CIPHER,
                    TEST_CIPHER_LEN, TEST_1_PLAIN, TEST_1_PLAIN_LEN);

    test_decrypt("Test 10: Decryption with PKCS7: 16 bytes: AES 128\0",
                    TEST_KEY_128, TEST_KEY_128_LEN, TEST_IV, TEST_2_128_CIPHER,
                    TEST_CIPHER_LEN, TEST_2_PLAIN, TEST_2_PLAIN_LEN);

    test_decrypt("Test 11: Decryption with PKCS7: 16 bytes: AES 192\0",
                    TEST_KEY_192, TEST_KEY_192_LEN, TEST_IV, TEST_2_192_CIPHER,
                    TEST_CIPHER_LEN, TEST_2_PLAIN, TEST_2_PLAIN_LEN);

    test_decrypt("Test 12: Decryption with PKCS7: 16 bytes: AES 256\0",
                    TEST_KEY_256, TEST_KEY_256_LEN, TEST_IV, TEST_2_256_CIPHER,
                    TEST_CIPHER_LEN, TEST_2_PLAIN, TEST_2_PLAIN_LEN);

    test_decrypt_negative("Test 13: Decryption with PKCS7: Wrong Padding: AES 128\0",
                    TEST_KEY_128, TEST_KEY_128_LEN, TEST_IV, TEST_3_CIPHER_WRONG_PADDING,
                    TEST_CIPHER_LEN);
}


Test *tests_crypto_modes_cbc_pkcs7_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_modes_cbc_pkcs7_encrypt),
        new_TestFixture(test_crypto_modes_cbc_pkcs7_decrypt)
    };

    EMB_UNIT_TESTCALLER(crypto_modes_cbc_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_modes_cbc_tests;
}
