/*
 * Copyright 2022 Bennet Blischke <bennet.blischke@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#include "embUnit/embUnit.h"

#include "checksum/crc16_ccitt.h"

#include "tests-checksum.h"

static int calc_and_compare_crc16_ccitt_false_with_update(const unsigned char *buf,
                                                          size_t len, size_t split,
                                                          uint16_t expected)
{
    uint16_t result = crc16_ccitt_false_calc(buf, split);

    result = crc16_ccitt_false_update(result, buf + split, len - split);
    return result == expected;
}

static int calc_and_compare_crc16_ccitt_false(const unsigned char *buf, size_t len,
                                              uint16_t expected)
{
    uint16_t result = crc16_ccitt_false_calc(buf, len);

    return result == expected;
}

static void test_checksum_crc16_ccitt_false_sequence_empty(void)
{
    unsigned char buf[] = "";
    uint16_t expect = 0xFFFF;

    TEST_ASSERT(calc_and_compare_crc16_ccitt_false(buf, sizeof(buf) - 1, expect));
    TEST_ASSERT(calc_and_compare_crc16_ccitt_false_with_update(buf, sizeof(buf) - 1,
                                                               (sizeof(buf) - 1) / 2, expect));
}

static void test_checksum_crc16_ccitt_false_sequence_1a(void)
{
    unsigned char buf[] = "A";
    uint16_t expect = 0xB915;

    TEST_ASSERT(calc_and_compare_crc16_ccitt_false(buf, sizeof(buf) - 1, expect));
    TEST_ASSERT(calc_and_compare_crc16_ccitt_false_with_update(buf, sizeof(buf) - 1,
                                                               (sizeof(buf) - 1) / 2, expect));
}

static void test_checksum_crc16_ccitt_false_sequence_256a(void)
{
    unsigned char buf[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAA";
    uint16_t expect = 0xEA0B;

    TEST_ASSERT(calc_and_compare_crc16_ccitt_false(buf, sizeof(buf) - 1, expect));
    TEST_ASSERT(calc_and_compare_crc16_ccitt_false_with_update(buf, sizeof(buf) - 1,
                                                               (sizeof(buf) - 1) / 2, expect));
}

static void test_checksum_crc16_ccitt_false_sequence_1to9(void)
{
    unsigned char buf[] = "123456789";
    uint16_t expect = 0x29B1;

    TEST_ASSERT(calc_and_compare_crc16_ccitt_false(buf, sizeof(buf) - 1, expect));
    TEST_ASSERT(calc_and_compare_crc16_ccitt_false_with_update(buf, sizeof(buf)
                                                               - 1, (sizeof(buf) - 1) / 2, expect));
}

static void test_checksum_crc16_ccitt_false_sequence_4bytes(void)
{
    unsigned char buf[] = { 0x12, 0x34, 0x56, 0x78 };
    uint16_t expect = 0x30EC;

    TEST_ASSERT(calc_and_compare_crc16_ccitt_false(buf, sizeof(buf), expect));
    TEST_ASSERT(calc_and_compare_crc16_ccitt_false_with_update(buf, sizeof(buf),
                                                               sizeof(buf) / 2, expect));
}

Test *tests_checksum_crc16_ccitt_false_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_checksum_crc16_ccitt_false_sequence_empty),
        new_TestFixture(test_checksum_crc16_ccitt_false_sequence_1a),
        new_TestFixture(test_checksum_crc16_ccitt_false_sequence_256a),
        new_TestFixture(test_checksum_crc16_ccitt_false_sequence_1to9),
        new_TestFixture(test_checksum_crc16_ccitt_false_sequence_4bytes),
    };

    EMB_UNIT_TESTCALLER(checksum_crc16_ccitt_false_tests, NULL, NULL, fixtures);

    return (Test *)&checksum_crc16_ccitt_false_tests;
}
