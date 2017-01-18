/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>

#include "embUnit/embUnit.h"

#include "checksum/ucrc16.h"

#include "tests-checksum.h"

#define CRC16_CCITT_SEED    (0x1d0f)

/* equivalent to crc16-ccitt version */
static int calc_and_compare_crc_be_with_update(const uint8_t *buf,
        size_t len, size_t split, uint16_t expected)
{
    uint16_t result = ucrc16_calc_be(buf, split, UCRC16_CCITT_POLY_BE,
                                     CRC16_CCITT_SEED);

    result = ucrc16_calc_be(buf + split, len - split, UCRC16_CCITT_POLY_BE,
                            result);

    return result == expected;
}

static int calc_and_compare_crc_be(const uint8_t *buf, size_t len,
        uint16_t expected)
{
    uint16_t result = ucrc16_calc_be(buf, len, UCRC16_CCITT_POLY_BE,
                                     CRC16_CCITT_SEED);

    return result == expected;
}

static void test_checksum_ucrc16_be_sequence_empty(void)
{
    uint8_t buf[] = "";
    uint16_t expect = CRC16_CCITT_SEED;

    TEST_ASSERT(calc_and_compare_crc_be(buf, sizeof(buf) - 1, expect));
    TEST_ASSERT(calc_and_compare_crc_be_with_update(buf, sizeof(buf) - 1,
                (sizeof(buf) - 1) / 2, expect));
}

static void test_checksum_ucrc16_be_sequence_1a(void)
{
    uint8_t buf[] = "A";
    uint16_t expect = 0x9479;

    TEST_ASSERT(calc_and_compare_crc_be(buf, sizeof(buf) - 1, expect));
    TEST_ASSERT(calc_and_compare_crc_be_with_update(buf, sizeof(buf) - 1,
                (sizeof(buf) - 1) / 2, expect));
}

static void test_checksum_ucrc16_be_sequence_256a(void)
{
    uint8_t buf[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    uint16_t expect = 0xE938;

    TEST_ASSERT(calc_and_compare_crc_be(buf, sizeof(buf) - 1, expect));
    TEST_ASSERT(calc_and_compare_crc_be_with_update(buf, sizeof(buf) - 1,
                (sizeof(buf) - 1) / 2, expect));
}

static void test_checksum_ucrc16_be_sequence_1to9(void)
{
    uint8_t buf[] = "123456789";
    uint16_t expect = 0xE5CC;

    TEST_ASSERT(calc_and_compare_crc_be(buf, sizeof(buf) - 1, expect));
    TEST_ASSERT(calc_and_compare_crc_be_with_update(buf, sizeof(buf)
                - 1, (sizeof(buf) - 1) / 2, expect));
}

static void test_checksum_ucrc16_be_sequence_4bytes(void)
{
    uint8_t buf[] = { 0x12, 0x34, 0x56, 0x78 };
    uint16_t expect = 0xBA3C;

    TEST_ASSERT(calc_and_compare_crc_be(buf, sizeof(buf), expect));
    TEST_ASSERT(calc_and_compare_crc_be_with_update(buf, sizeof(buf),
                sizeof(buf) / 2, expect));
}

static void test_checksum_ucrc16_le_ieee802164_frame(void)
{
    /* test data taken from first packet in
     * https://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=6LoWPAN.pcap.gz */
    uint8_t buf[] = { 0x41, 0xcc, 0xa4, 0xff, 0xff, 0x8a, 0x18, 0x00,
                      0xff, 0xff, 0xda, 0x1c, 0x00, 0x88, 0x18, 0x00,
                      0xff, 0xff, 0xda, 0x1c, 0x00, 0x41, 0x60, 0x00,
                      0x00, 0x00, 0x00, 0x19, 0x11, 0x40, 0xfe, 0x80,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c,
                      0xda, 0xff, 0xff, 0x00, 0x18, 0x88, 0xfe, 0x80,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c,
                      0xda, 0xff, 0xff, 0x00, 0x18, 0x8a, 0x04, 0x01,
                      0xf0, 0xb1, 0x00, 0x19, 0xea, 0x8a, 0x48, 0x65,
                      0x6c, 0x6c, 0x6f, 0x20, 0x30, 0x30, 0x33, 0x20,
                      0x30, 0x78, 0x43, 0x35, 0x39, 0x41, 0x0a };
    uint16_t expect = 0x31f9;
    uint16_t result = ucrc16_calc_le(buf, sizeof(buf), UCRC16_CCITT_POLY_LE, 0);

    TEST_ASSERT_EQUAL_INT(expect, result);
}

Test *tests_checksum_ucrc16_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        /* Reference values according to
         * http://srecord.sourceforge.net/crc16-ccitt.html */
        new_TestFixture(test_checksum_ucrc16_be_sequence_empty),
        new_TestFixture(test_checksum_ucrc16_be_sequence_1a),
        new_TestFixture(test_checksum_ucrc16_be_sequence_256a),
        new_TestFixture(test_checksum_ucrc16_be_sequence_1to9),
        new_TestFixture(test_checksum_ucrc16_be_sequence_4bytes),
        new_TestFixture(test_checksum_ucrc16_le_ieee802164_frame),
    };

    EMB_UNIT_TESTCALLER(checksum_ucrc16_tests, NULL, NULL, fixtures);

    return (Test *)&checksum_ucrc16_tests;
}
