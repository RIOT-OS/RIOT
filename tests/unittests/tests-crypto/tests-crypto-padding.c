/*
 * Copyright (C) 2017 Mathias Tausig
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <string.h>

#include "embUnit.h"
#include "crypto/padding.h"
#include "tests-crypto.h"

static uint8_t TEST_DATA[] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
};

static uint8_t TEST_1_BLOCKSIZE = 16;
static uint8_t TEST_1_PADDED_DATA[] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

static uint8_t TEST_2_BLOCKSIZE = 8;
static uint8_t TEST_2_PADDED_DATA[] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

static uint8_t TEST_3_BLOCKSIZE = 4;
static uint8_t TEST_3_PADDED_DATA[] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x04, 0x04, 0x04, 0x04,
};

static uint8_t TEST_4_BLOCKSIZE = 12;
static uint8_t TEST_4_PADDED_DATA[] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x04, 0x04, 0x04, 0x04,
};

static void test_pad_pkcs7_op(uint8_t *data, uint32_t data_len, uint8_t blocksize, uint8_t *expected_data, uint32_t expected_data_len)
{
    uint8_t padded_data[data_len + blocksize];
    int32_t padded_length = pkcs7_padding(data, data_len, blocksize, padded_data, data_len + blocksize);

    TEST_ASSERT_MESSAGE(padded_length > 0, "Padding failed");
    TEST_ASSERT_MESSAGE((unsigned int)padded_length == expected_data_len, "Padded data has wrong length");
    TEST_ASSERT_MESSAGE(0 == memcmp(padded_data, expected_data, padded_length), "Data padded incorrectly.");
}

static void test_unpad_pkcs7_op(uint8_t *data, uint32_t data_len, uint8_t blocksize, uint8_t *expected_data, uint32_t expected_data_len)
{
    int32_t unpadded_length = pkcs7_padding_remove(data, data_len, blocksize);

    TEST_ASSERT_MESSAGE(unpadded_length > 0, "Remove padding failed");
    TEST_ASSERT_MESSAGE((unsigned int)unpadded_length == expected_data_len, "Unpadded data has wrong length");
    TEST_ASSERT_MESSAGE(0 == memcmp(data, expected_data, unpadded_length), "Data unpadded incorrectly.");
}


static void test_pkcs7_padding(void)
{
    test_pad_pkcs7_op(TEST_DATA, sizeof(TEST_DATA),
                      TEST_1_BLOCKSIZE, TEST_1_PADDED_DATA, sizeof(TEST_1_PADDED_DATA));
    test_pad_pkcs7_op(TEST_DATA, sizeof(TEST_DATA),
                      TEST_2_BLOCKSIZE, TEST_2_PADDED_DATA, sizeof(TEST_2_PADDED_DATA));
    test_pad_pkcs7_op(TEST_DATA, sizeof(TEST_DATA),
                      TEST_3_BLOCKSIZE, TEST_3_PADDED_DATA, sizeof(TEST_3_PADDED_DATA));
    test_pad_pkcs7_op(TEST_DATA, sizeof(TEST_DATA),
                      TEST_4_BLOCKSIZE, TEST_4_PADDED_DATA, sizeof(TEST_4_PADDED_DATA));
}

static void test_pkcs7_padding_remove(void)
{
    test_unpad_pkcs7_op(TEST_1_PADDED_DATA, sizeof(TEST_1_PADDED_DATA), TEST_1_BLOCKSIZE,
                        TEST_DATA, sizeof(TEST_DATA));
    test_unpad_pkcs7_op(TEST_2_PADDED_DATA, sizeof(TEST_2_PADDED_DATA), TEST_2_BLOCKSIZE,
                        TEST_DATA, sizeof(TEST_DATA));
    test_unpad_pkcs7_op(TEST_3_PADDED_DATA, sizeof(TEST_3_PADDED_DATA), TEST_3_BLOCKSIZE,
                        TEST_DATA, sizeof(TEST_DATA));
    test_unpad_pkcs7_op(TEST_4_PADDED_DATA, sizeof(TEST_4_PADDED_DATA), TEST_4_BLOCKSIZE,
                        TEST_DATA, sizeof(TEST_DATA));
}

static void test_pkcs7_padding_remove_bad_data_length(void)
{
    uint8_t data[] = { 0x01 };
    uint8_t blocksize = 8;
    int32_t rv = pkcs7_padding_remove(data, sizeof(data), blocksize);

    TEST_ASSERT_EQUAL_INT(PADDING_INVALID_PADDING, rv);
}

static void test_pkcs7_padding_remove_bad_padding(void)
{
    uint8_t data[] = { 0xff, 0xff, 0xff, 0x2 };
    uint8_t blocksize = 4;
    int32_t rv = pkcs7_padding_remove(data, sizeof(data), blocksize);

    TEST_ASSERT_EQUAL_INT(PADDING_INVALID_PADDING, rv);
}

Test *tests_crypto_padding_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pkcs7_padding),
        new_TestFixture(test_pkcs7_padding_remove),
        new_TestFixture(test_pkcs7_padding_remove_bad_data_length),
        new_TestFixture(test_pkcs7_padding_remove_bad_padding),
    };

    EMB_UNIT_TESTCALLER(crypto_padding_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_padding_tests;
}
