/*
 * Copyright 2025 Bennet Hattesen <bennet.blischke@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <stdint.h>

#include "embUnit/embUnit.h"

#include "checksum/crc16_ccitt.h"

#include "tests-checksum.h"

static int calc_and_compare_crc16_ccitt_fcs_with_update(const unsigned char *buf,
                                                           size_t len, size_t split,
                                                           uint16_t expected)
{
    uint16_t result = crc16_ccitt_fcs_start(buf, split);

    result = crc16_ccitt_fcs_finish(result, buf + split, len - split);
    return result == expected;
}

static int calc_and_compare_crc16_ccitt_fcs(const unsigned char *buf, size_t len,
                                               uint16_t expected)
{
    uint16_t result = crc16_ccitt_fcs_calc(buf, len);

    return result == expected;
}

static void test_checksum_crc16_ccitt_fcs_sequence_empty(void)
{
    unsigned char buf[] = "";
    uint16_t expect = 0x0000;

    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs(buf, sizeof(buf) - 1, expect));
    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs_with_update(buf, sizeof(buf) - 1,
                                                                (sizeof(buf) - 1) / 2, expect));
}

static void test_checksum_crc16_ccitt_fcs_sequence_1a(void)
{
    unsigned char buf[] = "A";
    uint16_t expect = 0xA3F5;

    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs(buf, sizeof(buf) - 1, expect));
    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs_with_update(buf, sizeof(buf) - 1,
                                                                (sizeof(buf) - 1) / 2, expect));
}

static void test_checksum_crc16_ccitt_fcs_sequence_256a(void)
{
    unsigned char buf[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAA";
    uint16_t expect = 0x0F9F;

    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs(buf, sizeof(buf) - 1, expect));
    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs_with_update(buf, sizeof(buf) - 1,
                                                                (sizeof(buf) - 1) / 2, expect));
}

static void test_checksum_crc16_ccitt_fcs_sequence_1to9(void)
{
    unsigned char buf[] = "123456789";
    uint16_t expect = 0x906E;

    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs(buf, sizeof(buf) - 1, expect));
    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs_with_update(buf, sizeof(buf)
                                                                - 1, (sizeof(buf) - 1) / 2,
                                                                expect));
}

static void test_checksum_crc16_ccitt_fcs_sequence_4bytes(void)
{
    unsigned char buf[] = { 0x12, 0x34, 0x56, 0x78 };
    uint16_t expect = 0x9B2E;

    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs(buf, sizeof(buf), expect));
    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs_with_update(buf, sizeof(buf),
                                                                sizeof(buf) / 2, expect));
}

static void test_checksum_crc16_ccitt_fcs_sequence_6bytes(void)
{
    unsigned char buf[] = { 0x00, 0x01, 0x02, 0x03 };
    uint16_t expect = 0xA729;

    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs(buf, sizeof(buf), expect));
    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs_with_update(buf, sizeof(buf),
                                                                sizeof(buf) / 2, expect));

    unsigned char buf2[] = { 0x00, 0x01, 0x02, 0x03, 0x29, 0xA7 };
    uint16_t expect2 = 0x0F47;

    // Residue test
    TEST_ASSERT(crc16_ccitt_fcs_start(buf2, sizeof(buf2)) == 0xF0B8);

    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs(buf2, sizeof(buf2), expect2));
    TEST_ASSERT(calc_and_compare_crc16_ccitt_fcs_with_update(buf2, sizeof(buf2),
                                                                sizeof(buf2) / 2, expect2));
}

Test *tests_checksum_crc16_ccitt_fcs_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_checksum_crc16_ccitt_fcs_sequence_empty),
        new_TestFixture(test_checksum_crc16_ccitt_fcs_sequence_1a),
        new_TestFixture(test_checksum_crc16_ccitt_fcs_sequence_256a),
        new_TestFixture(test_checksum_crc16_ccitt_fcs_sequence_1to9),
        new_TestFixture(test_checksum_crc16_ccitt_fcs_sequence_4bytes),
        new_TestFixture(test_checksum_crc16_ccitt_fcs_sequence_6bytes),
    };

    EMB_UNIT_TESTCALLER(checksum_crc16_ccitt_fcs_tests, NULL, NULL, fixtures);

    return (Test *)&checksum_crc16_ccitt_fcs_tests;
}
