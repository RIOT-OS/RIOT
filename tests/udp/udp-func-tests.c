/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
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
#include <inttypes.h>
#include <stdio.h>

#include "embUnit.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/udp.h"
#include "net/udp.h"

#include "udp-tests-common.h"
#include "udp-func-tests.h"

void set_up(void)
{
    gnrc_pktbuf_init();
}

static void test_udp_calc_csum__EBADMSG(void)
{
    gnrc_pktsnip_t hdr = { 1, NULL, "abcdefg", 8, GNRC_NETTYPE_TEST };
    gnrc_pktsnip_t pseudo_hdr = { 1, &hdr, "abcdefghijklmnopqrstuvwxyz0123456789+-!",
                                  40, GNRC_NETTYPE_IPV6
                                };

    TEST_ASSERT_EQUAL_INT(-EBADMSG, gnrc_udp_calc_csum(&hdr, &pseudo_hdr));
}

static void test_udp_calc_csum__EFAULT_hdr_NULL(void)
{
    gnrc_pktsnip_t pseudo_hdr = { 1, NULL, "abcdefghijklmnopqrstuvwxyz0123456789+-!",
                                  40, GNRC_NETTYPE_IPV6
                                };

    TEST_ASSERT_EQUAL_INT(-EFAULT, gnrc_udp_calc_csum(NULL, &pseudo_hdr));
}

static void test_udp_calc_csum__EFAULT_pseudo_hdr_NULL(void)
{
    gnrc_pktsnip_t hdr = { 1, NULL, "abcdefg", 8, GNRC_NETTYPE_UDP };

    TEST_ASSERT_EQUAL_INT(-EFAULT, gnrc_udp_calc_csum(&hdr, NULL));
}

static void test_udp_calc_csum__ENOENT(void)
{
    gnrc_pktsnip_t hdr = { 1, NULL, "abcdefg", 8, GNRC_NETTYPE_UDP };
    gnrc_pktsnip_t pseudo_hdr = { 1, &hdr, "abcdef", 8, GNRC_NETTYPE_TEST };

    TEST_ASSERT_EQUAL_INT(-ENOENT, gnrc_udp_calc_csum(&hdr, &pseudo_hdr));
}

#if 0   /* see #3201*/
static void test_udp_calc_csum__odd_inner_payload_snip(void)
{
    char *payload_data = TEST_DATA;
    udp_hdr_t udp_data;
    size_t offset = ((sizeof(payload_data) * 2) / 4) - 1; /* get an odd number */
    gnrc_pktsnip_t payload2 = { 1, NULL, payload_data + offset,
                                sizeof(payload_data) - offset, GNRC_NETTYPE_UNDEF
                              };
    gnrc_pktsnip_t payload1 = { 1, &payload2, payload_data, offset,
                                GNRC_NETTYPE_UNDEF
                              };
    gnrc_pktsnip_t hdr = { 1, &payload1, &udp_data, 8, GNRC_NETTYPE_UDP };
    gnrc_pktsnip_t pseudo_hdr = { 1, &hdr, "abcdefghijklmnopqrstuvwxyz0123456789+-!",
                                  40, GNRC_NETTYPE_IPV6
                                };

    udp_data.src_port = byteorder_htons(TEST_SRC_PORT);
    udp_data.dst_port = byteorder_htons(TEST_DST_PORT);
    udp_data.length = byteorder_htons(sizeof(udp_hdr_t) + sizeof(payload_data));
    udp_data.checksum.u16 = 0;

    TEST_ASSERT_EQUAL_INT(0, gnrc_udp_calc_csum(&hdr, &pseudo_hdr));
    TEST_ASSERT_EQUAL_INT(TEST_SRC_PORT, byteorder_ntohs(udp_data.src_port));
    TEST_ASSERT_EQUAL_INT(TEST_DST_PORT, byteorder_ntohs(udp_data.dst_port));
    TEST_ASSERT_EQUAL_INT(sizeof(udp_hdr_t) + sizeof(payload_data),
                          byteorder_ntohs(udp_data.length));
    TEST_ASSERT_EQUAL_INT(0xb48e, udp_data.checksum.u16);
}
#endif

static void test_udp_calc_csum__even_inner_payload_snip(void)
{
    char *payload_data = TEST_DATA;
    udp_hdr_t udp_data;
    size_t offset = ((sizeof(payload_data) * 2) / 4); /* get an odd number */
    gnrc_pktsnip_t payload2 = { 1, NULL, payload_data + offset,
                                sizeof(payload_data) - offset, GNRC_NETTYPE_UNDEF
                              };
    gnrc_pktsnip_t payload1 = { 1, &payload2, payload_data, offset,
                                GNRC_NETTYPE_UNDEF
                              };
    gnrc_pktsnip_t hdr = { 1, &payload1, &udp_data, 8, GNRC_NETTYPE_UDP };
    gnrc_pktsnip_t pseudo_hdr = { 1, &hdr, "abcdefghijklmnopqrstuvwxyz0123456789+-!",
                                  40, GNRC_NETTYPE_IPV6
                                };

    udp_data.src_port = byteorder_htons(TEST_SRC_PORT);
    udp_data.dst_port = byteorder_htons(TEST_DST_PORT);
    udp_data.length = byteorder_htons(sizeof(udp_hdr_t) + sizeof(payload_data));
    udp_data.checksum.u16 = 0;

    TEST_ASSERT_EQUAL_INT(0, gnrc_udp_calc_csum(&hdr, &pseudo_hdr));
    TEST_ASSERT_EQUAL_INT(TEST_SRC_PORT, byteorder_ntohs(udp_data.src_port));
    TEST_ASSERT_EQUAL_INT(TEST_DST_PORT, byteorder_ntohs(udp_data.dst_port));
    TEST_ASSERT_EQUAL_INT(sizeof(udp_hdr_t) + sizeof(payload_data),
                          byteorder_ntohs(udp_data.length));
    TEST_ASSERT_EQUAL_INT(0xb48e, udp_data.checksum.u16);
}

static void test_udp_calc_csum__monolithic_payload_snip(void)
{
    char *payload_data = TEST_DATA;
    udp_hdr_t udp_data;
    gnrc_pktsnip_t payload = { 1, NULL, payload_data, sizeof(payload_data),
                               GNRC_NETTYPE_UNDEF
                             };
    gnrc_pktsnip_t hdr = { 1, &payload, &udp_data, 8, GNRC_NETTYPE_UDP };
    gnrc_pktsnip_t pseudo_hdr = { 1, &hdr, "abcdefghijklmnopqrstuvwxyz0123456789+-!",
                                  40, GNRC_NETTYPE_IPV6
                                };

    udp_data.src_port = byteorder_htons(TEST_SRC_PORT);
    udp_data.dst_port = byteorder_htons(TEST_DST_PORT);
    udp_data.length = byteorder_htons(sizeof(udp_hdr_t) + sizeof(payload_data));
    udp_data.checksum.u16 = 0;

    TEST_ASSERT_EQUAL_INT(0, gnrc_udp_calc_csum(&hdr, &pseudo_hdr));
    TEST_ASSERT_EQUAL_INT(TEST_SRC_PORT, byteorder_ntohs(udp_data.src_port));
    TEST_ASSERT_EQUAL_INT(TEST_DST_PORT, byteorder_ntohs(udp_data.dst_port));
    TEST_ASSERT_EQUAL_INT(sizeof(udp_hdr_t) + sizeof(payload_data),
                          byteorder_ntohs(udp_data.length));
    TEST_ASSERT_EQUAL_INT(0xb48e, udp_data.checksum.u16);
}

static void test_udp_hdr_build__payload_NULL(void)
{
    gnrc_pktsnip_t *pkt;
    uint16_t src = TEST_SRC_PORT;
    uint16_t dst = TEST_DST_PORT;
    udp_hdr_t *udp_data;

    pkt = gnrc_udp_hdr_build(NULL, (uint8_t *)&src, sizeof(uint16_t),
                             (uint8_t *)&dst, sizeof(uint16_t));
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(sizeof(udp_hdr_t), pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UDP, pkt->type);
    TEST_ASSERT_NOT_NULL(pkt->data);

    udp_data = pkt->data;

    TEST_ASSERT_EQUAL_INT(0, udp_data->length.u16);
    TEST_ASSERT_EQUAL_INT(0, udp_data->checksum.u16);
    TEST_ASSERT_EQUAL_INT(TEST_SRC_PORT, byteorder_ntohs(udp_data->src_port));
    TEST_ASSERT_EQUAL_INT(TEST_DST_PORT, byteorder_ntohs(udp_data->dst_port));
}

static void test_udp_hdr_build__src_NULL(void)
{
    char *payload_data = TEST_DATA;
    gnrc_pktsnip_t payload = { 1, NULL, payload_data, sizeof(payload_data),
                               GNRC_NETTYPE_UNDEF
                             };
    gnrc_pktsnip_t *pkt;
    uint16_t dst = TEST_DST_PORT;

    pkt = gnrc_udp_hdr_build(&payload, NULL, sizeof(uint16_t), (uint8_t *)&dst,
                             sizeof(uint16_t));
    TEST_ASSERT_NULL(pkt);
}

static void test_udp_hdr_build__src_len_0(void)
{
    char *payload_data = TEST_DATA;
    gnrc_pktsnip_t payload = { 1, NULL, payload_data, sizeof(payload_data),
                               GNRC_NETTYPE_UNDEF
                             };
    gnrc_pktsnip_t *pkt;
    uint16_t src = TEST_SRC_PORT;
    uint16_t dst = TEST_DST_PORT;

    pkt = gnrc_udp_hdr_build(&payload, (uint8_t *)&src, 0, (uint8_t *)&dst,
                             sizeof(uint16_t));
    TEST_ASSERT_NULL(pkt);
}

static void test_udp_hdr_build__dst_NULL(void)
{
    char *payload_data = TEST_DATA;
    gnrc_pktsnip_t payload = { 1, NULL, payload_data, sizeof(payload_data),
                               GNRC_NETTYPE_UNDEF
                             };
    gnrc_pktsnip_t *pkt;
    uint16_t src = TEST_SRC_PORT;

    pkt = gnrc_udp_hdr_build(&payload, (uint8_t *)&src, sizeof(uint16_t), NULL,
                             sizeof(uint16_t));
    TEST_ASSERT_NULL(pkt);
}

static void test_udp_hdr_build__dst_len_0(void)
{
    char *payload_data = TEST_DATA;
    gnrc_pktsnip_t payload = { 1, NULL, payload_data, sizeof(payload_data),
                               GNRC_NETTYPE_UNDEF
                             };
    gnrc_pktsnip_t *pkt;
    uint16_t src = TEST_SRC_PORT;
    uint16_t dst = TEST_DST_PORT;

    pkt = gnrc_udp_hdr_build(&payload, (uint8_t *)&src, sizeof(uint16_t),
                             (uint8_t *)&dst, 0);
    TEST_ASSERT_NULL(pkt);
}

static void test_udp_hdr_build__all_set(void)
{
    char *payload_data = TEST_DATA;
    gnrc_pktsnip_t payload = { 1, NULL, payload_data, sizeof(payload_data),
                               GNRC_NETTYPE_UNDEF
                             };
    gnrc_pktsnip_t *pkt;
    uint16_t src = TEST_SRC_PORT;
    uint16_t dst = TEST_DST_PORT;
    udp_hdr_t *udp_data;

    pkt = gnrc_udp_hdr_build(&payload, (uint8_t *)&src, sizeof(uint16_t),
                             (uint8_t *)&dst, sizeof(uint16_t));
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
    TEST_ASSERT(pkt->next == &payload);
    TEST_ASSERT_EQUAL_INT(sizeof(udp_hdr_t), pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UDP, pkt->type);
    TEST_ASSERT_NOT_NULL(pkt->data);

    udp_data = pkt->data;

    TEST_ASSERT_EQUAL_INT(0, udp_data->length.u16);
    TEST_ASSERT_EQUAL_INT(0, udp_data->checksum.u16);
    TEST_ASSERT_EQUAL_INT(TEST_SRC_PORT, byteorder_ntohs(udp_data->src_port));
    TEST_ASSERT_EQUAL_INT(TEST_DST_PORT, byteorder_ntohs(udp_data->dst_port));
}

Test *udp_func_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_udp_calc_csum__EBADMSG),
        new_TestFixture(test_udp_calc_csum__EFAULT_hdr_NULL),
        new_TestFixture(test_udp_calc_csum__EFAULT_pseudo_hdr_NULL),
        new_TestFixture(test_udp_calc_csum__ENOENT),
        /* new_TestFixture(test_udp_calc_csum__odd_inner_payload_snip), see #3201 */
        new_TestFixture(test_udp_calc_csum__even_inner_payload_snip),
        new_TestFixture(test_udp_calc_csum__monolithic_payload_snip),
        new_TestFixture(test_udp_hdr_build__payload_NULL),
        new_TestFixture(test_udp_hdr_build__src_NULL),
        new_TestFixture(test_udp_hdr_build__src_len_0),
        new_TestFixture(test_udp_hdr_build__dst_NULL),
        new_TestFixture(test_udp_hdr_build__dst_len_0),
        new_TestFixture(test_udp_hdr_build__all_set)
    };

    EMB_UNIT_TESTCALLER(udp_tests, set_up, NULL, fixtures);

    return (Test *)&udp_tests;
}

/** @} */
