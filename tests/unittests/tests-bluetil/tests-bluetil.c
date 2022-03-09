/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "embUnit.h"
#include "tests-bluetil.h"

#include "net/bluetil/addr.h"

#define ENABLE_DEBUG 0
#include "debug.h"

const uint8_t addr[][6] = {
    { 0xfe, 0xaf, 0xfe, 0xaf, 0xfe, 0xaf },
    { 0x03, 0x02, 0x01, 0xef, 0xcd, 0xab },
    { 255, 255, 255, 255, 255, 255 },
    { 0, 0, 0, 0, 0, 0 },
};

static void test_bluetil_addr(void)
{
    char astr[BLUETIL_ADDR_STRLEN];
    char istr[BLUETIL_IPV6_IID_STRLEN];

    bluetil_addr_sprint(astr, addr[0]);
    TEST_ASSERT_EQUAL_STRING("FE:AF:FE:AF:FE:AF", astr);
    bluetil_addr_ipv6_l2ll_sprint(istr, addr[0]);
    TEST_ASSERT_EQUAL_STRING("[FE80::FEAF:FEFF:FEAF:FEAF]", istr);

    bluetil_addr_sprint(astr, addr[1]);
    TEST_ASSERT_EQUAL_STRING("03:02:01:EF:CD:AB", astr);
    bluetil_addr_ipv6_l2ll_sprint(istr, addr[1]);
    TEST_ASSERT_EQUAL_STRING("[FE80::0302:01FF:FEEF:CDAB]", istr);

    bluetil_addr_sprint(astr, addr[2]);
    TEST_ASSERT_EQUAL_STRING("FF:FF:FF:FF:FF:FF", astr);
    bluetil_addr_ipv6_l2ll_sprint(istr, addr[2]);
    TEST_ASSERT_EQUAL_STRING("[FE80::FFFF:FFFF:FEFF:FFFF]", istr);

    bluetil_addr_sprint(astr, addr[3]);
    TEST_ASSERT_EQUAL_STRING("00:00:00:00:00:00", astr);
    bluetil_addr_ipv6_l2ll_sprint(istr, addr[3]);
    TEST_ASSERT_EQUAL_STRING("[FE80::0000:00FF:FE00:0000]", istr);
}

Test *tests_bluetil_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_bluetil_addr),
    };

    EMB_UNIT_TESTCALLER(bluetil_tests, NULL, NULL, fixtures);

    return (Test *)&bluetil_tests;
}

void tests_bluetil(void)
{
    TESTS_RUN(tests_bluetil_tests());
}
