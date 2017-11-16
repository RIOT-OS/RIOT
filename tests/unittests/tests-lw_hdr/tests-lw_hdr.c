/*
 * Copyright (C) 2016 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdlib.h>

#include "embUnit.h"

#include "unittests-constants.h"
#include "tests-lw_hdr.h"
#include "net/lorawan/hdr.h"

#define OTHER_BYTE  (TEST_UINT16 >> 8)

static void test_lw_hdr_set_mtype(void)
{
    uint8_t val[] = { TEST_UINT8 };

    lw_hdr_set_mtype((lw_hdr_t *)val, OTHER_BYTE & 0x07);

    TEST_ASSERT_EQUAL_INT((OTHER_BYTE << 5) & 0xe0, (val[0] & 0xe0));
    TEST_ASSERT_EQUAL_INT(TEST_UINT8 & 0x1f, (val[0] & 0x1f));
}

static void test_lw_hdr_get_mtype(void)
{
    uint8_t val[] = { TEST_UINT8 };

    TEST_ASSERT_EQUAL_INT((TEST_UINT8 & 0xe0) >> 5, lw_hdr_get_mtype((lw_hdr_t *)val));
}

static void test_lw_hdr_set_maj(void)
{
    uint8_t val[] = { TEST_UINT8 };

    lw_hdr_set_maj((lw_hdr_t *)val, OTHER_BYTE & 0x03);

    TEST_ASSERT_EQUAL_INT(OTHER_BYTE & 0x03, (val[0] & 0x03));
    TEST_ASSERT_EQUAL_INT(TEST_UINT8 & 0xfc, (val[0] & 0xfc));
}

static void test_lw_hdr_get_maj(void)
{
    uint8_t val[] = { TEST_UINT8 };

    TEST_ASSERT_EQUAL_INT((TEST_UINT8 & 0x03), lw_hdr_get_maj((lw_hdr_t *)val));
}

Test *tests_lw_hdr_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_lw_hdr_set_mtype),
        new_TestFixture(test_lw_hdr_get_mtype),
        new_TestFixture(test_lw_hdr_set_maj),
        new_TestFixture(test_lw_hdr_get_maj),
    };

    EMB_UNIT_TESTCALLER(lw_hdr_tests, NULL, NULL, fixtures);

    return (Test *)&lw_hdr_tests;
}

void tests_lw_hdr(void)
{
    TESTS_RUN(tests_lw_hdr_tests());
}
/** @} */
