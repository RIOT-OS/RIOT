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

#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/pktbuf.h"

#include "unittests-constants.h"
#include "tests-gnrc_ipv6_hdr.h"

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

static void test_gnrc_ipv6_hdr_build__src_NULL(void)
{
    ipv6_addr_t dst = DEFAULT_TEST_DST;
    gnrc_pktsnip_t *pkt;
    ipv6_hdr_t *hdr;

    gnrc_pktbuf_init();
    TEST_ASSERT_NOT_NULL(pkt = gnrc_ipv6_hdr_build(NULL, NULL, &dst));
    hdr = pkt->data;
    TEST_ASSERT_NOT_NULL(hdr);
    TEST_ASSERT(ipv6_hdr_is(hdr));
    TEST_ASSERT_EQUAL_INT(0, ipv6_hdr_get_tc(hdr));
    TEST_ASSERT_EQUAL_INT(0, ipv6_hdr_get_fl(hdr));
    TEST_ASSERT_EQUAL_INT(PROTNUM_RESERVED, hdr->nh);
    TEST_ASSERT_EQUAL_INT(0, hdr->hl);
    TEST_ASSERT(ipv6_addr_equal(&dst, &hdr->dst));
    TEST_ASSERT(!gnrc_pktbuf_is_empty());
}

static void test_gnrc_ipv6_hdr_build__dst_NULL(void)
{
    ipv6_addr_t src = DEFAULT_TEST_SRC;
    gnrc_pktsnip_t *pkt;
    ipv6_hdr_t *hdr;

    gnrc_pktbuf_init();
    TEST_ASSERT_NOT_NULL(pkt = gnrc_ipv6_hdr_build(NULL, &src, NULL));
    hdr = pkt->data;
    TEST_ASSERT_NOT_NULL(hdr);
    TEST_ASSERT(ipv6_hdr_is(hdr));
    TEST_ASSERT_EQUAL_INT(0, ipv6_hdr_get_tc(hdr));
    TEST_ASSERT_EQUAL_INT(0, ipv6_hdr_get_fl(hdr));
    TEST_ASSERT_EQUAL_INT(PROTNUM_RESERVED, hdr->nh);
    TEST_ASSERT_EQUAL_INT(0, hdr->hl);
    TEST_ASSERT(ipv6_addr_equal(&src, &hdr->src));
    TEST_ASSERT(!gnrc_pktbuf_is_empty());
}

static void test_gnrc_ipv6_hdr_build__complete(void)
{
    ipv6_addr_t src = DEFAULT_TEST_SRC;
    ipv6_addr_t dst = DEFAULT_TEST_DST;
    gnrc_pktsnip_t *pkt;
    ipv6_hdr_t *hdr;

    gnrc_pktbuf_init();
    TEST_ASSERT_NOT_NULL(pkt = gnrc_ipv6_hdr_build(NULL, &src, &dst));
    hdr = pkt->data;
    TEST_ASSERT_NOT_NULL(hdr);
    TEST_ASSERT(ipv6_hdr_is(hdr));
    TEST_ASSERT_EQUAL_INT(0, ipv6_hdr_get_tc(hdr));
    TEST_ASSERT_EQUAL_INT(0, ipv6_hdr_get_fl(hdr));
    TEST_ASSERT_EQUAL_INT(PROTNUM_RESERVED, hdr->nh);
    TEST_ASSERT_EQUAL_INT(0, hdr->hl);
    TEST_ASSERT(ipv6_addr_equal(&src, &hdr->src));
    TEST_ASSERT(ipv6_addr_equal(&dst, &hdr->dst));
    TEST_ASSERT(!gnrc_pktbuf_is_empty());
}

Test *tests_gnrc_ipv6_hdr_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_gnrc_ipv6_hdr_build__src_NULL),
        new_TestFixture(test_gnrc_ipv6_hdr_build__dst_NULL),
        new_TestFixture(test_gnrc_ipv6_hdr_build__complete),
    };

    EMB_UNIT_TESTCALLER(ipv6_hdr_tests, NULL, NULL, fixtures);

    return (Test *)&ipv6_hdr_tests;
}

void tests_gnrc_ipv6_hdr(void)
{
    TESTS_RUN(tests_gnrc_ipv6_hdr_tests());
}
/** @} */
