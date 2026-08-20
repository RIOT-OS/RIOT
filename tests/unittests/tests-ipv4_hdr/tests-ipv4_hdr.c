/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */
#include <stdint.h>

#include "embUnit.h"

#include "net/ipv4/hdr.h"

#include "unittests-constants.h"
#include "tests-ipv4_hdr.h"

static void test_ipv4_hdr_set_version(void)
{
    uint8_t val[sizeof(ipv4_hdr_t)] = { TEST_UINT8 };

    ipv4_hdr_set_version((ipv4_hdr_t *)val);

    TEST_ASSERT_EQUAL_INT(0x40, val[0] & 0xf0);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8 & 0x0f, val[0] & 0x0f);
}

static void test_ipv4_hdr_get_version(void)
{
    uint8_t val[sizeof(ipv4_hdr_t)] = { TEST_UINT8 };

    TEST_ASSERT_EQUAL_INT(TEST_UINT8 >> 4, ipv4_hdr_get_version((ipv4_hdr_t *)val));
}

static void test_ipv4_hdr_set_ihl(void)
{
    uint8_t val[sizeof(ipv4_hdr_t)] = { TEST_UINT8 };

    /* minimum internet header length */
    ipv4_hdr_set_ihl((ipv4_hdr_t *)val, 20);

    TEST_ASSERT_EQUAL_INT(TEST_UINT8 & 0xf0, val[0] & 0xf0);
    TEST_ASSERT_EQUAL_INT(5, val[0] & 0x0f);

    /* maximum internet header length */
    ipv4_hdr_set_ihl((ipv4_hdr_t *)val, 60);

    TEST_ASSERT_EQUAL_INT(TEST_UINT8 & 0xf0, val[0] & 0xf0);
    TEST_ASSERT_EQUAL_INT(15, val[0] & 0x0f);
}

static void test_ipv4_hdr_get_ihl(void)
{
    uint8_t val[sizeof(ipv4_hdr_t)];

    /* minimum internet header length */
    val[0] = 0x45;

    TEST_ASSERT_EQUAL_INT(20, ipv4_hdr_get_ihl((ipv4_hdr_t *)val));

    /* maximum internet header length */
    val[0] = 0x4f;

    TEST_ASSERT_EQUAL_INT(60, ipv4_hdr_get_ihl((ipv4_hdr_t *)val));
}

static void test_ipv4_hdr_set_flags(void)
{
    uint8_t val[sizeof(ipv4_hdr_t)] = { 0 };

    val[6] = TEST_UINT8;

    /* no flags set */
    ipv4_hdr_set_flags((ipv4_hdr_t *)val, 0);

    TEST_ASSERT_EQUAL_INT(0x00, val[6] & 0xe0);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8 & 0x1f, val[6] & 0x1f);

    /* all flags set */
    ipv4_hdr_set_flags((ipv4_hdr_t *)val, 0x07);

    TEST_ASSERT_EQUAL_INT(0xe0, val[6] & 0xe0);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8 & 0x1f, val[6] & 0x1f);
}

static void test_ipv4_hdr_get_flags(void)
{
    uint8_t val[sizeof(ipv4_hdr_t)] = { 0 };

    /* no flags set */
    val[6] = 0x00;

    TEST_ASSERT_EQUAL_INT(0, ipv4_hdr_get_flags((ipv4_hdr_t *)val));

    /* all flags set */
    val[6] = 0xe0;

    TEST_ASSERT_EQUAL_INT(0x07, ipv4_hdr_get_flags((ipv4_hdr_t *)val));

    /* mixed: only MSB of flags set */
    val[6] = 0x80;

    TEST_ASSERT_EQUAL_INT(0x04, ipv4_hdr_get_flags((ipv4_hdr_t *)val));
}

static void test_ipv4_hdr_set_fo(void)
{
    uint8_t val[sizeof(ipv4_hdr_t)] = { 0 };

    val[6] = TEST_UINT8;

    /* zero fragment offset */
    ipv4_hdr_set_fo((ipv4_hdr_t *)val, 0);

    TEST_ASSERT_EQUAL_INT(TEST_UINT8 & 0xe0, val[6] & 0xe0);
    TEST_ASSERT_EQUAL_INT(0x00, val[6] & 0x1f);
    TEST_ASSERT_EQUAL_INT(0x00, val[7]);

    /* maximum fragment offset (8191) */
    ipv4_hdr_set_fo((ipv4_hdr_t *)val, 0x1fff);

    TEST_ASSERT_EQUAL_INT(TEST_UINT8 & 0xe0, val[6] & 0xe0);
    TEST_ASSERT_EQUAL_INT(0x1f, val[6] & 0x1f);
    TEST_ASSERT_EQUAL_INT(0xff, val[7]);
}

static void test_ipv4_hdr_get_fo(void)
{
    uint8_t val[sizeof(ipv4_hdr_t)] = { 0 };

    /* zero fragment offset */
    val[6] = 0x00;
    val[7] = 0x00;

    TEST_ASSERT_EQUAL_INT(0, ipv4_hdr_get_fo((ipv4_hdr_t *)val));

    /* maximum fragment offset (8191) */
    val[6] = 0x1f;
    val[7] = 0xff;

    TEST_ASSERT_EQUAL_INT(0x1fff, ipv4_hdr_get_fo((ipv4_hdr_t *)val));

    /* value spanning both bytes */
    val[6] = 0x0a;
    val[7] = 0xbc;

    TEST_ASSERT_EQUAL_INT(0x0abc, ipv4_hdr_get_fo((ipv4_hdr_t *)val));
}

Test *tests_ipv4_hdr_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ipv4_hdr_set_version),
        new_TestFixture(test_ipv4_hdr_get_version),
        new_TestFixture(test_ipv4_hdr_set_ihl),
        new_TestFixture(test_ipv4_hdr_get_ihl),
        new_TestFixture(test_ipv4_hdr_set_flags),
        new_TestFixture(test_ipv4_hdr_get_flags),
        new_TestFixture(test_ipv4_hdr_set_fo),
        new_TestFixture(test_ipv4_hdr_get_fo),
    };

    EMB_UNIT_TESTCALLER(ipv4_hdr_tests, NULL, NULL, fixtures);

    return (Test *)&ipv4_hdr_tests;
}

void tests_ipv4_hdr(void)
{
    TESTS_RUN(tests_ipv4_hdr_tests());
}
/** @} */
