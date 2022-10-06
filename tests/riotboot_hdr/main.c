/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Tests for module riotboot_hdr
 *
 * @author     Francisco Acosta <francisco.acosta@inria.fr>
 */

#include <stdio.h>

#include "riotboot/hdr.h"
#include "embUnit.h"

const riotboot_hdr_t riotboot_hdr_good = {
    .magic_number = RIOTBOOT_MAGIC,
    .version = 0x5bd19bff,
    .start_addr = 0x00001100,
    .chksum_legacy = 0x02eda672,
    .img_size = 0x7d00,
    .header_len = 0x0038,
    .reserved = 0x0000,
    .digest = {0x3e, 0xda, 0x9f, 0xfe, 0x55, 0x37, 0xa5, 0x88,
               0xf5, 0x4d, 0x0b, 0x2a, 0x45, 0x3e, 0x5f, 0xa9,
               0x32, 0x98, 0x6d, 0x0b, 0xc0, 0xf9, 0x55, 0x69,
               0x24, 0xf5, 0xc2, 0x37, 0x9b, 0x2c, 0x91, 0xb0},
    .chksum = 0x83dad653,
};

const riotboot_hdr_t riotboot_hdr_bad_magic = {
    .magic_number = 0x12345678,
    .version = 0x5bd19bff,
    .start_addr = 0x00001100,
    .chksum_legacy = 0x02eda672,
    .img_size = 0x7d00,
    .header_len = 0x0038,
    .reserved = 0x0000,
    .digest = {0x3e, 0xda, 0x9f, 0xfe, 0x55, 0x37, 0xa5, 0x88,
               0xf5, 0x4d, 0x0b, 0x2a, 0x45, 0x3e, 0x5f, 0xa9,
               0x32, 0x98, 0x6d, 0x0b, 0xc0, 0xf9, 0x55, 0x69,
               0x24, 0xf5, 0xc2, 0x37, 0x9b, 0x2c, 0x91, 0xb0},
    .chksum = 0x83dad653,
};

const riotboot_hdr_t riotboot_hdr_bad_chksum_legacy = {
    .magic_number = RIOTBOOT_MAGIC,
    .version = 0x5bd19bff,
    .start_addr = 0x00001100,
    .chksum_legacy = 0x02000000,
    .img_size = 0x7d00,
    .header_len = 0x0038,
    .reserved = 0x0000,
    .digest = {0x3e, 0xda, 0x9f, 0xfe, 0x55, 0x37, 0xa5, 0x88,
               0xf5, 0x4d, 0x0b, 0x2a, 0x45, 0x3e, 0x5f, 0xa9,
               0x32, 0x98, 0x6d, 0x0b, 0xc0, 0xf9, 0x55, 0x69,
               0x24, 0xf5, 0xc2, 0x37, 0x9b, 0x2c, 0x91, 0xb0},
    .chksum = 0x83dad653,
};

const riotboot_hdr_t riotboot_hdr_bad_chksum = {
    .magic_number = RIOTBOOT_MAGIC,
    .version = 0x5bd19bff,
    .start_addr = 0x00001100,
    .chksum_legacy = 0x02eda672,
    .img_size = 0x7d00,
    .header_len = 0x0038,
    .reserved = 0x0000,
    .digest = {0x3e, 0xda, 0x9f, 0xfe, 0x55, 0x37, 0xa5, 0x88,
               0xf5, 0x4d, 0x0b, 0x2a, 0x45, 0x3e, 0x5f, 0xa9,
               0x32, 0x98, 0x6d, 0x0b, 0xc0, 0xf9, 0x55, 0x69,
               0x24, 0xf5, 0xc2, 0x37, 0x9b, 0x2c, 0x91, 0xb0},
    .chksum = 0xd3b82ebc,
};

const riotboot_hdr_t riotboot_hdr_bad_hdr_len = {
    .magic_number = RIOTBOOT_MAGIC,
    .version = 0x5bd19bff,
    .start_addr = 0x00001100,
    .chksum_legacy = 0x02eda672,
    .img_size = 0x7d00,
    .header_len = 0x0040,
    .reserved = 0x0000,
    .digest = {0x3e, 0xda, 0x9f, 0xfe, 0x55, 0x37, 0xa5, 0x88,
               0xf5, 0x4d, 0x0b, 0x2a, 0x45, 0x3e, 0x5f, 0xa9,
               0x32, 0x98, 0x6d, 0x0b, 0xc0, 0xf9, 0x55, 0x69,
               0x24, 0xf5, 0xc2, 0x37, 0x9b, 0x2c, 0x91, 0xb0},
    .chksum = 0x83dad653,
};

static void test_riotboot_hdr_01(void)
{
    int ret = riotboot_hdr_validate(&riotboot_hdr_good);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_riotboot_hdr_02(void)
{
    int ret = riotboot_hdr_validate(&riotboot_hdr_bad_magic);

    TEST_ASSERT_EQUAL_INT(-1, ret);
}

static void test_riotboot_hdr_03(void)
{
    int ret = riotboot_hdr_validate(&riotboot_hdr_bad_chksum);

    TEST_ASSERT_EQUAL_INT(-1, ret);
}

static void test_riotboot_hdr_04(void)
{
    uint32_t chksum = riotboot_hdr_checksum(&riotboot_hdr_good);

    TEST_ASSERT_EQUAL_INT(riotboot_hdr_good.chksum, chksum);
}

static void test_riotboot_hdr_05(void)
{
    uint32_t chksum = riotboot_hdr_checksum_legacy(&riotboot_hdr_good);

    TEST_ASSERT_EQUAL_INT(riotboot_hdr_good.chksum_legacy, chksum);
}

static void test_riotboot_hdr_06(void)
{
    int ret = riotboot_hdr_validate(&riotboot_hdr_bad_hdr_len);

    TEST_ASSERT_EQUAL_INT(-1, ret);
}

Test *tests_riotboot_hdr(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_riotboot_hdr_01),
        new_TestFixture(test_riotboot_hdr_02),
        new_TestFixture(test_riotboot_hdr_03),
        new_TestFixture(test_riotboot_hdr_04),
        new_TestFixture(test_riotboot_hdr_05),
        new_TestFixture(test_riotboot_hdr_06),
    };

    EMB_UNIT_TESTCALLER(riotboot_hdr_tests, NULL, NULL, fixtures);

    return (Test *)&riotboot_hdr_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_riotboot_hdr());
    TESTS_END();
    return 0;
}
