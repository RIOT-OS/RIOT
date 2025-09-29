/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include "embUnit.h"

#include "bit.h"

static void test_bit8(void)
{
    uint8_t tmp, word = UINT8_MAX;

    for (unsigned i = 0; i < sizeof(word) * 8; ++i) {
        TEST_ASSERT_EQUAL_INT(1, bit_check8(&word, i));
    }

    tmp = word;
    for (unsigned i = 0; i < sizeof(word) * 8; i += 2) {
        bit_clear8(&word, i);
        tmp &= ~(1UL << i);
        TEST_ASSERT_EQUAL_INT(tmp, word);
    }

    word <<= 1;
    tmp = word;
    for (unsigned i = 1; i < sizeof(word) * 8; i += 2) {
        bit_set8(&word, i);
        tmp |= 1UL << i;
        TEST_ASSERT_EQUAL_INT(tmp, word);
    }
}

static void test_bit16(void)
{
    uint16_t tmp, word = UINT16_MAX;

    for (unsigned i = 0; i < sizeof(word) * 8; ++i) {
        TEST_ASSERT_EQUAL_INT(1, bit_check16(&word, i));
    }

    tmp = word;
    for (unsigned i = 0; i < sizeof(word) * 8; i += 2) {
        bit_clear16(&word, i);
        tmp &= ~(1UL << i);
        TEST_ASSERT_EQUAL_INT(tmp, word);
    }

    word <<= 1;
    tmp = word;
    for (unsigned i = 1; i < sizeof(word) * 8; i += 2) {
        bit_set16(&word, i);
        tmp |= 1UL << i;
        TEST_ASSERT_EQUAL_INT(tmp, word);
    }
}

static void test_bit32(void)
{
    uint32_t tmp, word = UINT32_MAX;

    for (unsigned i = 0; i < sizeof(word) * 8; ++i) {
        TEST_ASSERT_EQUAL_INT(1, bit_check32(&word, i));
    }

    tmp = word;
    for (unsigned i = 0; i < sizeof(word) * 8; i += 2) {
        bit_clear32(&word, i);
        tmp &= ~(1UL << i);
        TEST_ASSERT_EQUAL_INT(tmp, word);
    }

    word <<= 1;
    tmp = word;
    for (unsigned i = 1; i < sizeof(word) * 8; i += 2) {
        bit_set32(&word, i);
        tmp |= 1UL << i;
        TEST_ASSERT_EQUAL_INT(tmp, word);
    }
}

Test *tests_bit_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_bit8),
        new_TestFixture(test_bit16),
        new_TestFixture(test_bit32),
    };

    EMB_UNIT_TESTCALLER(bit_tests, NULL, NULL, fixtures);

    return (Test *)&bit_tests;
}

void tests_bit(void)
{
    TESTS_RUN(tests_bit_tests());
}
