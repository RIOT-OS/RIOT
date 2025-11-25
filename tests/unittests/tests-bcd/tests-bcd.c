/*
 * Copyright (C) 2017 Freie Universität Berlin
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

Test *tests_bcd_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_bcd_from_byte__zero),
        new_TestFixture(test_bcd_from_byte__greater_99),
        new_TestFixture(test_bcd_from_byte),
        new_TestFixture(test_bcd_to_byte__zero),
        new_TestFixture(test_bcd_to_byte__greater_0x99),
        new_TestFixture(test_bcd_to_byte),
    };

    EMB_UNIT_TESTCALLER(bcd_tests, NULL, NULL, fixtures);

    return (Test *)&bcd_tests;
}

void tests_bcd(void)
{
    TESTS_RUN(tests_bcd_tests());
}
