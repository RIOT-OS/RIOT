/*
 * Copyright (C) 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
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

#include "net/gnrc/udp.h"
#include "net/ipv6/hdr.h"

#include "unittests-constants.h"
#include "tests-gnrc_udp.h"

static gnrc_pktsnip_t zero_snip = {
    .users = 0,
    .next = NULL,
    .data = NULL,
    .size = 0,
    .type = GNRC_NETTYPE_UNDEF,
};

static void test_gnrc_udp__csum_null(void)
{
    gnrc_pktsnip_t *hdr = NULL;
    gnrc_pktsnip_t *pseudo_hdr = NULL;

    TEST_ASSERT_EQUAL_INT(-EFAULT, gnrc_udp_calc_csum(hdr, pseudo_hdr));
}

static void test_gnrc_udp__csum_not_a_udp(void)
{
    gnrc_pktsnip_t hdr = zero_snip;
    gnrc_pktsnip_t pseudo_hdr = zero_snip;

    TEST_ASSERT_EQUAL_INT(-EBADMSG, gnrc_udp_calc_csum(&hdr, &pseudo_hdr));
}

static void test_gnrc_udp__csum_not_a_ipv6(void)
{
    gnrc_pktsnip_t payload = zero_snip;
    uint8_t payload_data[] = { 0 };
    gnrc_pktsnip_t hdr = zero_snip;
    udp_hdr_t hdr_data = (udp_hdr_t) {
        .src_port = byteorder_htons(0),
        .dst_port = byteorder_htons(0),
        .length = byteorder_htons(0),
        .checksum = byteorder_htons(0),
    };
    gnrc_pktsnip_t pseudo_hdr = zero_snip;
    ipv6_hdr_t pseudo_hdr_data = (ipv6_hdr_t) {
        .v_tc_fl = byteorder_htonl(0),
        .len = byteorder_htons((uint16_t) (sizeof(hdr_data) + sizeof(payload_data))),
        .nh = GNRC_NETTYPE_UDP,
        .hl = 0,
        .src = IPV6_ADDR_UNSPECIFIED,
        .dst = IPV6_ADDR_UNSPECIFIED,
    };

    pseudo_hdr.type = GNRC_NETTYPE_UNDEF; /* This should result in ENOENT */
    pseudo_hdr.data = &pseudo_hdr_data;
    pseudo_hdr.size = sizeof(pseudo_hdr_data);
    pseudo_hdr.next = &hdr;

    hdr.type = GNRC_NETTYPE_UDP;
    hdr.data = &hdr_data;
    hdr.size = sizeof(hdr_data);
    hdr.next = &payload;

    payload.data = payload_data;
    payload.size = 0;

    TEST_ASSERT_EQUAL_INT(-ENOENT, gnrc_udp_calc_csum(&hdr, &pseudo_hdr));
}

/**
 * @brief computes UDP checksum for given UDP payload and checksum.
 *
 * @param[in] payload_data UDP payload
 * @param[in] size         The size of the payload
 * @param[in] checksum     Checksum field of the UDP packet.
 *                         Will be overridden with the computed checksum.
 *
 * @return  0 on success
 * @return  non-zero on failure
 */
static uint16_t _compute_checksum(uint8_t *payload_data, size_t size, uint16_t *checksum) {
    gnrc_pktsnip_t payload = zero_snip;

    gnrc_pktsnip_t hdr = zero_snip;
    udp_hdr_t hdr_data = (udp_hdr_t) {
        .src_port = byteorder_htons(0),
        .dst_port = byteorder_htons(0),
        .length = byteorder_htons(0),
        .checksum = byteorder_htons(*checksum),
    };

    gnrc_pktsnip_t pseudo_hdr = zero_snip;
    ipv6_hdr_t pseudo_hdr_data = (ipv6_hdr_t) {
        .v_tc_fl = byteorder_htonl(0),
        .len = byteorder_htons((uint16_t) (sizeof(hdr_data) + size)),
        .nh = GNRC_NETTYPE_UDP,
        .hl = 0,
        .src = IPV6_ADDR_UNSPECIFIED,
        .dst = IPV6_ADDR_UNSPECIFIED,
    };

    pseudo_hdr.type = GNRC_NETTYPE_IPV6;
    pseudo_hdr.data = &pseudo_hdr_data;
    pseudo_hdr.size = sizeof(pseudo_hdr_data);
    pseudo_hdr.next = &hdr;

    hdr.type = GNRC_NETTYPE_UDP;
    hdr.data = &hdr_data;
    hdr.size = sizeof(hdr_data);
    hdr.next = &payload;

    payload.data = payload_data;
    payload.size = size;

    int status = gnrc_udp_calc_csum(&hdr, &pseudo_hdr);

    *checksum = byteorder_ntohs(hdr_data.checksum);

    return status;
}

static void test_gnrc_udp__csum_simple1(void)
{
    uint8_t payload_data[] = {
        0x00, 0x01, 0xFF, 0xE2,
    };

    uint16_t checksum = 0;

    int status = _compute_checksum(payload_data, sizeof(payload_data), &checksum);

    TEST_ASSERT_EQUAL_INT(0, status);
    TEST_ASSERT_EQUAL_INT((~0x0001) & 0xFFFF, checksum);
}

static void test_gnrc_udp__csum_simple2(void)
{
    uint8_t payload_data[] = {
        0x00, 0x02, 0xFF, 0xE2,
    };

    uint16_t checksum = 0;

    int status = _compute_checksum(payload_data, sizeof(payload_data), &checksum);

    TEST_ASSERT_EQUAL_INT(0, status);
    TEST_ASSERT_EQUAL_INT((~0x0002) & 0xFFFF, checksum);
}

static void test_gnrc_udp__csum_applying_twice_yields_ffff(void)
{
    uint8_t payload_data[] = {
        0x00, 0x02, 0xFF, 0xE2,
    };

    uint16_t checksum = 0;

    int status1 = _compute_checksum(payload_data, sizeof(payload_data), &checksum);
    int status2 = _compute_checksum(payload_data, sizeof(payload_data), &checksum);

    TEST_ASSERT_EQUAL_INT(0, status1);
    TEST_ASSERT_EQUAL_INT(0, status2);
    TEST_ASSERT_EQUAL_INT(0xFFFF, checksum);
}

static void test_gnrc_udp__csum_ffff(void)
{
    uint8_t payload_data[] = {
        0x00, 0x00, 0xFF, 0xE2,
    };

    uint16_t checksum = 0;

    int status = _compute_checksum(payload_data, sizeof(payload_data), &checksum);

    TEST_ASSERT_EQUAL_INT(0, status);
    TEST_ASSERT_EQUAL_INT(0xFFFF, checksum);
}

static void test_gnrc_udp__csum_zero(void)
{
    uint8_t payload_data[] = {
        0xFF, 0xFF, 0xFF, 0xE2,
    };

    uint16_t checksum = 0;

    int status = _compute_checksum(payload_data, sizeof(payload_data), &checksum);

    TEST_ASSERT_EQUAL_INT(0, status);
    /* https://tools.ietf.org/html/rfc8200#section-8.1
     * bullet 4
     * "if that computation yields a result of zero, it must be changed
     * to hex FFFF for placement in the UDP header."
     */
    TEST_ASSERT_EQUAL_INT(0xFFFF, checksum);
}

static void test_gnrc_udp__csum_all(void)
{
    uint8_t payload_data[] = {
        0x00, 0x00, 0xFF, 0xE2,
    };

    for (uint32_t i = 0; i < 0x10000; i++) {
        payload_data[0] = i >> 8;
        payload_data[1] = i & 0xFF;

        uint16_t checksum = 0;

        int status = _compute_checksum(payload_data, sizeof(payload_data), &checksum);

        TEST_ASSERT_EQUAL_INT(0, status);
        if (i == 0xFFFF) {
            /* https://tools.ietf.org/html/rfc8200#section-8.1
             * bullet 4
             * "if that computation yields a result of zero, it must be changed
             * to hex FFFF for placement in the UDP header."
             */
            TEST_ASSERT_EQUAL_INT(0xFFFF, checksum);
        } else {
            TEST_ASSERT_EQUAL_INT(0xFFFF - i, checksum);
        }
    }
}

Test *tests_gnrc_udp_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_gnrc_udp__csum_null),
        new_TestFixture(test_gnrc_udp__csum_not_a_udp),
        new_TestFixture(test_gnrc_udp__csum_not_a_ipv6),
        new_TestFixture(test_gnrc_udp__csum_simple1),
        new_TestFixture(test_gnrc_udp__csum_simple2),
        new_TestFixture(test_gnrc_udp__csum_applying_twice_yields_ffff),
        new_TestFixture(test_gnrc_udp__csum_ffff),
        new_TestFixture(test_gnrc_udp__csum_zero),
        new_TestFixture(test_gnrc_udp__csum_all),
    };

    EMB_UNIT_TESTCALLER(gnrc_udp_tests, NULL, NULL, fixtures);

    return (Test *)&gnrc_udp_tests;
}

void tests_gnrc_udp(void)
{
    TESTS_RUN(tests_gnrc_udp_tests());
}
/** @} */
