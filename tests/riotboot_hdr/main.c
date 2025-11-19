/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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
    .chksum = 0x02eda672
};

const riotboot_hdr_t riotboot_hdr_bad_magic = {
    .magic_number = 0x12345678,
    .version = 0x5bd19bff,
    .start_addr = 0x00001100,
    .chksum = 0x02eda672
};

const riotboot_hdr_t riotboot_hdr_bad_chksum = {
    .magic_number = RIOTBOOT_MAGIC,
    .version = 0x5bd19bff,
    .start_addr = 0x00001100,
    .chksum = 0x02000000
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

    TEST_ASSERT_EQUAL_INT(0x02eda672, chksum);
}

Test *tests_riotboot_hdr(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_riotboot_hdr_01),
        new_TestFixture(test_riotboot_hdr_02),
        new_TestFixture(test_riotboot_hdr_03),
        new_TestFixture(test_riotboot_hdr_04),
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
