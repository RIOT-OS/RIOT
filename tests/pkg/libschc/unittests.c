/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include "embUnit.h"

#include "bit_operations.h"

static void test_copy_bits__1bit_shift(void)
{
    static const uint8_t src[] = { 0xE0, 0xBC, 0x7D, 0xFF, 0xFE, 0xCB, 0xF5, 0x50 };
    static const uint8_t exp_dst[] = {
        (0xE0 >> 1),
        ((0xE0 << 7) & 0xFF) | 0xBC >> 1,
        ((0xBC << 7) & 0xFF) | 0x7D >> 1,
        ((0x7D << 7) & 0xFF) | 0xFF >> 1,
        ((0xFF << 7) & 0xFF) | 0xFE >> 1,
        ((0xFE << 7) & 0xFF) | 0xCB >> 1,
        ((0xCB << 7) & 0xFF) | 0xF5 >> 1,
        ((0xF5 << 7) & 0xFF) | 0x50 >> 1,
        ((0x50 << 7) & 0xFF)
    };
    static uint8_t dst[sizeof(exp_dst)] = { 0 };

    copy_bits(dst, 1, src, 0, sizeof(src) * 8);
    TEST_ASSERT_EQUAL_INT(sizeof(exp_dst), sizeof(dst));
    for (unsigned i = 0; i < sizeof(exp_dst); i++) {
        TEST_ASSERT_EQUAL_INT(exp_dst[i], dst[i]);
    }
}

static void test_copy_bits__2bit_shift(void)
{
    static const uint8_t src[] = {
        0x80, 0x00, 0x94, 0x11, 0xB9, 0x14, 0x00, 0x02, 0x48, 0x32, 0x9A, 0x00
    };
    static const uint8_t exp_dst[] = {
        (0x80 >> 2),
        ((0x80 << 6) & 0xFF) | 0x00 >> 2,
        ((0x00 << 6) & 0xFF) | 0x94 >> 2,
        ((0x94 << 6) & 0xFF) | 0x11 >> 2,
        ((0x11 << 6) & 0xFF) | 0xB9 >> 2,
        ((0xB9 << 6) & 0xFF) | 0x14 >> 2,
        ((0x14 << 6) & 0xFF) | 0x00 >> 2,
        ((0x00 << 6) & 0xFF) | 0x02 >> 2,
        ((0x02 << 6) & 0xFF) | 0x48 >> 2,
        ((0x48 << 6) & 0xFF) | 0x32 >> 2,
        ((0x32 << 6) & 0xFF) | 0x9A >> 2,
        ((0x9A << 6) & 0xFF) | 0x00 >> 2,
        ((0x00 << 6) & 0xFF),
    };
    static uint8_t dst[sizeof(exp_dst)] = { 0 };

    copy_bits(dst, 2, src, 0, sizeof(src) * 8);
    TEST_ASSERT_EQUAL_INT(sizeof(exp_dst), sizeof(dst));
    for (unsigned i = 0; i < sizeof(exp_dst); i++) {
        TEST_ASSERT_EQUAL_INT(exp_dst[i], dst[i]);
    }
}

int unittests(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_copy_bits__1bit_shift),
        new_TestFixture(test_copy_bits__2bit_shift),
    };
    EMB_UNIT_TESTCALLER(libschc_tests, NULL, NULL, fixtures);
    TESTS_START();
    TESTS_RUN((Test *)&libschc_tests);
    TESTS_END();
    return 0;
}

/** @} */
