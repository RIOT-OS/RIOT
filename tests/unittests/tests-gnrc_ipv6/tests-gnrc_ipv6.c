/*
 * Copyright (C) 2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @file
 */

#include "embUnit.h"

#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/pktbuf.h"

#include "unittests-constants.h"
#include "test_utils/expect.h"
#include "tests-gnrc_ipv6.h"

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

#define DEFAULT_TEST_V_TC_FL    TEST_UINT32
#define DEFAULT_TEST_LEN        TEST_UINT16
#define DEFAULT_TEST_NH         TEST_UINT8
#define DEFAULT_TEST_HL         TEST_UINT8

static gnrc_pktsnip_t *_pkt_w_ip_hdr, *_pkt_no_ip_hdr;

static void set_up(void)
{
    ipv6_hdr_t ip = (ipv6_hdr_t) {
        .v_tc_fl    = byteorder_htonl(DEFAULT_TEST_V_TC_FL),
        .len        = byteorder_htons(DEFAULT_TEST_LEN),
        .nh         = DEFAULT_TEST_NH,
        .hl         = DEFAULT_TEST_HL,
        .src        = DEFAULT_TEST_SRC,
        .dst        = DEFAULT_TEST_DST
    };

    ipv6_hdr_set_version(&ip);

    gnrc_pktbuf_init();
    _pkt_w_ip_hdr = gnrc_pktbuf_add(NULL, NULL, 1, GNRC_NETTYPE_NETIF);
    expect(_pkt_w_ip_hdr);
    _pkt_w_ip_hdr = gnrc_pktbuf_add(_pkt_w_ip_hdr, &ip, sizeof(ipv6_hdr_t), GNRC_NETTYPE_IPV6);
    expect(_pkt_w_ip_hdr);
    _pkt_w_ip_hdr = gnrc_pktbuf_add(_pkt_w_ip_hdr, NULL, 1, GNRC_NETTYPE_UNDEF);
    expect(_pkt_w_ip_hdr);

    _pkt_no_ip_hdr = gnrc_pktbuf_add(NULL, NULL, 1, GNRC_NETTYPE_NETIF);
    expect(_pkt_no_ip_hdr);
    _pkt_no_ip_hdr = gnrc_pktbuf_add(_pkt_no_ip_hdr, NULL, 1, GNRC_NETTYPE_UNDEF);
    expect(_pkt_no_ip_hdr);
}

static void tear_down(void)
{
    gnrc_pktbuf_release(_pkt_w_ip_hdr->next->next);
    gnrc_pktbuf_release(_pkt_no_ip_hdr->next);
}

static void test_gnrc_ipv6_get_header(void)
{
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_get_header(_pkt_w_ip_hdr));
}

static void test_gnrc_ipv6_get_header_no_header(void)
{
    TEST_ASSERT_NULL(gnrc_ipv6_get_header(_pkt_no_ip_hdr));
}

static void test_gnrc_ipv6_get_header_check_version(void)
{
    ipv6_hdr_t *hdr = gnrc_ipv6_get_header(_pkt_w_ip_hdr);

    TEST_ASSERT(ipv6_hdr_get_version(hdr) == 0x06);
}

static void test_gnrc_ipv6_get_header_check_len(void)
{
    ipv6_hdr_t *hdr = gnrc_ipv6_get_header(_pkt_w_ip_hdr);

    TEST_ASSERT(byteorder_ntohs(hdr->len) == DEFAULT_TEST_LEN);
}

static void test_gnrc_ipv6_get_header_check_nh(void)
{
    ipv6_hdr_t *hdr = gnrc_ipv6_get_header(_pkt_w_ip_hdr);

    TEST_ASSERT(hdr->nh == DEFAULT_TEST_NH);
}

static void test_gnrc_ipv6_get_header_check_hl(void)
{
    ipv6_hdr_t *hdr = gnrc_ipv6_get_header(_pkt_w_ip_hdr);

    TEST_ASSERT(hdr->hl == DEFAULT_TEST_HL);
}

static void test_gnrc_ipv6_get_header_check_src(void)
{
    ipv6_hdr_t *hdr = gnrc_ipv6_get_header(_pkt_w_ip_hdr);
    ipv6_addr_t addr = DEFAULT_TEST_SRC;

    TEST_ASSERT(ipv6_addr_equal(&(hdr->src), &addr));
}

static void test_gnrc_ipv6_get_header_check_dst(void)
{
    ipv6_hdr_t *hdr = gnrc_ipv6_get_header(_pkt_w_ip_hdr);
    ipv6_addr_t addr = DEFAULT_TEST_DST;

    TEST_ASSERT(ipv6_addr_equal(&(hdr->dst), &addr));
}

Test *tests_gnrc_ipv6_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_gnrc_ipv6_get_header),
        new_TestFixture(test_gnrc_ipv6_get_header_no_header),
        new_TestFixture(test_gnrc_ipv6_get_header_check_version),
        new_TestFixture(test_gnrc_ipv6_get_header_check_len),
        new_TestFixture(test_gnrc_ipv6_get_header_check_nh),
        new_TestFixture(test_gnrc_ipv6_get_header_check_hl),
        new_TestFixture(test_gnrc_ipv6_get_header_check_src),
        new_TestFixture(test_gnrc_ipv6_get_header_check_dst),
    };

    EMB_UNIT_TESTCALLER(gnrc_ipv6_tests, set_up, tear_down, fixtures);

    return (Test *)&gnrc_ipv6_tests;
}

void tests_gnrc_ipv6(void)
{
    TESTS_RUN(tests_gnrc_ipv6_tests());
}
/** @} */
