/*
 * Copyright (C) 2015 Lucas Jenß
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     fs
 * @brief
 * @{
 *
 * @brief       Tests for Hamming Code implementation
 * @author      Lucas Jenß <lucas@x3ro.de>
 */
#include <string.h>
#include "embUnit.h"

#include "ecc/hamming256.h"

static void test_single(void)
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
    ecc[1] ^= 1; // Flip first bit, corrupting the ECC
    result = hamming_verify256x(data, 256, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_ECC, result);
}

static void test_padding(void)
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
    ecc[1] ^= 1; // Flip first bit, corrupting the ECC
    result = hamming_verify256x(data, 203, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_ECC, result);
}

TestRef test_all(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_single),
        new_TestFixture(test_padding),
    };

    EMB_UNIT_TESTCALLER(EccTest, 0, 0, fixtures);
    return (TestRef) & EccTest;
}

void tests_ecc(void)
{
    TESTS_RUN(test_all());
}
