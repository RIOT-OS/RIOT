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
#include "crypto/modes/ccms.h"
#include "tests-crypto.h"

static const size_t nonce_and_len_encoding_size = 15;

/* IEEE 0537r2 */
static const uint8_t TEST_IEEE_0537R2_1_KEY[] = {
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
    0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF
};
static const uint8_t TEST_IEEE_0537R2_1_KEY_LEN = 16;
static const uint8_t TEST_IEEE_0537R2_1_NONCE[] = {
    0xAC, 0xDE, 0x48, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x05, 0x02
};
static const uint8_t TEST_IEEE_0537R2_1_NONCE_LEN = 13;
static const uint8_t TEST_IEEE_0537R2_1_MAC_LEN = 8;
static const uint8_t TEST_IEEE_0537R2_1_INPUT[] = {
    /* AAD */
    0x08, 0xD0, 0x84, 0x21, 0x43, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x48, 0xDE, 0xAC, 0x02, 0x05, 0x00,
    0x00, 0x00, 0x55, 0xCF, 0x00, 0x00, 0x51, 0x52,
    0x53, 0x54,
    /* PLAINTEXT */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const size_t TEST_IEEE_0537R2_1_INPUT_LEN = 0;
static const size_t TEST_IEEE_0537R2_1_ADATA_LEN = 26;
static const uint8_t TEST_IEEE_0537R2_1_EXPECTED[] = {
    /* AAD */
    0x08, 0xD0, 0x84, 0x21, 0x43, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x48, 0xDE, 0xAC, 0x02, 0x05, 0x00,
    0x00, 0x00, 0x55, 0xCF, 0x00, 0x00, 0x51, 0x52,
    0x53, 0x54,
    /* MAC */
    0x22, 0x3B, 0xC1, 0xEC, 0x84, 0x1A, 0xB5, 0x53,
};
static const size_t TEST_IEEE_0537R2_1_EXPECTED_LEN = 34;

/* IEEE 0537r2 */
static const uint8_t TEST_IEEE_0537R2_2_KEY[] = {
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
     0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF
};
static const uint8_t TEST_IEEE_0537R2_2_KEY_LEN = 16;
static const uint8_t TEST_IEEE_0537R2_2_NONCE[] = {
    0xAC, 0xDE, 0x48, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x05, 0x04
};
static const uint8_t TEST_IEEE_0537R2_2_NONCE_LEN = 13;
static const uint8_t TEST_IEEE_0537R2_2_MAC_LEN = 0;
static const uint8_t TEST_IEEE_0537R2_2_INPUT[] = {
    /* AAD */
    0x69, 0xDC, 0x84, 0x21, 0x43, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x48, 0xDE, 0xAC, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x48, 0xDE, 0xAC, 0x04, 0x05, 0x00,
    0x00, 0x00,
    /* PLAINTEXT */
    0x61, 0x62, 0x63, 0x64
};
static const size_t TEST_IEEE_0537R2_2_INPUT_LEN = 4;
static const size_t TEST_IEEE_0537R2_2_ADATA_LEN = 26;
static const uint8_t TEST_IEEE_0537R2_2_EXPECTED[] = {
    /* AAD */
    0x69, 0xDC, 0x84, 0x21, 0x43, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x48, 0xDE, 0xAC, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x48, 0xDE, 0xAC, 0x04, 0x05, 0x00,
    0x00, 0x00,
    /* CIPHERTEXT */
    0xD4, 0x3E, 0x02, 0x2B,
};
static const size_t TEST_IEEE_0537R2_2_EXPECTED_LEN = 30;

/* IEEE 0537r2 */
static const uint8_t TEST_IEEE_0537R2_3_KEY[] = {
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
     0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF
};
static const uint8_t TEST_IEEE_0537R2_3_KEY_LEN = 16;
static const uint8_t TEST_IEEE_0537R2_3_NONCE[] = {
    0xAC, 0xDE, 0x48, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x05, 0x06
};
static const uint8_t TEST_IEEE_0537R2_3_NONCE_LEN = 13;
static const uint8_t TEST_IEEE_0537R2_3_MAC_LEN = 8;
static const uint8_t TEST_IEEE_0537R2_3_INPUT[] = {
    /* AAD */
    0x2B, 0xDC, 0x84, 0x21, 0x43, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x48, 0xDE, 0xAC, 0xFF, 0xFF, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC, 0x06,
    0x05, 0x00, 0x00, 0x00, 0x01,
    /* PLAINTEXT */
    0xCE
};
static const size_t TEST_IEEE_0537R2_3_INPUT_LEN = 1;
static const size_t TEST_IEEE_0537R2_3_ADATA_LEN = 29;
static const uint8_t TEST_IEEE_0537R2_3_EXPECTED[] = {
    /* AAD */
    0x2B, 0xDC, 0x84, 0x21, 0x43, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x48, 0xDE, 0xAC, 0xFF, 0xFF, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC, 0x06,
    0x05, 0x00, 0x00, 0x00, 0x01,
    /* CIPHERTEXT */
    0xD8,
    /* MAC */
    0x4F, 0xDE, 0x52, 0x90, 0x61, 0xF9, 0xC6, 0xF1
};
static const size_t TEST_IEEE_0537R2_3_EXPECTED_LEN = 38;

/* Share test buffer output */
static uint8_t data[512];

static void test_encrypt_op(const uint8_t *key, uint8_t key_len,
                            const uint8_t *adata, size_t adata_len,
                            const uint8_t *nonce, uint8_t nonce_len,
                            const uint8_t *plain, size_t plain_len,
                            const uint8_t *output_expected,
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

    len = cipher_encrypt_ccms(&cipher, adata, adata_len,
                              mac_length, len_encoding,
                              nonce, nonce_len, plain, plain_len, data);
    TEST_ASSERT_MESSAGE(len >= 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_expected_len, len);
    cmp = compare(output_expected, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");
}

static void test_decrypt_op(const uint8_t *key, uint8_t key_len,
                            const uint8_t *adata, size_t adata_len,
                            const uint8_t *nonce, uint8_t nonce_len,
                            const uint8_t *encrypted, size_t encrypted_len,
                            const uint8_t *output_expected,
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

    len = cipher_decrypt_ccms(&cipher, adata, adata_len,
                              mac_length, len_encoding,
                              nonce, nonce_len, encrypted, encrypted_len, data);
    TEST_ASSERT_MESSAGE(len >= 0, "Decryption failed");

    TEST_ASSERT_EQUAL_INT(output_expected_len, len);
    cmp = compare(output_expected, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");

}

#define do_test_encrypt_op(name) do { \
        test_encrypt_op(TEST_ ## name ## _KEY, TEST_ ## name ## _KEY_LEN, \
                        TEST_ ## name ## _INPUT, TEST_ ## name ## _ADATA_LEN, \
                        TEST_ ## name ## _NONCE, TEST_ ## name ## _NONCE_LEN, \
                    \
                        TEST_ ## name ## _INPUT + TEST_ ## name ## _ADATA_LEN, \
                        TEST_ ## name ## _INPUT_LEN, \
                    \
                        TEST_ ## name ## _EXPECTED + TEST_ ## name ## _ADATA_LEN, \
                        TEST_ ## name ## _EXPECTED_LEN - TEST_ ## name ## _ADATA_LEN, \
                    \
                        TEST_ ## name ## _MAC_LEN \
                        ); \
} while (0)


static void test_crypto_modes_ccms_encrypt(void)
{
    do_test_encrypt_op(IEEE_0537R2_1);
    do_test_encrypt_op(IEEE_0537R2_2);
    do_test_encrypt_op(IEEE_0537R2_3);
}

#define do_test_decrypt_op(name) do { \
        test_decrypt_op(TEST_ ## name ## _KEY, TEST_ ## name ## _KEY_LEN, \
                        TEST_ ## name ## _INPUT, TEST_ ## name ## _ADATA_LEN, \
                        TEST_ ## name ## _NONCE, TEST_ ## name ## _NONCE_LEN, \
                    \
                        TEST_ ## name ## _EXPECTED + TEST_ ## name ## _ADATA_LEN, \
                        TEST_ ## name ## _EXPECTED_LEN - TEST_ ## name ## _ADATA_LEN, \
                    \
                        TEST_ ## name ## _INPUT + TEST_ ## name ## _ADATA_LEN, \
                        TEST_ ## name ## _INPUT_LEN, \
                    \
                        TEST_ ## name ## _MAC_LEN \
                        ); \
} while (0)

static void test_crypto_modes_ccms_decrypt(void)
{
    do_test_decrypt_op(IEEE_0537R2_1);
    do_test_decrypt_op(IEEE_0537R2_2);
    do_test_decrypt_op(IEEE_0537R2_3);
}


typedef int (*func_ccms_t)(cipher_t *, const uint8_t *, uint32_t,
                          uint8_t, uint8_t, const uint8_t *, size_t,
                          const uint8_t *, size_t, uint8_t *);

static int _test_ccms_len(func_ccms_t func, uint8_t len_encoding,
                         const uint8_t *input, size_t input_len,
                         size_t adata_len)
{
    int ret;
    cipher_t cipher;
    uint8_t mac_length = 8;
    uint8_t nonce[15] = { 0 };
    uint8_t key[16] = { 0 };

    uint8_t nonce_len = nonce_and_len_encoding_size - len_encoding;

    cipher_init(&cipher, CIPHER_AES_128, key, 16);

    ret = func(&cipher, NULL, adata_len, mac_length, len_encoding,
               nonce, nonce_len, input, input_len, data);
    return ret;
}


/* Test length checking in ccm functions. */
static void test_crypto_modes_ccms_check_len(void)
{
    int ret;

/* Using length_encoding above UINT16_MAX doesn't work on 8/16 bit
   architectures, SIZE_MAX is equal to UINT16_MAX there */
#if SIZE_MAX > UINT16_MAX
    /* Just 1 to big to fit */
    ret = _test_ccms_len(cipher_encrypt_ccms, 2, NULL, 1 << 16, 0);
    TEST_ASSERT_EQUAL_INT(CCM_ERR_INVALID_LENGTH_ENCODING, ret);
    ret = _test_ccms_len(cipher_decrypt_ccms, 2, NULL, 1 << 16, 0);
    TEST_ASSERT_EQUAL_INT(CCM_ERR_INVALID_LENGTH_ENCODING, ret);

    /* adata_len should not change the result (was wrong in previous implem) */
    ret = _test_ccms_len(cipher_encrypt_ccms, 2, NULL, 1 << 16, 65535);
    TEST_ASSERT_EQUAL_INT(CCM_ERR_INVALID_LENGTH_ENCODING, ret);
    ret = _test_ccms_len(cipher_decrypt_ccms, 2, NULL, 1 << 16, 65535);
    TEST_ASSERT_EQUAL_INT(CCM_ERR_INVALID_LENGTH_ENCODING, ret);
#endif

    /* Invalid length when length_encoding < 2 */
    ret = _test_ccms_len(cipher_encrypt_ccms, 1, NULL, 8, 0);
    TEST_ASSERT_EQUAL_INT(CCM_ERR_INVALID_LENGTH_ENCODING, ret);

    /* Valid length that were wrongly checked */
    /* Check should work with len_encoding >= 4, test with 8 */
    uint8_t input[8];
    ret = _test_ccms_len(cipher_encrypt_ccms, 8, input, 8, 0);
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

    ret = _test_ccms_len(cipher_decrypt_ccms, 8, einput, 16, 0);
    TEST_ASSERT_MESSAGE(ret > 0, "Decryption : failed with valid input_len");

    /* ccm library does not support auth_data_len > 0xFEFF */
    ret = _test_ccms_len(cipher_encrypt_ccms, 2, NULL, 0, 0xFEFF + 1);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}


Test *tests_crypto_modes_ccms_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_modes_ccms_encrypt),
        new_TestFixture(test_crypto_modes_ccms_decrypt),
        new_TestFixture(test_crypto_modes_ccms_check_len),
    };

    EMB_UNIT_TESTCALLER(crypto_modes_ccms_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_modes_ccms_tests;
}
