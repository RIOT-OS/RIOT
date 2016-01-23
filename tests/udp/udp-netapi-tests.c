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
#include "kernel_types.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/nettest.h"
#include "net/protnum.h"
#include "net/gnrc/udp.h"

#include "udp-tests-common.h"
#include "udp-netapi-tests.h"

static kernel_pid_t _udp_pid = KERNEL_PID_UNDEF;

static void set_up(void)
{
    gnrc_netreg_entry_t *entry = gnrc_netreg_lookup(GNRC_NETTYPE_UDP,
                                                    GNRC_NETREG_DEMUX_CTX_ALL);
    gnrc_pktbuf_init();
    if ((entry != NULL) && (entry->next == NULL)) { /* there must be only one */
        _udp_pid = entry->pid;
    }
}

static void tear_down(void)
{
    _udp_pid = KERNEL_PID_UNDEF;
}

/**
 * @brief   Builds a UDP over IPv6 packet for sending
 *
 * @param[in] src       Source IPv6 address for the packet. May be NULL for
 *                      unspecified address (::).
 * @param[in] src_port  Source UDP port for the packet.
 * @param[in] dst       Destination IPv6 address for the packet. May be NULL for
 *                      unspecified address (::).
 * @param[in] dst_port  Destination UDP port for the packet.
 * @param[in] data      Payload data for the packet.
 * @param[in] data_len  Length of @p data.
 *
 * @return  A UDP over IPv6 packet for sending.
 * @return  NULL on packet buffer error.
 */
static gnrc_pktsnip_t *build_udp_send_packet(const ipv6_addr_t *src,
        uint16_t src_port, const ipv6_addr_t *dst, uint16_t dst_port,
        char *data, size_t data_len)
{
    gnrc_pktsnip_t *ipv6, *udp, *payload;
    size_t src_len = 0, dst_len = 0;

    payload = gnrc_pktbuf_add(NULL, data, data_len, GNRC_NETTYPE_UNDEF);

    if (payload == NULL) {
        return NULL;
    }

    udp = gnrc_udp_hdr_build(payload, (uint8_t *)&src_port, sizeof(src_port),
                             (uint8_t *)&dst_port, sizeof(dst_port));

    if (udp == NULL) {
        /* no need to release, we reset packet buffer on fail anyways */
        return NULL;
    }

    if (src != NULL) {
        src_len = sizeof(ipv6_addr_t);
    }

    if (dst != NULL) {
        dst_len = sizeof(ipv6_addr_t);
    }

    ipv6 = gnrc_ipv6_hdr_build(udp, (uint8_t *)src, src_len, (uint8_t *)dst, dst_len);

    return ipv6;
}

/**
 * @brief   Builds a received IPv6 header
 *
 * @param[in] src       Source IPv6 address. May be NULL for unspecified
 *                      address (::).
 * @param[in] dst       Destination IPv6 address. May be NULL for unspecified
 *                      address (::).
 * @param[in] data_len  Length of the UDP payload (minus UDP header).
 *
 * @return  An IPv6 header.
 * @return  NULL, on packet buffer error.
 */
static inline gnrc_pktsnip_t *build_ipv6_recv_header(const ipv6_addr_t *src,
        const ipv6_addr_t *dst, size_t data_len)
{
    gnrc_pktsnip_t *ipv6;
    size_t src_len = 0, dst_len = 0;

    if (src != NULL) {
        src_len = sizeof(ipv6_addr_t);
    }

    if (dst != NULL) {
        dst_len = sizeof(ipv6_addr_t);
    }

    ipv6 = gnrc_ipv6_hdr_build(NULL, (uint8_t *)src, src_len, (uint8_t *)dst, dst_len);

    if (ipv6 != NULL) {
        ipv6_hdr_t *ipv6_hdr = ipv6->data;
        ipv6_hdr->hl = 0;
        ipv6_hdr->nh = PROTNUM_UDP;
        ipv6_hdr->len = byteorder_htons(sizeof(udp_hdr_t) + data_len);
    }

    return ipv6;
}

/**
 * @brief   Build a received UDP over IPv6 packet.
 *
 * @param[in] src       Source IPv6 address for the packet. May be NULL for
 *                      unspecified address (::).
 * @param[in] src_port  Source UDP port for the packet.
 * @param[in] dst       Destination IPv6 address for the packet. May be NULL for
 *                      unspecified address (::).
 * @param[in] dst_port  Destination UDP port for the packet.
 * @param[in] checksum  Checksum for the UDP packet.
 * @param[in] data      Payload data for the packet.
 * @param[in] data_len  Length of @p data.
 *
 * @return  A received UDP over IPv6 packet.
 * @return  NULL on packet buffer error.
 */
static gnrc_pktsnip_t *build_udp_recv_packet(const ipv6_addr_t *src,
        uint16_t src_port, const ipv6_addr_t *dst, uint16_t dst_port,
        uint16_t checksum, char *data, size_t data_len)
{
    gnrc_pktsnip_t *ipv6, *payload;
    udp_hdr_t *udp_hdr;
    uint8_t *payload_data;

    ipv6 = build_ipv6_recv_header(src, dst, data_len);

    if (ipv6 == NULL) {
        return NULL;
    }

    payload = gnrc_pktbuf_add(ipv6, NULL, sizeof(udp_hdr_t) + data_len,
                              GNRC_NETTYPE_UDP);

    if (payload == NULL) {
        /* no need to release, we reset packet buffer on fail anyways */
        return NULL;
    }

    udp_hdr = payload->data;
    payload_data = ((uint8_t *)payload->data) + sizeof(udp_hdr_t);
    udp_hdr->src_port = byteorder_htons(src_port);
    udp_hdr->dst_port = byteorder_htons(dst_port);
    udp_hdr->checksum = byteorder_htons(checksum);
    udp_hdr->length = byteorder_htons(sizeof(udp_hdr_t) + data_len);

    memcpy(payload_data, data, data_len);

    return payload;
}

/**
 * @brief   Build a UDP over IPv6 packet as it is expected to be received by the
 *          application layer.
 *
 * @param[in] src       Source IPv6 address for the packet. May be NULL for
 *                      unspecified address (::).
 * @param[in] src_port  Source UDP port for the packet.
 * @param[in] dst       Destination IPv6 address for the packet. May be NULL for
 *                      unspecified address (::).
 * @param[in] dst_port  Destination UDP port for the packet.
 * @param[in] checksum  Checksum for the UDP packet.
 * @param[in] data      Payload data for the packet.
 * @param[in] data_len  Length of @p data.
 *
 * @return  A UDP over IPv6 packet as it is expected to be received by the
 *          application layer.
 * @return  NULL on packet buffer error.
 */
static gnrc_pktsnip_t *build_udp_recv_exp_packet(const ipv6_addr_t *src,
        uint16_t src_port, const ipv6_addr_t *dst, uint16_t dst_port,
        uint16_t checksum, char *data, size_t data_len)
{
    gnrc_pktsnip_t *ipv6, *udp, *payload;
    udp_hdr_t *udp_hdr;

    ipv6 = build_ipv6_recv_header(src, dst, data_len);

    if (ipv6 == NULL) {
        return NULL;
    }

    udp = gnrc_udp_hdr_build(ipv6, (uint8_t *)&src_port, sizeof(src_port),
                             (uint8_t *)&dst_port, sizeof(dst_port));

    if (udp == NULL) {
        /* no need to release, we reset packet buffer on fail anyways */
        return NULL;
    }

    udp_hdr = udp->data;
    udp_hdr->checksum = byteorder_htons(checksum);
    udp_hdr->length = byteorder_htons(sizeof(udp_hdr_t) + data_len);

    payload = gnrc_pktbuf_add(udp, data, data_len, GNRC_NETTYPE_UNDEF);

    return payload;
}

/**
 * @brief   Tests sending of UDP over IPv6 packets.
 *
 * * **in**: Packet with initialized headers.
 * * **out**: Unchanged packet.
 *
 * @pre     Packet buffer is empty and _udp_pid is initialized.
 * @post    Packet was received from UDP module as expected and
 *          packet buffer is empty
 */
static void test_send_ipv6(void)
{
    TEST_ASSERT(_udp_pid != KERNEL_PID_UNDEF);

    const ipv6_addr_t src = TEST_SRC_IPV6;
    const ipv6_addr_t dst = TEST_DST_IPV6;
    gnrc_pktsnip_t *in = build_udp_send_packet(&src, TEST_SRC_PORT,
                                               &dst, TEST_DST_PORT,
                                               TEST_DATA, sizeof(TEST_DATA));
    kernel_pid_t exp_senders[] = { _udp_pid };
    gnrc_pktsnip_t *exp_out[] = { build_udp_send_packet(&src, TEST_SRC_PORT,
                                                        &dst, TEST_DST_PORT,
                                                        TEST_DATA, sizeof(TEST_DATA))
                                };

    TEST_ASSERT_NOT_NULL(exp_out[0]);

    udp_hdr_t *exp_udp = exp_out[0]->next->data;

    exp_udp->length = byteorder_htons(sizeof(udp_hdr_t) + sizeof(TEST_DATA));

    TEST_ASSERT_NOT_NULL(in);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTEST_SUCCESS,
                          gnrc_nettest_send(_udp_pid, in, 1, exp_senders,
                                            (const gnrc_pktsnip_t **)exp_out,
                                            GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL));
    gnrc_pktbuf_release(exp_out[0]);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

/**
 * @brief   Tests receiving of UDP over IPv6 packets with wrong checksum.
 *
 * * **in**: Packet with initialized headers (wrong checksum).
 * * **out**: -
 *
 * @pre     Packet buffer is empty and _udp_pid is initialized.
 * @post    No packet was received and packet buffer is empty
 */
static void test_receive_ipv6_wrognrc_checksum(void)
{
    TEST_ASSERT(_udp_pid != KERNEL_PID_UNDEF);

    const ipv6_addr_t src = TEST_SRC_IPV6;
    const ipv6_addr_t dst = TEST_DST_IPV6;
    gnrc_pktsnip_t *in = build_udp_recv_packet(&src, TEST_SRC_PORT,
                                               &dst, TEST_DST_PORT,
                                               TEST_CSUM + 42, TEST_DATA,
                                               sizeof(TEST_DATA));

    TEST_ASSERT_NOT_NULL(in);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTEST_SUCCESS, /* UDP will drop this packet */
                          gnrc_nettest_receive(_udp_pid, in, 0, NULL, NULL,
                                               GNRC_NETTYPE_UDP, TEST_DST_PORT));
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

/**
 * @brief   Tests receiving of UDP over IPv6 packets with correct checksum.
 *
 * * **in**: Packet with initialized headers (correct checksum).
 * * **out**: Unchanged packet.
 *
 * @pre     Packet buffer is empty and _udp_pid is initialized.
 * @post    Packet was received from UDP module as expected and
 *          packet buffer is empty
 */
static void test_receive_ipv6(void)
{
    TEST_ASSERT(_udp_pid != KERNEL_PID_UNDEF);

    const ipv6_addr_t src = TEST_SRC_IPV6;
    const ipv6_addr_t dst = TEST_DST_IPV6;
    gnrc_pktsnip_t *in = build_udp_recv_packet(&src, TEST_SRC_PORT,
                                               &dst, TEST_DST_PORT,
                                               TEST_CSUM, TEST_DATA,
                                               sizeof(TEST_DATA));
    kernel_pid_t exp_senders[] = { _udp_pid };
    gnrc_pktsnip_t *exp_out[] = { build_udp_recv_exp_packet(&src, TEST_SRC_PORT,
                                                            &dst, TEST_DST_PORT,
                                                            TEST_CSUM, TEST_DATA,
                                                            sizeof(TEST_DATA))
                                };

    TEST_ASSERT_NOT_NULL(in);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTEST_SUCCESS,
                          gnrc_nettest_receive(_udp_pid, in, 1, exp_senders,
                                               (const gnrc_pktsnip_t **)exp_out,
                                               GNRC_NETTYPE_UDP, TEST_DST_PORT));
    gnrc_pktbuf_release(exp_out[0]);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

/**
 * @brief   Tests getting of options.
 *
 * * **in**: Any option.
 * * **out**: not supported error.
 *
 * @pre     _udp_pid is set.
 * @post    -
 */
static void test_get(void)
{
    TEST_ASSERT(_udp_pid != KERNEL_PID_UNDEF);

    uint8_t data[] = TEST_DATA;

    for (netopt_t i = 0; i < NETOPT_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(GNRC_NETTEST_SUCCESS,
                              gnrc_nettest_get(_udp_pid, i, TEST_CTX, data,
                                               sizeof(data), NULL, -ENOTSUP));
    }
}

/**
 * @brief   Tests setting of options.
 *
 * * **in**: Any option.
 * * **out**: not supported error.
 *
 * @pre     _udp_pid is set.
 * @post    -
 */
static void test_set(void)
{
    TEST_ASSERT(_udp_pid != KERNEL_PID_UNDEF);

    uint8_t data[] = TEST_DATA;

    for (netopt_t i = 0; i < NETOPT_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(GNRC_NETTEST_SUCCESS,
                              gnrc_nettest_set(_udp_pid, i, TEST_CTX, data,
                                               sizeof(data), -ENOTSUP));
    }
}

Test *udp_netapi_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_send_ipv6),
        new_TestFixture(test_receive_ipv6_wrognrc_checksum),
        new_TestFixture(test_receive_ipv6),
        new_TestFixture(test_get),
        new_TestFixture(test_set),
    };

    EMB_UNIT_TESTCALLER(udp_tests, set_up, tear_down, fixtures);

    return (Test *)&udp_tests;
}

int udp_netapi_tests_init(void)
{
    if (gnrc_udp_init() < 0) {
        puts("Error initializing UDP");
        return -1;
    }

    return 0;
}

/** @} */
