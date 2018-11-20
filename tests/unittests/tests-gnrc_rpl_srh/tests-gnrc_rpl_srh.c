/*
 * Copyright (C) 2016 Cenk Gündoğan <mail@cgundogan.de>
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Cenk Gündoğan <mail@cgundogan.de>
 * @author Martine Lenders <m.lenders@fu-berlin.de>
 */
#include <stdlib.h>
#include <string.h>
#include "embUnit.h"

#include "net/ipv6/addr.h"
#include "net/ipv6/ext.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/rpl/srh.h"
#include "net/gnrc/ipv6/ext/rh.h"

#include "unittests-constants.h"
#include "tests-gnrc_rpl_srh.h"

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
#define IPV6_MCAST_ADDR     {{ 0xff, 0x05, 0xab, 0xcd, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x03 }}

#define IPV6_ADDR1_ELIDED   { 0x00, 0x00, 0x02 }
#define IPV6_ADDR2_ELIDED   { 0x00, 0x00, 0x03 }
#define IPV6_ELIDED_PREFIX  (13)

#define SRH_SEG_LEFT        (2)
#define MAX_BUF_SIZE        ((sizeof(gnrc_rpl_srh_t) + 2) + sizeof(ipv6_addr_t))

static ipv6_hdr_t hdr;
static uint8_t buf[MAX_BUF_SIZE];

static void set_up(void)
{
    memset(&hdr, 0, sizeof(hdr));
    memset(buf, 0, sizeof(buf));
}

static inline void _init_hdrs(gnrc_rpl_srh_t **srh, uint8_t **vec,
                              const ipv6_addr_t *dst)
{
    *srh = (gnrc_rpl_srh_t *)buf;
    *vec = (uint8_t *)(*srh + 1);
    memcpy(&hdr.dst, dst, sizeof(hdr.dst));
}

static void test_rpl_srh_dst_multicast(void)
{
    static const ipv6_addr_t a1 = IPV6_ADDR1, a2 = IPV6_ADDR2;
    static const ipv6_addr_t mcast = IPV6_MCAST_ADDR;
    gnrc_rpl_srh_t *srh;
    uint8_t *vec;
    int res;

    _init_hdrs(&srh, &vec, &mcast);
    srh->len = (2 * sizeof(ipv6_addr_t)) / 8;
    srh->seg_left = SRH_SEG_LEFT;
    memcpy(vec, &a1, sizeof(a1));
    memcpy(vec + sizeof(a1), &a2, sizeof(a2));

    res = gnrc_rpl_srh_process(&hdr, srh);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_ERROR);
}

static void test_rpl_srh_route_multicast(void)
{
    static const ipv6_addr_t a1 = IPV6_ADDR1;
    static const ipv6_addr_t mcast = IPV6_MCAST_ADDR;
    static const ipv6_addr_t dst = IPV6_DST;
    gnrc_rpl_srh_t *srh;
    uint8_t *vec;
    int res;

    _init_hdrs(&srh, &vec, &dst);
    srh->len = (2 * sizeof(ipv6_addr_t)) / 8;
    srh->seg_left = SRH_SEG_LEFT;
    memcpy(vec, &mcast, sizeof(mcast));
    memcpy(vec + sizeof(mcast), &a1, sizeof(a1));

    res = gnrc_rpl_srh_process(&hdr, srh);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_ERROR);
}

static void test_rpl_srh_too_many_seg_left(void)
{
    static const ipv6_addr_t a1 = IPV6_ADDR1;
    static const ipv6_addr_t dst = IPV6_DST;
    gnrc_rpl_srh_t *srh;
    uint8_t *vec;
    int res;

    _init_hdrs(&srh, &vec, &dst);
    srh->len = sizeof(ipv6_addr_t) / 8;
    srh->seg_left = SRH_SEG_LEFT;
    memcpy(vec, &a1, sizeof(a1));

    res = gnrc_rpl_srh_process(&hdr, srh);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_ERROR);
}

static void test_rpl_srh_nexthop_no_prefix_elided(void)
{
    static const ipv6_addr_t a1 = IPV6_ADDR1, a2 = IPV6_ADDR2, dst = IPV6_DST;
    static const ipv6_addr_t expected1 = IPV6_ADDR1, expected2 = IPV6_ADDR2;
    gnrc_rpl_srh_t *srh;
    uint8_t *vec;
    int res;

    _init_hdrs(&srh, &vec, &dst);
    srh->len = (2 * sizeof(ipv6_addr_t)) / 8;
    srh->seg_left = SRH_SEG_LEFT;
    memcpy(vec, &a1, sizeof(a1));
    memcpy(vec + sizeof(a1), &a2, sizeof(a2));

    /* first hop */
    res = gnrc_rpl_srh_process(&hdr, srh);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_FORWARDED);
    TEST_ASSERT_EQUAL_INT(SRH_SEG_LEFT - 1, srh->seg_left);
    TEST_ASSERT(ipv6_addr_equal(&hdr.dst, &expected1));

    /* second hop */
    res = gnrc_rpl_srh_process(&hdr, srh);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_FORWARDED);
    TEST_ASSERT_EQUAL_INT(SRH_SEG_LEFT - 2, srh->seg_left);
    TEST_ASSERT(ipv6_addr_equal(&hdr.dst, &expected2));
}

static void test_rpl_srh_nexthop_prefix_elided(void)
{
    static const ipv6_addr_t dst = IPV6_DST;
    static const ipv6_addr_t expected1 = IPV6_ADDR1, expected2 = IPV6_ADDR2;
    gnrc_rpl_srh_t *srh;
    uint8_t *vec;
    int res;
    static const uint8_t a1[3] = IPV6_ADDR1_ELIDED;
    static const uint8_t a2[3] = IPV6_ADDR2_ELIDED;

    _init_hdrs(&srh, &vec, &dst);
    srh->len = (sizeof(a1) + sizeof(a2) + 2) / 8;
    srh->seg_left = SRH_SEG_LEFT;
    srh->compr = (IPV6_ELIDED_PREFIX << 4) | IPV6_ELIDED_PREFIX;
    srh->pad_resv = 2 << 4;
    memcpy(vec, &a1, sizeof(a1));
    memcpy(vec + sizeof(a1), &a2, sizeof(a2));

    /* first hop */
    res = gnrc_rpl_srh_process(&hdr, srh);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_FORWARDED);
    TEST_ASSERT_EQUAL_INT(SRH_SEG_LEFT - 1, srh->seg_left);
    TEST_ASSERT(ipv6_addr_equal(&hdr.dst, &expected1));

    /* second hop */
    res = gnrc_rpl_srh_process(&hdr, srh);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_FORWARDED);
    TEST_ASSERT_EQUAL_INT(SRH_SEG_LEFT - 2, srh->seg_left);
    TEST_ASSERT(ipv6_addr_equal(&hdr.dst, &expected2));
}

static Test *tests_rpl_srh_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_rpl_srh_dst_multicast),
        new_TestFixture(test_rpl_srh_route_multicast),
        new_TestFixture(test_rpl_srh_too_many_seg_left),
        new_TestFixture(test_rpl_srh_nexthop_no_prefix_elided),
        new_TestFixture(test_rpl_srh_nexthop_prefix_elided),
    };

    EMB_UNIT_TESTCALLER(rpl_srh_tests, set_up, NULL, fixtures);

    return (Test *)&rpl_srh_tests;
}

void tests_gnrc_rpl_srh(void)
{
    TESTS_RUN(tests_rpl_srh_tests());
}
/** @} */
