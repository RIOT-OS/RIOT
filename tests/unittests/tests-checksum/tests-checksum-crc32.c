/*
 * SPDX-FileCopyrightText: 2022 Benjamin Valentin <benpicco@googlemail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "checksum/crc32.h"
#include "tests-checksum.h"

static void test_checksum_crc32_sequence_empty(void)
{
    unsigned char buf[] = "";
    uint32_t expect = 0x0;

    TEST_ASSERT_EQUAL_INT(expect, crc32(buf, sizeof(buf) - 1));
}

static void test_checksum_crc32_sequence_1a(void)
{
    unsigned char buf[] = "A";
    uint32_t expect = 0xd3d99e8b;

    TEST_ASSERT_EQUAL_INT(expect, crc32(buf, sizeof(buf) - 1));
}

static void test_checksum_crc32_sequence_256a(void)
{
    unsigned char buf[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    uint32_t expect = 0x49975b13;

    TEST_ASSERT_EQUAL_INT(expect, crc32(buf, sizeof(buf) - 1));
}

static void test_checksum_crc32_sequence_1to9(void)
{
    unsigned char buf[] = "123456789";
    uint32_t expect = 0xcbf43926;

    TEST_ASSERT_EQUAL_INT(expect, crc32(buf, sizeof(buf) - 1));
}

static void test_checksum_crc32_sequence_4bytes(void)
{
    unsigned char buf[] = { 0x12, 0x34, 0x56, 0x78 };
    uint32_t expect = 0x4a090e98;

    TEST_ASSERT_EQUAL_INT(expect, crc32(buf, sizeof(buf)));
}

Test *tests_checksum_crc32_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_checksum_crc32_sequence_empty),
        new_TestFixture(test_checksum_crc32_sequence_1a),
        new_TestFixture(test_checksum_crc32_sequence_256a),
        new_TestFixture(test_checksum_crc32_sequence_1to9),
        new_TestFixture(test_checksum_crc32_sequence_4bytes),
    };

    EMB_UNIT_TESTCALLER(checksum_crc32_tests, NULL, NULL, fixtures);

    return (Test *)&checksum_crc32_tests;
}
