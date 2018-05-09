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
#include "crypto/modes/ccm.h"
#include "tests-crypto.h"

static const size_t nonce_and_len_encoding_size = 15;

/* PACKET VECTOR #1 (RFC 3610 - Page 10) */
static uint8_t TEST_1_KEY[] = {
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
    0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF
};
static size_t TEST_1_KEY_LEN = 16;

static uint8_t TEST_1_NONCE[] = {
    0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00, 0xA0,
    0xA1, 0xA2, 0xA3, 0xA4, 0xA5
};
static size_t TEST_1_NONCE_LEN = 13;

static size_t TEST_1_MAC_LEN = 8;

static uint8_t TEST_1_INPUT[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* additional auth data */
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, /* input */
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* input */
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E        /* input */
};
static size_t TEST_1_INPUT_LEN = 23;
static size_t TEST_1_ADATA_LEN = 8;

static uint8_t TEST_1_EXPECTED[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x58, 0x8C, 0x97, 0x9A, 0x61, 0xC6, 0x63, 0xD2,
    0xF0, 0x66, 0xD0, 0xC2, 0xC0, 0xF9, 0x89, 0x80,
    0x6D, 0x5F, 0x6B, 0x61, 0xDA, 0xC3, 0x84, 0x17,
    0xE8, 0xD1, 0x2C, 0xFD, 0xF9, 0x26, 0xE0
};
static size_t TEST_1_EXPECTED_LEN = 39;

/* PACKET VECTOR #2 (RFC 3610 - Page 10) */
static uint8_t TEST_2_KEY[] = {
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
    0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF
};
static size_t TEST_2_KEY_LEN = 16;

static uint8_t TEST_2_NONCE[] = {
    0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01, 0xA0,
    0xA1, 0xA2, 0xA3, 0xA4, 0xA5
};
static size_t TEST_2_NONCE_LEN = 13;

static size_t TEST_2_MAC_LEN = 8;

static uint8_t TEST_2_INPUT[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};
static size_t TEST_2_INPUT_LEN = 24;
static size_t TEST_2_ADATA_LEN = 8;

static uint8_t TEST_2_EXPECTED[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x72, 0xC9, 0x1A, 0x36, 0xE1, 0x35, 0xF8, 0xCF,
    0x29, 0x1C, 0xA8, 0x94, 0x08, 0x5C, 0x87, 0xE3,
    0xCC, 0x15, 0xC4, 0x39, 0xC9, 0xE4, 0x3A, 0x3B,
    0xA0, 0x91, 0xD5, 0x6E, 0x10, 0x40, 0x09, 0x16
};
static size_t TEST_2_EXPECTED_LEN = 40;

/* Share test buffer output */
static uint8_t data[60];

static void test_encrypt_op(uint8_t* key, uint8_t key_len,
                            uint8_t* adata, size_t adata_len,
                            uint8_t* nonce, uint8_t nonce_len,
                            uint8_t* plain, size_t plain_len,
                            uint8_t* output_expected,
                            size_t output_expected_len,
                            uint8_t mac_length)
{
    cipher_t cipher;
    int len, err, cmp;
    size_t len_encoding = nonce_and_len_encoding_size - nonce_len;

    TEST_ASSERT_MESSAGE(sizeof(data) >= output_expected_len,
                        "Output buffer too small");

    err = cipher_init(&cipher, CIPHER_AES_128, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_encrypt_ccm(&cipher, adata, adata_len,
                             mac_length, len_encoding,
                             nonce, nonce_len, plain, plain_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_expected_len, len);
    cmp = compare(output_expected, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp , "wrong ciphertext");

}

static void test_decrypt_op(uint8_t* key, uint8_t key_len,
                            uint8_t* adata, size_t adata_len,
                            uint8_t* nonce, uint8_t nonce_len,
                            uint8_t* encrypted, size_t encrypted_len,
                            uint8_t* output_expected,
                            size_t output_expected_len,
                            uint8_t mac_length)
{
    cipher_t cipher;
    int len, err, cmp;
    size_t len_encoding = nonce_and_len_encoding_size - nonce_len;

    TEST_ASSERT_MESSAGE(sizeof(data) >= output_expected_len,
                        "Output buffer too small");

    err = cipher_init(&cipher, CIPHER_AES_128, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_decrypt_ccm(&cipher, adata, adata_len,
                             mac_length, len_encoding,
                             nonce, nonce_len, encrypted, encrypted_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Decryption failed");

    TEST_ASSERT_EQUAL_INT(output_expected_len, len);
    cmp = compare(output_expected, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp , "wrong ciphertext");

}

#define do_test_encrypt_op(name) do { \
    test_encrypt_op(TEST_##name##_KEY, TEST_##name##_KEY_LEN, \
                    TEST_##name##_INPUT, TEST_##name##_ADATA_LEN, \
                    TEST_##name##_NONCE, TEST_##name##_NONCE_LEN, \
                    \
                    TEST_##name##_INPUT + TEST_##name##_ADATA_LEN, \
                    TEST_##name##_INPUT_LEN, \
                    \
                    TEST_##name##_EXPECTED + TEST_##name##_ADATA_LEN, \
                    TEST_##name##_EXPECTED_LEN - TEST_##name##_ADATA_LEN, \
                    \
                    TEST_##name##_MAC_LEN \
                    ); \
} while (0)


static void test_crypto_modes_ccm_encrypt(void)
{
    do_test_encrypt_op(1);
    do_test_encrypt_op(2);
}

#define do_test_decrypt_op(name) do { \
    test_decrypt_op(TEST_##name##_KEY, TEST_##name##_KEY_LEN, \
                    TEST_##name##_INPUT, TEST_##name##_ADATA_LEN, \
                    TEST_##name##_NONCE, TEST_##name##_NONCE_LEN, \
                    \
                    TEST_##name##_EXPECTED + TEST_##name##_ADATA_LEN, \
                    TEST_##name##_EXPECTED_LEN - TEST_##name##_ADATA_LEN, \
                    \
                    TEST_##name##_INPUT + TEST_##name##_ADATA_LEN, \
                    TEST_##name##_INPUT_LEN, \
                    \
                    TEST_##name##_MAC_LEN \
                    ); \
} while (0)

static void test_crypto_modes_ccm_decrypt(void)
{
    do_test_decrypt_op(1);
    do_test_decrypt_op(2);
}


typedef int (*func_ccm_t)(cipher_t*, uint8_t*, uint32_t, uint8_t, uint8_t,
                          uint8_t*, size_t, uint8_t*, size_t, uint8_t*);

static int _test_ccm_len(func_ccm_t func, uint8_t len_encoding,
                         uint8_t *input, size_t input_len, size_t adata_len)
{
    int ret;
    cipher_t cipher;
    uint8_t mac_length = 8;
    uint8_t nonce[15] = {0};
    uint8_t key[16] = {0};

    uint8_t nonce_len = nonce_and_len_encoding_size - len_encoding;
    cipher_init(&cipher, CIPHER_AES_128, key, 16);

    ret = func(&cipher, NULL, adata_len, mac_length, len_encoding,
               nonce, nonce_len, input, input_len, data);
    return ret;
}


/* Test length checking in ccm functions. */
static void test_crypto_modes_ccm_check_len(void)
{
    int ret;
    /* Just 1 to big to fit */
    ret = _test_ccm_len(cipher_encrypt_ccm, 2, NULL, 1 << 16, 0);
    TEST_ASSERT_EQUAL_INT(CCM_ERR_INVALID_LENGTH_ENCODING, ret);
    ret = _test_ccm_len(cipher_decrypt_ccm, 2, NULL, 1 << 16, 0);
    TEST_ASSERT_EQUAL_INT(CCM_ERR_INVALID_LENGTH_ENCODING, ret);

    /* adata_len should not change the result (was wrong in previous implem) */
    ret = _test_ccm_len(cipher_encrypt_ccm, 2, NULL, 1 << 16, 65535);
    TEST_ASSERT_EQUAL_INT(CCM_ERR_INVALID_LENGTH_ENCODING, ret);
    ret = _test_ccm_len(cipher_decrypt_ccm, 2, NULL, 1 << 16, 65535);
    TEST_ASSERT_EQUAL_INT(CCM_ERR_INVALID_LENGTH_ENCODING, ret);

    /* Valid length that were wrongly checked */
    /* Check should work with len_encoding >= 4, test with 8 */
    uint8_t input[8];
    ret = _test_ccm_len(cipher_encrypt_ccm, 8, input, 8, 0);
    TEST_ASSERT_MESSAGE(ret > 0, "Encryption : failed with valid input_len");

    /* einput is encrypted value for
     * - 8 * 0 input
     * - All 0 nonce and key
     * - adata_len == 0
     * - mac_len == 8 and len_encoding = 8
     */
    uint8_t einput[16] = {
        0xa2, 0x46, 0x75, 0xfc, 0x5f, 0x1b, 0x01, 0x37,
        0x8a, 0x85, 0xd7, 0xf8, 0x42, 0x82, 0x6a, 0x63,
    };

    ret = _test_ccm_len(cipher_decrypt_ccm, 8, einput, 16, 0);
    TEST_ASSERT_MESSAGE(ret > 0, "Decryption : failed with valid input_len");
}


Test* tests_crypto_modes_ccm_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_modes_ccm_encrypt),
        new_TestFixture(test_crypto_modes_ccm_decrypt),
        new_TestFixture(test_crypto_modes_ccm_check_len),
    };

    EMB_UNIT_TESTCALLER(crypto_modes_ccm_tests, NULL, NULL, fixtures);

    return (Test*)&crypto_modes_ccm_tests;
}
