/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <errno.h>
#include "embUnit.h"

#include "bcd.h"

static void test_bcd_from_byte__zero(void)
{
    TEST_ASSERT_EQUAL_INT(0x00, bcd_from_byte(0));
}

static void test_bcd_from_byte__greater_99(void)
{
    /* output is garbled intentionally */
    TEST_ASSERT_EQUAL_INT(0xa0, bcd_from_byte(100));
    TEST_ASSERT_EQUAL_INT(0x95, bcd_from_byte(255));
}

static void test_bcd_from_byte(void)
{
    TEST_ASSERT_EQUAL_INT(0x01, bcd_from_byte(1));
    TEST_ASSERT_EQUAL_INT(0x09, bcd_from_byte(9));
    TEST_ASSERT_EQUAL_INT(0x10, bcd_from_byte(10));
    TEST_ASSERT_EQUAL_INT(0x11, bcd_from_byte(11));
    TEST_ASSERT_EQUAL_INT(0x99, bcd_from_byte(99));
}

static void test_bcd_to_byte__zero(void)
{
    TEST_ASSERT_EQUAL_INT(0, bcd_to_byte(0x00));
}

static void test_bcd_to_byte__greater_0x99(void)
{
    TEST_ASSERT_EQUAL_INT(100, bcd_to_byte(0xa0));
    TEST_ASSERT_EQUAL_INT(110, bcd_to_byte(0xaa));
    TEST_ASSERT_EQUAL_INT(165, bcd_to_byte(0xff));
}

static void test_bcd_to_byte(void)
{
    TEST_ASSERT_EQUAL_INT( 1, bcd_to_byte(0x01));
    TEST_ASSERT_EQUAL_INT( 9, bcd_to_byte(0x09));
    TEST_ASSERT_EQUAL_INT(10, bcd_to_byte(0x10));
    TEST_ASSERT_EQUAL_INT(11, bcd_to_byte(0x11));
    TEST_ASSERT_EQUAL_INT(99, bcd_to_byte(0x99));
}

static void test_bcd_buf_from_u32(void)
{
    uint32_t buf = UINT32_MAX; /* test if full buffer gets written */

    TEST_ASSERT_EQUAL_INT(1, bcd_buf_from_u32(0, &buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(0x0, buf);

    TEST_ASSERT_EQUAL_INT(1, bcd_buf_from_u32(12, &buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(0x12, buf);

    TEST_ASSERT_EQUAL_INT(2, bcd_buf_from_u32(123, &buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(0x123, buf);

    TEST_ASSERT_EQUAL_INT(3, bcd_buf_from_u32(123456, &buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(0x123456, buf);

    TEST_ASSERT_EQUAL_INT(4, bcd_buf_from_u32(12345678, &buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(0x12345678, buf);

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, bcd_buf_from_u32(123456789, &buf, sizeof(buf)));

    /* test empty buffer */
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, bcd_buf_from_u32(0, NULL, 0));
}

static void test_bcd_buf_to_u32(void)
{
    char buf[4];

    bcd_buf_from_u32(1, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(1, bcd_buf_to_u32(buf, sizeof(buf)));

    bcd_buf_from_u32(12, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(12, bcd_buf_to_u32(buf, sizeof(buf)));

    bcd_buf_from_u32(123, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(123, bcd_buf_to_u32(buf, sizeof(buf)));

    bcd_buf_from_u32(1234, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(1234, bcd_buf_to_u32(buf, sizeof(buf)));
}

static void test_bcd_buf_from_str(void)
{
    uint8_t buf[4];

    TEST_ASSERT_EQUAL_INT(1, bcd_buf_from_str("1", 1, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(1, bcd_buf_to_u32(buf, sizeof(buf)));

    TEST_ASSERT_EQUAL_INT(1, bcd_buf_from_str("12", 2, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(12, bcd_buf_to_u32(buf, sizeof(buf)));

    TEST_ASSERT_EQUAL_INT(2, bcd_buf_from_str("123", 3, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(123, bcd_buf_to_u32(buf, sizeof(buf)));

    TEST_ASSERT_EQUAL_INT(2, bcd_buf_from_str("1234", 4, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(1234, bcd_buf_to_u32(buf, sizeof(buf)));

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, bcd_buf_from_str("1234567890", 10, buf, sizeof(buf)));
}

Test *tests_bcd_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_bcd_from_byte__zero),
        new_TestFixture(test_bcd_from_byte__greater_99),
        new_TestFixture(test_bcd_from_byte),
        new_TestFixture(test_bcd_to_byte__zero),
        new_TestFixture(test_bcd_to_byte__greater_0x99),
        new_TestFixture(test_bcd_to_byte),
        new_TestFixture(test_bcd_buf_from_u32),
        new_TestFixture(test_bcd_buf_to_u32),
        new_TestFixture(test_bcd_buf_from_str),
    };

    EMB_UNIT_TESTCALLER(bcd_tests, NULL, NULL, fixtures);

    return (Test *)&bcd_tests;
}

void tests_bcd(void)
{
    TESTS_RUN(tests_bcd_tests());
}
