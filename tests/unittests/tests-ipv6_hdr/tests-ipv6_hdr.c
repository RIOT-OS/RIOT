/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
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

#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/hdr.h"
#include "net/ng_pktbuf.h"
#include "net/ng_protnum.h"
#include "net/ng_inet_csum.h"

#include "unittests-constants.h"
#include "tests-ipv6_hdr.h"

#define OTHER_BYTE  (TEST_UINT16 >> 8)
#define DEFAULT_TEST_SRC    { { \
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f \
        } \
    }
#define DEFAULT_TEST_DST    { { \
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, \
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f \
        } \
    }

static void test_ipv6_hdr_set_version(void)
{
    uint8_t val[] = { TEST_UINT8 };

    ng_ipv6_hdr_set_version((ng_ipv6_hdr_t *)val);

    /*
     * Header format:
     *  1
     * +----+--
     * |  6 |
     * +----+--
     */
    TEST_ASSERT_EQUAL_INT(0x60, val[0] & 0xf0);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8 & 0x0f, val[0] & 0x0f);
}

static void test_ipv6_hdr_get_version(void)
{
    uint8_t val[] = { TEST_UINT8 };

    /*
     * Header format:
     *          8
     * +----+----
     * |  6 |
     * +----+----
     */
    TEST_ASSERT_EQUAL_INT(TEST_UINT8 >> 4,
                          ng_ipv6_hdr_get_version((ng_ipv6_hdr_t *)val));
}

static void test_ipv6_hdr_is_ipv6_hdr__false(void)
{
    /*
     * Header format:
     *          8
     * +----+----
     * |  6 |
     * +----+----
     */
    uint8_t val[] = { 0 };

    TEST_ASSERT(!ng_ipv6_hdr_is((ng_ipv6_hdr_t *)val));
}

static void test_ipv6_hdr_is_ipv6_hdr__true(void)
{
    /*
     * Header format:
     *          8
     * +----+----
     * |  6 |
     * +----+----
     */
    uint8_t val[] = { 0x60 | (TEST_UINT8 & 0x0f) };

    TEST_ASSERT(ng_ipv6_hdr_is((ng_ipv6_hdr_t *)val));
}

static void test_ipv6_hdr_set_tc(void)
{
    uint8_t val[] = { TEST_UINT8, 0 };

    ng_ipv6_hdr_set_tc((ng_ipv6_hdr_t *)val, OTHER_BYTE);

    /*
     * Header format:
     *          8
     * +----+--------+--
     * |  6 |   tc   |
     * +----+--------+--
     */
    TEST_ASSERT_EQUAL_INT((TEST_UINT8 & 0xf0) | (OTHER_BYTE >> 4), val[0]);
    TEST_ASSERT_EQUAL_INT((OTHER_BYTE << 4) & 0xf0, val[1]);
}

static void test_ipv6_hdr_set_tc_ecn(void)
{
    uint8_t val[] = { TEST_UINT8 };

    ng_ipv6_hdr_set_tc_ecn((ng_ipv6_hdr_t *)val, OTHER_BYTE);

    /*
     * Header format:
     *          8
     * +----+--------+--
     * |  6 |   tc   |
     * +----+--------+--
     *     /          \
     *          8       12
     *  +----+------------+
     *  | ecn|    dscp    |
     *  +----+------------+
     */
    TEST_ASSERT_EQUAL_INT((TEST_UINT8 & 0xf3) | ((OTHER_BYTE & 0x03) << 2), val[0]);
}

static void test_ipv6_hdr_set_tc_dscp(void)
{
    uint8_t val[] = { TEST_UINT8, 0 };

    ng_ipv6_hdr_set_tc_dscp((ng_ipv6_hdr_t *)val, OTHER_BYTE);

    /*
     * Header format:
     *          8
     * +----+--------+--
     * |  6 |   tc   |
     * +----+--------+--
     *     /          \
     *          8       12
     *  +----+------------+
     *  | ecn|    dscp    |
     *  +----+------------+
     */
    TEST_ASSERT_EQUAL_INT((TEST_UINT8 & 0xfc) | ((OTHER_BYTE & 0x30) >> 4), val[0]);
    TEST_ASSERT_EQUAL_INT((OTHER_BYTE & 0x0f) << 4, val[1]);
}

static void test_ipv6_hdr_get_tc(void)
{
    uint8_t val[] = { TEST_UINT8, OTHER_BYTE };

    /*
     * Header format:
     *          8
     * +----+--------+--
     * |  6 |   tc   |
     * +----+--------+--
     */
    TEST_ASSERT_EQUAL_INT(((TEST_UINT8 << 4) & 0xf0) | (OTHER_BYTE >> 4),
                          ng_ipv6_hdr_get_tc((ng_ipv6_hdr_t *)val));
}

static void test_ipv6_hdr_get_tc_ecn(void)
{
    uint8_t val[] = { TEST_UINT8 };

    /*
     * Header format:
     *          8
     * +----+--------+--
     * |  6 |   tc   |
     * +----+--------+--
     *     /          \
     *          8       12
     *  +----+------------+
     *  | ecn|    dscp    |
     *  +----+------------+
     */
    TEST_ASSERT_EQUAL_INT(TEST_UINT8 & 0x03,
                          ng_ipv6_hdr_get_tc_ecn((ng_ipv6_hdr_t *)val));
}

static void test_ipv6_hdr_get_tc_dscp(void)
{
    uint8_t val[] = { TEST_UINT8, OTHER_BYTE };

    /*
     * Header format:
     *          8
     * +----+--------+--
     * |  6 |   tc   |
     * +----+--------+--
     *     /          \
     *          8       12
     *  +----+------------+
     *  | ecn|    dscp    |
     *  +----+------------+
     */
    TEST_ASSERT_EQUAL_INT(((TEST_UINT8 & 0x03) << 4) | ((OTHER_BYTE & 0xf0) >> 4),
                          ng_ipv6_hdr_get_tc_dscp((ng_ipv6_hdr_t *)val));
}

static void test_ipv6_hdr_set_fl(void)
{
    uint8_t val[] = { 0, TEST_UINT8, 0, 0 };

    ng_ipv6_hdr_set_fl((ng_ipv6_hdr_t *)val, TEST_UINT32);

    /*
     * Header format:
     *          8       16      24      32
     * +----+--------+--------------------+
     * |  6 |   tc   |     flow label     |
     * +----+--------+--------------------+
     */
    TEST_ASSERT_EQUAL_INT(0, val[0]);
    TEST_ASSERT_EQUAL_INT((TEST_UINT8 & 0xf0) | ((TEST_UINT32 & 0x000f0000) >> 16),
                          val[1]);
    TEST_ASSERT_EQUAL_INT((TEST_UINT32 & 0x0000ff00) >> 8,
                          val[2]);
    TEST_ASSERT_EQUAL_INT((TEST_UINT32 & 0x000000ff), val[3]);
}

static void test_ipv6_hdr_get_fl(void)
{
    uint8_t val[] = { TEST_UINT8, OTHER_BYTE, 0, 0 };

    /*
     * Header format:
     *          8       16      24      32
     * +----+--------+--------------------+
     * |  6 |   tc   |     flow label     |
     * +----+--------+--------------------+
     */
    TEST_ASSERT_EQUAL_INT((OTHER_BYTE & 0x0f) << 16,
                          ng_ipv6_hdr_get_fl((ng_ipv6_hdr_t *)val));
}

static void test_ipv6_hdr_inet_csum__initial_sum_overflows(void)
{
    uint16_t sum = 0xffff;
    uint16_t res = 0, payload_len = 0;
    uint8_t val[] = {
        0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x3a, 0x40, /* IPv6 header */
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xc8, 0x86, 0xcd, 0xff, 0xfe, 0x0f, 0xce, 0x49,
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x18, 0xaa, 0x2d, 0xff, 0xfe, 0x44, 0x43, 0xac
    };

    /* calculate checksum of pseudo header */
    res = ng_ipv6_hdr_inet_csum(sum, (ng_ipv6_hdr_t *)&val, NG_PROTNUM_ICMPV6,
                                payload_len);
    res = ~res;     /* take 1's-complement for correct checksum */

    TEST_ASSERT_EQUAL_INT(0x1749, res);
}

static void test_ipv6_hdr_inet_csum__initial_sum_0(void)
{
    /* source: https://www.cloudshark.org/captures/ea72fbab241b (No. 56) */
    uint16_t res = 0, payload_len;
    uint8_t val[] = {
        0x60, 0x00, 0x00, 0x00, 0x00, 0x38, 0x3a, 0xff, /* IPv6 header */
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x5a, 0x6d, 0x8f, 0xff, 0xfe, 0x56, 0x30, 0x09,
        0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x86, 0x00, 0x00, 0x00, 0x40, 0x58, 0x07, 0x08, /* ICMPv6 payload */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* set checksum to 0 */
        0x03, 0x04, 0x40, 0xc0, 0x00, 0x00, 0x00, 0x1e,
        0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00,
        0x20, 0x02, 0x18, 0x3d, 0xdb, 0xa4, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x58, 0x6d, 0x8f, 0x56, 0x30, 0x09
    };

    payload_len = sizeof(val) - sizeof(ng_ipv6_hdr_t);

    /* calculate checksum of pseudo header */
    res = ng_ipv6_hdr_inet_csum(0, (ng_ipv6_hdr_t *)&val, NG_PROTNUM_ICMPV6,
                                payload_len);
    /* calculate checksum of payload */
    res = ng_inet_csum(res, val + sizeof(ng_ipv6_hdr_t), payload_len);
    res = ~res;     /* take 1's-complement for correct checksum */

    TEST_ASSERT_EQUAL_INT(0xab32, res);
}

static void test_ipv6_hdr_build__wrong_src_len(void)
{
    ng_ipv6_addr_t src = DEFAULT_TEST_SRC;
    ng_ipv6_addr_t dst = DEFAULT_TEST_DST;

    TEST_ASSERT_NULL(ng_ipv6_hdr_build(NULL, (uint8_t *)&src,
                                       sizeof(ng_ipv6_addr_t) + TEST_UINT8,
                                       (uint8_t *)&dst,
                                       sizeof(ng_ipv6_addr_t)));
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_ipv6_hdr_build__wrong_dst_len(void)
{
    ng_ipv6_addr_t src = DEFAULT_TEST_SRC;
    ng_ipv6_addr_t dst = DEFAULT_TEST_DST;

    TEST_ASSERT_NULL(ng_ipv6_hdr_build(NULL, (uint8_t *)&src,
                                       sizeof(ng_ipv6_addr_t),
                                       (uint8_t *)&dst,
                                       sizeof(ng_ipv6_addr_t) + TEST_UINT8));
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_ipv6_hdr_build__src_NULL(void)
{
    ng_ipv6_addr_t dst = DEFAULT_TEST_DST;
    ng_pktsnip_t *pkt;
    ng_ipv6_hdr_t *hdr;

    TEST_ASSERT(ng_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL((pkt = ng_ipv6_hdr_build(NULL, NULL, 0, (uint8_t *)&dst,
                                sizeof(ng_ipv6_addr_t))));
    hdr = pkt->data;
    TEST_ASSERT_NOT_NULL(hdr);
    TEST_ASSERT(ng_ipv6_hdr_is(hdr));
    TEST_ASSERT_EQUAL_INT(0, ng_ipv6_hdr_get_tc(hdr));
    TEST_ASSERT_EQUAL_INT(0, ng_ipv6_hdr_get_fl(hdr));
    TEST_ASSERT_EQUAL_INT(NG_PROTNUM_RESERVED, hdr->nh);
    TEST_ASSERT_EQUAL_INT(0, hdr->hl);
    TEST_ASSERT(ng_ipv6_addr_equal(&dst, &hdr->dst));
    TEST_ASSERT(!ng_pktbuf_is_empty());
    ng_pktbuf_reset();
}

static void test_ipv6_hdr_build__dst_NULL(void)
{
    ng_ipv6_addr_t src = DEFAULT_TEST_SRC;
    ng_pktsnip_t *pkt;
    ng_ipv6_hdr_t *hdr;

    TEST_ASSERT(ng_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL((pkt = ng_ipv6_hdr_build(NULL, (uint8_t *)&src,
                                sizeof(ng_ipv6_addr_t),
                                NULL, 0)));
    hdr = pkt->data;
    TEST_ASSERT_NOT_NULL(hdr);
    TEST_ASSERT(ng_ipv6_hdr_is(hdr));
    TEST_ASSERT_EQUAL_INT(0, ng_ipv6_hdr_get_tc(hdr));
    TEST_ASSERT_EQUAL_INT(0, ng_ipv6_hdr_get_fl(hdr));
    TEST_ASSERT_EQUAL_INT(NG_PROTNUM_RESERVED, hdr->nh);
    TEST_ASSERT_EQUAL_INT(0, hdr->hl);
    TEST_ASSERT(ng_ipv6_addr_equal(&src, &hdr->src));
    TEST_ASSERT(!ng_pktbuf_is_empty());
    ng_pktbuf_reset();
}

static void test_ipv6_hdr_build__complete(void)
{
    ng_ipv6_addr_t src = DEFAULT_TEST_SRC;
    ng_ipv6_addr_t dst = DEFAULT_TEST_DST;
    ng_pktsnip_t *pkt;
    ng_ipv6_hdr_t *hdr;

    TEST_ASSERT(ng_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL((pkt = ng_ipv6_hdr_build(NULL, (uint8_t *)&src,
                                sizeof(ng_ipv6_addr_t),
                                (uint8_t *)&dst,
                                sizeof(ng_ipv6_addr_t))));
    hdr = pkt->data;
    TEST_ASSERT_NOT_NULL(hdr);
    TEST_ASSERT(ng_ipv6_hdr_is(hdr));
    TEST_ASSERT_EQUAL_INT(0, ng_ipv6_hdr_get_tc(hdr));
    TEST_ASSERT_EQUAL_INT(0, ng_ipv6_hdr_get_fl(hdr));
    TEST_ASSERT_EQUAL_INT(NG_PROTNUM_RESERVED, hdr->nh);
    TEST_ASSERT_EQUAL_INT(0, hdr->hl);
    TEST_ASSERT(ng_ipv6_addr_equal(&src, &hdr->src));
    TEST_ASSERT(ng_ipv6_addr_equal(&dst, &hdr->dst));
    TEST_ASSERT(!ng_pktbuf_is_empty());
    ng_pktbuf_reset();
}

Test *tests_ipv6_hdr_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ipv6_hdr_set_version),
        new_TestFixture(test_ipv6_hdr_get_version),
        new_TestFixture(test_ipv6_hdr_is_ipv6_hdr__false),
        new_TestFixture(test_ipv6_hdr_is_ipv6_hdr__true),
        new_TestFixture(test_ipv6_hdr_set_tc),
        new_TestFixture(test_ipv6_hdr_set_tc_ecn),
        new_TestFixture(test_ipv6_hdr_set_tc_dscp),
        new_TestFixture(test_ipv6_hdr_get_tc),
        new_TestFixture(test_ipv6_hdr_get_tc_ecn),
        new_TestFixture(test_ipv6_hdr_get_tc_dscp),
        new_TestFixture(test_ipv6_hdr_set_fl),
        new_TestFixture(test_ipv6_hdr_get_fl),
        new_TestFixture(test_ipv6_hdr_inet_csum__initial_sum_overflows),
        new_TestFixture(test_ipv6_hdr_inet_csum__initial_sum_0),
        new_TestFixture(test_ipv6_hdr_build__wrong_src_len),
        new_TestFixture(test_ipv6_hdr_build__wrong_dst_len),
        new_TestFixture(test_ipv6_hdr_build__src_NULL),
        new_TestFixture(test_ipv6_hdr_build__dst_NULL),
        new_TestFixture(test_ipv6_hdr_build__complete),
    };

    EMB_UNIT_TESTCALLER(ipv6_hdr_tests, NULL, NULL, fixtures);

    return (Test *)&ipv6_hdr_tests;
}

void tests_ipv6_hdr(void)
{
    TESTS_RUN(tests_ipv6_hdr_tests());
}
/** @} */
