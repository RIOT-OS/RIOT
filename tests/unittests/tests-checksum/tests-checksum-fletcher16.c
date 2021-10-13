/*
 * Copyright 2016 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#include "embUnit/embUnit.h"

#include "checksum/fletcher16.h"

#include "tests-checksum.h"

/* String is longer than 20 so it checks the wrap-around property of
 * fletcher16 */
static const unsigned char wrap_around_data[] = "u9k89rm88z58f3hGxTfgG3BnyKnyRP";

static int calc_and_compare_checksum(const unsigned char *buf, size_t len,
                                     uint16_t expected)
{
    uint16_t result = fletcher16(buf, len);

    return result == expected;
}

static void test_checksum_fletcher16_empty(void)
{
    unsigned char buf[] = "";
    uint16_t expect = 0xFFFF;

    TEST_ASSERT(calc_and_compare_checksum(buf, sizeof(buf) - 1, expect));
}

static void test_checksum_fletcher16_0to1_undetected(void)
{
    /* fletcher cannot distinguish between all 0 and all 1 segments */
    unsigned char buf0[16] = {
        0xA1, 0xA1, 0xA1, 0xA1,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x1A, 0x1A, 0x1A, 0x1A,
    };
    uint32_t expect = fletcher16(buf0, sizeof(buf0));
    unsigned char buf1[16] = {
        0xA1, 0xA1, 0xA1, 0xA1,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0x1A, 0x1A, 0x1A, 0x1A,
    };

    TEST_ASSERT(calc_and_compare_checksum(buf1, sizeof(buf1), expect));
}

static void test_checksum_fletcher16_atoe(void)
{
    /* verified with http://www.nitrxgen.net/hashgen/ */
    unsigned char buf[] = "abcde";
    uint16_t expect = 0xc8f0;

    TEST_ASSERT(calc_and_compare_checksum(buf, sizeof(buf) - 1, expect));
}

static void test_checksum_fletcher16_wrap_around(void)
{
    /* verified with http://www.nitrxgen.net/hashgen/ */
    uint16_t expect = 0xf122;

    TEST_ASSERT(calc_and_compare_checksum(wrap_around_data,
                                          sizeof(wrap_around_data) - 1, expect));
}

static void test_checksum_fletcher16_multipart(void)
{
    static const uint16_t expect = 0xf122;
    static const size_t half_len = sizeof(wrap_around_data)/2;
    fletcher16_ctx_t ctx;

    fletcher16_init(&ctx);
    fletcher16_update(&ctx, wrap_around_data, half_len);
    fletcher16_update(&ctx, wrap_around_data + half_len,
                      sizeof(wrap_around_data) - half_len - 1);
    TEST_ASSERT_EQUAL_INT(expect, fletcher16_finish(&ctx));
}

Test *tests_checksum_fletcher16_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_checksum_fletcher16_empty),
        new_TestFixture(test_checksum_fletcher16_0to1_undetected),
        new_TestFixture(test_checksum_fletcher16_atoe),
        new_TestFixture(test_checksum_fletcher16_wrap_around),
        new_TestFixture(test_checksum_fletcher16_multipart),
    };

    EMB_UNIT_TESTCALLER(checksum_fletcher16_tests, NULL, NULL, fixtures);

    return (Test *)&checksum_fletcher16_tests;
}
