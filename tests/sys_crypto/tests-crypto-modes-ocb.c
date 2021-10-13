/*
 * Copyright (C) 2018 Mathias Tausig
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "crypto/ciphers.h"
#include "crypto/modes/ocb.h"
#include "tests-crypto.h"

/* Test vectors from RFC 7253, Appendix A */
/* The key (K) has a fixed value, the tag length is
   128 bits, and the nonce (N) increments.

     K : 000102030405060708090A0B0C0D0E0F
 */
static uint8_t TEST_KEY[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};
static uint8_t TEST_KEY_LEN = 16;

/* Test 1:
    N: BBAA99887766554433221100
    A:
    P:
    C: 785407BFFFC8AD9EDCC5520AC9111EE6
 */
static uint8_t *TEST_1_KEY = TEST_KEY;

static uint8_t TEST_1_NONCE[] = {
    0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
    0x33, 0x22, 0x11, 0x00,
};
static size_t TEST_1_NONCE_LEN = 12;

static uint8_t *TEST_1_INPUT;
static size_t TEST_1_INPUT_LEN = 0;

static uint8_t *TEST_1_ADATA;
static size_t TEST_1_ADATA_LEN = 0;

static uint8_t TEST_1_EXPECTED[] = {
    0x78, 0x54, 0x07, 0xBF, 0xFF, 0xC8, 0xAD, 0x9E,
    0xDC, 0xC5, 0x52, 0x0A, 0xC9, 0x11, 0x1E, 0xE6
};
static size_t TEST_1_EXPECTED_LEN = 16;

static uint8_t TEST_1_TAG_LEN = 16;

/* Test 2:
    N: BBAA99887766554433221101
    A: 0001020304050607
    P: 0001020304050607
    C: 6820B3657B6F615A5725BDA0D3B4EB3A257C9AF1F8F03009
 */

static uint8_t *TEST_2_KEY = TEST_KEY;

static uint8_t TEST_2_NONCE[] = {
    0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
    0x33, 0x22, 0x11, 0x01,
};
static size_t TEST_2_NONCE_LEN = 12;

static uint8_t TEST_2_INPUT[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};
static size_t TEST_2_INPUT_LEN = sizeof(TEST_2_INPUT);

static uint8_t TEST_2_ADATA[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};
static size_t TEST_2_ADATA_LEN = sizeof(TEST_2_ADATA);

static uint8_t TEST_2_EXPECTED[] = {
    0x68, 0x20, 0xB3, 0x65, 0x7B, 0x6F, 0x61, 0x5A,
    0x57, 0x25, 0xBD, 0xA0, 0xD3, 0xB4, 0xEB, 0x3A,
    0x25, 0x7C, 0x9A, 0xF1, 0xF8, 0xF0, 0x30, 0x09
};
static size_t TEST_2_EXPECTED_LEN = sizeof(TEST_2_EXPECTED);

static uint8_t TEST_2_TAG_LEN = 16;

/* Test 3:
    N: BBAA99887766554433221102
    A: 0001020304050607
    P:
    C: 81017F8203F081277152FADE694A0A00
 */

static uint8_t *TEST_3_KEY = TEST_KEY;

static uint8_t TEST_3_NONCE[] = {
    0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
    0x33, 0x22, 0x11, 0x02,
};
static size_t TEST_3_NONCE_LEN = 12;

static uint8_t *TEST_3_INPUT;

static size_t TEST_3_INPUT_LEN = 0;

static uint8_t TEST_3_ADATA[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};
static size_t TEST_3_ADATA_LEN = sizeof(TEST_2_ADATA);

static uint8_t TEST_3_EXPECTED[] = {
    0x81, 0x01, 0x7F, 0x82, 0x03, 0xF0, 0x81, 0x27,
    0x71, 0x52, 0xFA, 0xDE, 0x69, 0x4A, 0x0A, 0x00
};
static size_t TEST_3_EXPECTED_LEN = sizeof(TEST_3_EXPECTED);

static uint8_t TEST_3_TAG_LEN = 16;

/* Test 4:
    N: BBAA99887766554433221103
    A:
    P: 0001020304050607
    C: 45DD69F8F5AAE72414054CD1F35D82760B2CD00D2F99BFA9
 */

static uint8_t *TEST_4_KEY = TEST_KEY;

static uint8_t TEST_4_NONCE[] = {
    0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
    0x33, 0x22, 0x11, 0x03,
};
static size_t TEST_4_NONCE_LEN = 12;

static uint8_t TEST_4_INPUT[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};
static size_t TEST_4_INPUT_LEN = sizeof(TEST_4_INPUT);

static uint8_t *TEST_4_ADATA;

static size_t TEST_4_ADATA_LEN = 0;

static uint8_t TEST_4_EXPECTED[] = {
    0x45, 0xDD, 0x69, 0xF8, 0xF5, 0xAA, 0xE7, 0x24,
    0x14, 0x05, 0x4C, 0xD1, 0xF3, 0x5D, 0x82, 0x76,
    0x0B, 0x2C, 0xD0, 0x0D, 0x2F, 0x99, 0xBF, 0xA9
};
static size_t TEST_4_EXPECTED_LEN = sizeof(TEST_4_EXPECTED);

static uint8_t TEST_4_TAG_LEN = 16;

/* Test 16:
    N: BBAA9988776655443322110F
    A:
    P: 000102030405060708090A0B0C0D0E0F1011121314151617
       18191A1B1C1D1E1F2021222324252627
    C: 4412923493C57D5DE0D700F753CCE0D1D2D95060122E9F15
       A5DDBFC5787E50B5CC55EE507BCB084E479AD363AC366B95
       A98CA5F3000B1479
 */

static uint8_t *TEST_16_KEY = TEST_KEY;

static uint8_t TEST_16_NONCE[] = {
    0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
    0x33, 0x22, 0x11, 0x0F,
};
static size_t TEST_16_NONCE_LEN = 12;

static uint8_t TEST_16_INPUT[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
};
static size_t TEST_16_INPUT_LEN = sizeof(TEST_16_INPUT);

static uint8_t *TEST_16_ADATA;
static size_t TEST_16_ADATA_LEN = 0;

static uint8_t TEST_16_EXPECTED[] = {
    0x44, 0x12, 0x92, 0x34, 0x93, 0xC5, 0x7D, 0x5D,
    0xE0, 0xD7, 0x00, 0xF7, 0x53, 0xCC, 0xE0, 0xD1,
    0xD2, 0xD9, 0x50, 0x60, 0x12, 0x2E, 0x9F, 0x15,
    0xA5, 0xDD, 0xBF, 0xC5, 0x78, 0x7E, 0x50, 0xB5,
    0xCC, 0x55, 0xEE, 0x50, 0x7B, 0xCB, 0x08, 0x4E,
    0x47, 0x9A, 0xD3, 0x63, 0xAC, 0x36, 0x6B, 0x95,
    0xA9, 0x8C, 0xA5, 0xF3, 0x00, 0x0B, 0x14, 0x79
};
static size_t TEST_16_EXPECTED_LEN = sizeof(TEST_16_EXPECTED);

static uint8_t TEST_16_TAG_LEN = 16;

/* Test 17:

    The next tuple shows a result with a tag length of 96 bits and a
    different key.

    K: 0F0E0D0C0B0A09080706050403020100

    N: BBAA9988776655443322110D
    A: 000102030405060708090A0B0C0D0E0F1011121314151617
        18191A1B1C1D1E1F2021222324252627
    P: 000102030405060708090A0B0C0D0E0F1011121314151617
        18191A1B1C1D1E1F2021222324252627
    C: 1792A4E31E0755FB03E31B22116E6C2DDF9EFD6E33D536F1
        A0124B0A55BAE884ED93481529C76B6AD0C515F4D1CDD4FD
        AC4F02AA

 */

static uint8_t TEST_17_KEY[] = {
    0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
    0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};

static uint8_t TEST_17_NONCE[] = {
    0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
    0x33, 0x22, 0x11, 0x0D,
};
static size_t TEST_17_NONCE_LEN = 12;

static uint8_t TEST_17_INPUT[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
};
static size_t TEST_17_INPUT_LEN = sizeof(TEST_17_INPUT);

static uint8_t TEST_17_ADATA[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
};
static size_t TEST_17_ADATA_LEN = sizeof(TEST_17_ADATA);

static uint8_t TEST_17_EXPECTED[] = {
    0x17, 0x92, 0xA4, 0xE3, 0x1E, 0x07, 0x55, 0xFB,
    0x03, 0xE3, 0x1B, 0x22, 0x11, 0x6E, 0x6C, 0x2D,
    0xDF, 0x9E, 0xFD, 0x6E, 0x33, 0xD5, 0x36, 0xF1,
    0xA0, 0x12, 0x4B, 0x0A, 0x55, 0xBA, 0xE8, 0x84,
    0xED, 0x93, 0x48, 0x15, 0x29, 0xC7, 0x6B, 0x6A,
    0xD0, 0xC5, 0x15, 0xF4, 0xD1, 0xCD, 0xD4, 0xFD,
    0xAC, 0x4F, 0x02, 0xAA
};
static size_t TEST_17_EXPECTED_LEN = sizeof(TEST_17_EXPECTED);

static uint8_t TEST_17_TAG_LEN = 12;

/* Share test buffer output */
static uint8_t data[60];

static void test_encrypt_op(uint8_t *key, uint8_t key_len,
                            uint8_t *adata, size_t adata_len,
                            uint8_t *nonce, uint8_t nonce_len,
                            uint8_t *plain, size_t plain_len,
                            uint8_t *output_expected,
                            size_t output_expected_len,
                            uint8_t tag_length)
{
    cipher_t cipher;
    int len, err, cmp;

    TEST_ASSERT_MESSAGE(sizeof(data) >= output_expected_len,
                        "Output buffer too small");

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_encrypt_ocb(&cipher, adata, adata_len,
                             tag_length, nonce, nonce_len,
                             plain, plain_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_expected_len, len);

    cmp = compare(output_expected, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");
}

#define do_test_encrypt_op(name) do { \
        test_encrypt_op(TEST_ ## name ## _KEY, TEST_KEY_LEN, \
                        TEST_ ## name ## _ADATA, TEST_ ## name ## _ADATA_LEN, \
                        TEST_ ## name ## _NONCE, TEST_ ## name ## _NONCE_LEN, \
                    \
                        TEST_ ## name ## _INPUT, TEST_ ## name ## _INPUT_LEN, \
                    \
                        TEST_ ## name ## _EXPECTED, \
                        TEST_ ## name ## _EXPECTED_LEN, TEST_ ## name ## _TAG_LEN \
                        ); \
} while (0)

static void test_crypto_modes_ocb_encrypt(void)
{
    do_test_encrypt_op(1);
    do_test_encrypt_op(2);
    do_test_encrypt_op(3);
    do_test_encrypt_op(4);
    do_test_encrypt_op(16);
    do_test_encrypt_op(17);
}

static void test_decrypt_op(uint8_t *key, uint8_t key_len,
                            uint8_t *adata, size_t adata_len,
                            uint8_t *nonce, uint8_t nonce_len,
                            uint8_t *encrypted, size_t encrypted_len,
                            uint8_t *output_expected,
                            size_t output_expected_len,
                            uint8_t tag_length)
{
    cipher_t cipher;
    int len, err, cmp;

    TEST_ASSERT_MESSAGE(sizeof(data) >= output_expected_len,
                        "Output buffer too small");

    err = cipher_init(&cipher, CIPHER_AES, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_decrypt_ocb(&cipher, adata, adata_len,
                             tag_length, nonce, nonce_len,
                             encrypted, encrypted_len, data);
    TEST_ASSERT_MESSAGE(len >= 0, "Decryption failed");

    TEST_ASSERT_EQUAL_INT(output_expected_len, len);
    cmp = compare(output_expected, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");

    /* do some negative tests for the tag verification */
    if (adata_len > 0) {
        /* Drop one byte of auth data */
        len = cipher_decrypt_ocb(&cipher, adata, adata_len - 1,
                                 tag_length, nonce, nonce_len,
                                 encrypted, encrypted_len, data);
        TEST_ASSERT_EQUAL_INT(OCB_ERR_INVALID_TAG, len);
        /* Alter one byte of auth data */
        adata[0] = adata[0] ^ 0x01;
        len = cipher_decrypt_ocb(&cipher, adata, adata_len,
                                 tag_length, nonce, nonce_len,
                                 encrypted, encrypted_len, data);
        TEST_ASSERT_EQUAL_INT(OCB_ERR_INVALID_TAG, len);
        adata[0] = adata[0] ^ 0x01;
    }
    /* Drop one byte of the nonce */
    len = cipher_decrypt_ocb(&cipher, adata, adata_len,
                             tag_length, nonce, nonce_len - 1,
                             encrypted, encrypted_len, data);
    TEST_ASSERT_EQUAL_INT(OCB_ERR_INVALID_TAG, len);
    /* Alter one byte of the nonce */
    nonce[0] = nonce[0] ^ 0x01;
    len = cipher_decrypt_ocb(&cipher, adata, adata_len,
                             tag_length, nonce, nonce_len,
                             encrypted, encrypted_len, data);
    TEST_ASSERT_EQUAL_INT(OCB_ERR_INVALID_TAG, len);
    nonce[0] = nonce[0] ^ 0x01;
    /* Alter one byte of the ciphertext */
    encrypted[0] = encrypted[0] ^ 0x01;
    len = cipher_decrypt_ocb(&cipher, adata, adata_len,
                             tag_length, nonce, nonce_len,
                             encrypted, encrypted_len, data);
    TEST_ASSERT_EQUAL_INT(OCB_ERR_INVALID_TAG, len);
    encrypted[0] = encrypted[0] ^ 0x01;
    /* Alter one byte of the tag */
    encrypted[encrypted_len - 1] = encrypted[encrypted_len - 1] ^ 0x01;
    len = cipher_decrypt_ocb(&cipher, adata, adata_len,
                             tag_length, nonce, nonce_len,
                             encrypted, encrypted_len, data);
    TEST_ASSERT_EQUAL_INT(OCB_ERR_INVALID_TAG, len);
    encrypted[encrypted_len - 1] = encrypted[encrypted_len - 1] ^ 0x01;
}

#define do_test_decrypt_op(name) do { \
        test_decrypt_op(TEST_ ## name ## _KEY, TEST_KEY_LEN, \
                        TEST_ ## name ## _ADATA, TEST_ ## name ## _ADATA_LEN, \
                        TEST_ ## name ## _NONCE, TEST_ ## name ## _NONCE_LEN, \
                    \
                        TEST_ ## name ## _EXPECTED, \
                        TEST_ ## name ## _EXPECTED_LEN, \
                    \
                        TEST_ ## name ## _INPUT, \
                        TEST_ ## name ## _INPUT_LEN, \
                    \
                        TEST_ ## name ## _TAG_LEN \
                        ); \
} while (0)

static void test_crypto_modes_ocb_decrypt(void)
{
    do_test_decrypt_op(1);
    do_test_decrypt_op(2);
    do_test_decrypt_op(3);
    do_test_decrypt_op(4);
    do_test_decrypt_op(16);
    do_test_decrypt_op(17);
}

static void test_crypto_modes_ocb_bad_parameter_values(void)
{
    uint8_t key[16], auth_data[1], nonce[16], input[16], output[32];
    cipher_t cipher;

    cipher_init(&cipher, CIPHER_AES, key, 16);
    /* tag length must be positive */
    int rv = cipher_encrypt_ocb(&cipher, auth_data, sizeof(auth_data), 0, nonce,
                                15, input, sizeof(input), output);
    TEST_ASSERT_EQUAL_INT(OCB_ERR_INVALID_TAG_LENGTH, rv);
    /* tag length must be <= 16 */
    rv = cipher_encrypt_ocb(&cipher, auth_data, sizeof(auth_data), 17, nonce,
                            15, input, sizeof(input), output);
    TEST_ASSERT_EQUAL_INT(OCB_ERR_INVALID_TAG_LENGTH, rv);
    /* nonce must not be empty */
    rv = cipher_encrypt_ocb(&cipher, auth_data, sizeof(auth_data), 16, nonce, 0,
                            input, sizeof(input), output);
    TEST_ASSERT_EQUAL_INT(OCB_ERR_INVALID_NONCE_LENGTH, rv);
    /* nonce must be <=15 */
    rv = cipher_encrypt_ocb(&cipher, auth_data, sizeof(auth_data), 16, nonce,
                            16, input, sizeof(input), output);
    TEST_ASSERT_EQUAL_INT(OCB_ERR_INVALID_NONCE_LENGTH, rv);
}

Test *tests_crypto_modes_ocb_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_modes_ocb_encrypt),
        new_TestFixture(test_crypto_modes_ocb_decrypt),
        new_TestFixture(test_crypto_modes_ocb_bad_parameter_values),
    };

    EMB_UNIT_TESTCALLER(crypto_modes_ocb_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_modes_ocb_tests;
}
