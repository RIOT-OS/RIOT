/*
 * Copyright (C) 2016 Cenk Gündoğan <mail@cgundogan.de>
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
#include <stdlib.h>
#include <string.h>
#include "embUnit.h"

#include "net/ipv6/addr.h"
#include "net/ipv6/ext.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/rpl/srh.h"

#include "unittests-constants.h"
#include "tests-rpl_srh.h"

#define IPV6_DST            {{ 0x20, 0x01, 0xab, 0xcd, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x01 }}
#define IPV6_ADDR1          {{ 0x20, 0x01, 0xab, 0xcd, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x02 }}
#define IPV6_ADDR2          {{ 0x20, 0x01, 0xab, 0xcd, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x03 }}

#define IPV6_ADDR1_ELIDED   { 0x00, 0x00, 0x02 }
#define IPV6_ADDR2_ELIDED   { 0x00, 0x00, 0x03 }
#define IPV6_ELIDED_PREFIX  (13)

#define SRH_SEG_LEFT        (2)

static void test_rpl_srh_nexthop_no_prefix_elided(void)
{
    ipv6_hdr_t hdr;
    uint8_t buf[sizeof(gnrc_rpl_srh_t) + 2 * sizeof(ipv6_addr_t)] = { 0 };
    int res;
    gnrc_rpl_srh_t *srh = (gnrc_rpl_srh_t *) buf;
    uint8_t *vec = (uint8_t *) (srh + 1);
    ipv6_addr_t a1 = IPV6_ADDR1, a2 = IPV6_ADDR2, dst = IPV6_DST,
                     expected1 = IPV6_ADDR1, expected2 = IPV6_ADDR2;

    hdr.dst = dst;

    srh->len = (2 * sizeof(ipv6_addr_t)) / 8;
    srh->seg_left = SRH_SEG_LEFT;
    memcpy(vec, &a1, sizeof(a1));
    memcpy(vec + sizeof(a1), &a2, sizeof(a2));

    /* first hop */
    res = gnrc_rpl_srh_process(&hdr, srh);
    TEST_ASSERT_EQUAL_INT(res, EXT_RH_CODE_FORWARD);
    TEST_ASSERT_EQUAL_INT(SRH_SEG_LEFT - 1, srh->seg_left);
    TEST_ASSERT(ipv6_addr_equal(&hdr.dst, &expected1));

    /* second hop */
    res = gnrc_rpl_srh_process(&hdr, srh);
    TEST_ASSERT_EQUAL_INT(res, EXT_RH_CODE_FORWARD);
    TEST_ASSERT_EQUAL_INT(SRH_SEG_LEFT - 2, srh->seg_left);
    TEST_ASSERT(ipv6_addr_equal(&hdr.dst, &expected2));
}

static void test_rpl_srh_nexthop_prefix_elided(void)
{
    ipv6_hdr_t hdr;
    uint8_t a1[3] = IPV6_ADDR1_ELIDED;
    uint8_t a2[3] = IPV6_ADDR2_ELIDED;
    uint8_t buf[sizeof(gnrc_rpl_srh_t) + sizeof(a1) + sizeof(a2)] = { 0 };
    int res;
    gnrc_rpl_srh_t *srh = (gnrc_rpl_srh_t *) buf;
    uint8_t *vec = (uint8_t *) (srh + 1);
    ipv6_addr_t dst = IPV6_DST, expected1 = IPV6_ADDR1, expected2 = IPV6_ADDR2;

    hdr.dst = dst;

    srh->len = (sizeof(a1) + sizeof(a2) + 2) / 8;
    srh->seg_left = SRH_SEG_LEFT;
    srh->compr = (IPV6_ELIDED_PREFIX << 4) | IPV6_ELIDED_PREFIX;
    srh->pad_resv = 2 << 4;
    memcpy(vec, &a1, sizeof(a1));
    memcpy(vec + sizeof(a1), &a2, sizeof(a2));

    /* first hop */
    res = gnrc_rpl_srh_process(&hdr, srh);
    TEST_ASSERT_EQUAL_INT(res, EXT_RH_CODE_FORWARD);
    TEST_ASSERT_EQUAL_INT(SRH_SEG_LEFT - 1, srh->seg_left);
    TEST_ASSERT(ipv6_addr_equal(&hdr.dst, &expected1));

    /* second hop */
    res = gnrc_rpl_srh_process(&hdr, srh);
    TEST_ASSERT_EQUAL_INT(res, EXT_RH_CODE_FORWARD);
    TEST_ASSERT_EQUAL_INT(SRH_SEG_LEFT - 2, srh->seg_left);
    TEST_ASSERT(ipv6_addr_equal(&hdr.dst, &expected2));
}

Test *tests_rpl_srh_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_rpl_srh_nexthop_no_prefix_elided),
        new_TestFixture(test_rpl_srh_nexthop_prefix_elided),
    };

    EMB_UNIT_TESTCALLER(rpl_srh_tests, NULL, NULL, fixtures);

    return (Test *)&rpl_srh_tests;
}

void tests_rpl_srh(void)
{
    TESTS_RUN(tests_rpl_srh_tests());
}
/** @} */
