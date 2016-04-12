/*
 * Copyright 2016 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#include "embUnit/embUnit.h"

#include "checksum/fletcher32.h"

#include "tests-checksum.h"

static int calc_and_compare_checksum(const unsigned char *buf, size_t len,
                                     uint32_t expected)
{
    const uint16_t *buf16 = (const uint16_t *) buf;
    size_t len16 = len/2;
    uint32_t result = fletcher32(buf16, len16);

    return result == expected;
}

static void test_checksum_fletcher32_empty(void)
{
    /* the initial checksum value is 0xFFFFFFFF */
    unsigned char buf[] = "";
    uint32_t expect = 0xFFFFFFFF;

    TEST_ASSERT(calc_and_compare_checksum(buf, sizeof(buf) - 1, expect));
}

static void test_checksum_fletcher32_0to1_undetected(void)
{
    /* fletcher cannot distinguish between all 0 and all 1 segments */
    unsigned char buf0[16] = {
        0xA1, 0xA1, 0xA1, 0xA1,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x1A, 0x1A, 0x1A, 0x1A,
    };
    uint32_t expect = fletcher32((const uint16_t *) buf0, sizeof(buf0)/2);
    unsigned char buf1[16] = {
        0xA1, 0xA1, 0xA1, 0xA1,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0x1A, 0x1A, 0x1A, 0x1A,
    };

    TEST_ASSERT(calc_and_compare_checksum(buf1, sizeof(buf1), expect));
}

static void test_checksum_fletcher32_atof(void)
{
    /* XXX: not verified with external implementation yet */
    unsigned char buf[] = "abcdef";
    uint32_t expect = 0x56502d2a;

    TEST_ASSERT(calc_and_compare_checksum(buf, sizeof(buf) - 1, expect));
}

Test *tests_checksum_fletcher32_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_checksum_fletcher32_empty),
        new_TestFixture(test_checksum_fletcher32_0to1_undetected),
        new_TestFixture(test_checksum_fletcher32_atof),
    };

    EMB_UNIT_TESTCALLER(checksum_fletcher32_tests, NULL, NULL, fixtures);

    return (Test *)&checksum_fletcher32_tests;
}
