/*
 * Copyright (C) 2015 Lucas Jenß
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     unittests
 * @brief
 * @{
 *
 * @brief       Tests for Error Correction Codes
 *
 * @author      Lucas Jenß <lucas@x3ro.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */
#include <string.h>
#include "embUnit.h"

#include "ecc/hamming256.h"
#include "ecc/golay2412.h"
#include "ecc/repetition.h"

/* source for random bytes: https://www.random.org/bytes */
unsigned char data_in[] =                        { 201, 240, 154,   5, 227,  60, 116, 192, 214 };
/* golay encoded sequence */
unsigned char msg_enc_golay[] =                  { 220, 124, 159,  19,  64, 154, 135, 208,  94,
                                                   227,  35,  60, 248,  39,  76, 187,  16, 214 };
/* Added errors. golay(24, 12) can correct up to 3 errors in one 24-bit
 * symbol (= 3 bytes). Positions for bitflips generated at
 * https://www.random.org/bytes */
unsigned char msg_enc_golay_3_err_per_symbol[] = { 252,  60, 191,  18,  64, 190, 135, 209,  22,
                                                   227,  43, 188, 252,  33,  76,  57,  16, 212 };
/* repetition encoded sequence */
unsigned char msg_enc_rep[] =                    { 201, 240, 154,   5, 227,  60, 116, 192, 214,
                                                   201, 240, 154,   5, 227,  60, 116, 192, 214,
                                                   201, 240, 154,   5, 227,  60, 116, 192, 214 };
/* Added errors. repetition(3,1) can correct up to (ECC_REPETITION_COUNT-1)/2 errors
 * per symbol (one encoded has 3 bytes here). Positions for bitflips generated at
 * https://www.random.org/bytes */
unsigned char msg_enc_rep_1_err_per_symbol[] =   { 193, 240, 158,   5, 226,  60, 116, 193, 214,
                                                   201, 242, 154,   5, 227,  28, 116, 192, 210,
                                                   201, 240, 154,  21, 227,  60, 112, 192, 214 };

static void test_hamming256_single(void)
{
    uint8_t data[256];
    uint8_t ecc[3];
    uint8_t result;

    memset(data, 0xAB, 256);

    hamming_compute256x(data, 256, ecc);
    result = hamming_verify256x(data, 256, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_NONE, result);

    data[10] |= (2 << 3);
    result = hamming_verify256x(data, 256, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_SINGLEBIT, result);

    data[10] |= (2 << 3);
    data[20] |= (2 << 5);
    result = hamming_verify256x(data, 256, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_MULTIPLEBITS, result);

    memset(data, 0xAB, 256);
    ecc[1] ^= 1; /* Flip first bit, corrupting the ECC */
    result = hamming_verify256x(data, 256, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_ECC, result);
}

static void test_hamming256_padding(void)
{
    uint8_t data[203];
    uint8_t ecc[3];
    uint8_t result;

    memset(data, 0xAB, 203);

    hamming_compute256x(data, 203, ecc);
    result = hamming_verify256x(data, 203, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_NONE, result);

    data[10] |= (2 << 3);
    result = hamming_verify256x(data, 203, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_SINGLEBIT, result);

    data[10] |= (2 << 3);
    data[20] |= (2 << 5);
    result = hamming_verify256x(data, 203, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_MULTIPLEBITS, result);

    memset(data, 0xAB, 203);
    ecc[1] ^= 1; /* Flip first bit, corrupting the ECC */
    result = hamming_verify256x(data, 203, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_ECC, result);
}

static void test_golay2412_message_encode(void)
{
    unsigned char encoded[2 * sizeof(data_in)];

    golay2412_encode(sizeof(data_in), &data_in[0], &encoded[0]);

    TEST_ASSERT_EQUAL_INT(0, memcmp(&msg_enc_golay, &encoded, sizeof(encoded)));
}

static void test_golay2412_message_noerr(void)
{
    unsigned char result[sizeof(data_in)];
    unsigned char encoded[2 * sizeof(data_in)];

    golay2412_encode(sizeof(data_in), &data_in[0], &encoded[0]);
    golay2412_decode(sizeof(result), &encoded[0], &result[0]);
    TEST_ASSERT_EQUAL_INT(0, memcmp(&data_in, &result, sizeof(data_in)));
}

static void test_golay2412_message_decode_success(void)
{
    unsigned char result[sizeof(data_in)];

    golay2412_decode(sizeof(result), &msg_enc_golay_3_err_per_symbol[0], &result[0]);

    TEST_ASSERT_EQUAL_INT(0, memcmp(&data_in, &result, sizeof(data_in)));
}

static void test_golay2412_message_decode_fail(void)
{
    unsigned char result[sizeof(data_in)];

    /* adding a 4th error to the first symbol leads to an
     * uncorrectable sequence */
    msg_enc_golay_3_err_per_symbol[0] ^= (1L << 6);

    golay2412_decode(sizeof(result), &msg_enc_golay_3_err_per_symbol[0], &result[0]);

    TEST_ASSERT(memcmp(&data_in, &result, sizeof(data_in)));
}
static void test_repetition_message_encode(void)
{
    unsigned char encoded[ECC_REPETITION_COUNT * sizeof(data_in)];

    repetition_encode(sizeof(data_in), &data_in[0], &encoded[0]);

    TEST_ASSERT_EQUAL_INT(0, memcmp(encoded, msg_enc_rep, sizeof(encoded)));
}

static void test_repetition_message_noerr(void)
{
    unsigned char result[sizeof(data_in)];
    unsigned char encoded[ECC_REPETITION_COUNT * sizeof(data_in)];

    repetition_encode(sizeof(data_in), &data_in[0], &encoded[0]);
    repetition_decode(sizeof(result), &encoded[0], &result[0]);

    TEST_ASSERT_EQUAL_INT(0, memcmp(&data_in, &result, sizeof(data_in)));
}

static void test_repetition_message_decode_success(void)
{
    unsigned char result[sizeof(data_in)];

    repetition_decode(sizeof(result), &msg_enc_rep_1_err_per_symbol[0], &result[0]);

    TEST_ASSERT_EQUAL_INT(0, memcmp(&data_in, &result, sizeof(data_in)));
}

static void test_repetition_message_decode_fail(void)
{
    unsigned char result[sizeof(data_in)];

    /* adding a second error to the first symbol leads to an
     * uncorrectable sequence */
    msg_enc_rep_1_err_per_symbol[0] ^= (1L << 6);

    golay2412_decode(sizeof(result), &msg_enc_rep_1_err_per_symbol[0], &result[0]);

    TEST_ASSERT(memcmp(&data_in, &result, sizeof(data_in)));
}

TestRef test_all(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hamming256_single),
        new_TestFixture(test_hamming256_padding),
        new_TestFixture(test_golay2412_message_encode),
        new_TestFixture(test_golay2412_message_noerr),
        new_TestFixture(test_golay2412_message_decode_success),
        new_TestFixture(test_golay2412_message_decode_fail),
        new_TestFixture(test_repetition_message_encode),
        new_TestFixture(test_repetition_message_noerr),
        new_TestFixture(test_repetition_message_decode_success),
        new_TestFixture(test_repetition_message_decode_fail),
    };

    EMB_UNIT_TESTCALLER(EccTest, NULL, NULL, fixtures);
    return (TestRef) & EccTest;
}

void tests_ecc(void)
{
    TESTS_RUN(test_all());
}
