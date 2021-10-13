/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "event.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/sock/ip.h"
#include "net/sock/udp.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc/udp.h"
#include "net/protnum.h"
#include "od.h"
#include "test_utils/expect.h"
#include "thread.h"

#include "net/sock/async/event.h"

#define RECV_BUFFER_SIZE        (128)

#define TEST_PORT               (38664U)
#define TEST_LOCAL              { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }
#define TEST_REMOTE             { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 }
#define TEST_PAYLOAD            { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }

static const uint8_t _test_local[] = TEST_LOCAL;
static const uint8_t _test_remote[] = TEST_REMOTE;
static const uint8_t _test_payload[] = TEST_PAYLOAD;

static gnrc_netreg_entry_t _pktdump;

static char _addr_str[IPV6_ADDR_MAX_STR_LEN];
static event_queue_t _ev_queue;
static uint8_t _buffer[128];
static sock_ip_t _ip_sock;
static sock_udp_t _udp_sock;

/* module is not compiled in, so provide this function for the test */
ipv6_hdr_t *gnrc_ipv6_get_header(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *tmp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    if (tmp == NULL) {
        return NULL;
    }

    expect(tmp->data != NULL);
    expect(tmp->size >= sizeof(ipv6_hdr_t));
    expect(ipv6_hdr_is(tmp->data));

    return ((ipv6_hdr_t*) tmp->data);
}

static void _recv_udp(sock_udp_t *sock, sock_async_flags_t flags, void *arg)
{
    expect(strcmp(arg, "test") == 0);
    printf("UDP event triggered: %04X\n", flags);
    if (flags & SOCK_ASYNC_MSG_RECV) {
        sock_udp_ep_t remote;
        ssize_t res;

        if ((res = sock_udp_recv(sock, _buffer, sizeof(_buffer), 0,
                                 &remote)) >= 0) {
            printf("Received UDP packet from [%s]:%u:\n",
                   ipv6_addr_to_str(_addr_str, (ipv6_addr_t *)remote.addr.ipv6,
                                    sizeof(_addr_str)),
                   remote.port);
            od_hex_dump(_buffer, res, OD_WIDTH_DEFAULT);
        }
    }
    if (flags & SOCK_ASYNC_MSG_SENT) {
        puts("UDP message successfully sent");
    }
}

static void _recv_ip(sock_ip_t *sock, sock_async_flags_t flags, void *arg)
{
    expect(strcmp(arg, "test") == 0);
    printf("IP event triggered: %04X\n", flags);
    if (flags & SOCK_ASYNC_MSG_RECV) {
        sock_ip_ep_t remote;
        ssize_t res;

        if ((res = sock_ip_recv(sock, _buffer, sizeof(_buffer), 0,
                                &remote)) >= 0) {
            printf("Received IP packet from [%s]:\n",
                   ipv6_addr_to_str(_addr_str, (ipv6_addr_t *)remote.addr.ipv6,
                                    sizeof(_addr_str)));
            od_hex_dump(_buffer, res, OD_WIDTH_DEFAULT);
        }
    }
    if (flags & SOCK_ASYNC_MSG_SENT) {
        puts("IP message successfully sent");
    }
}

int main(void)
{
    gnrc_pktsnip_t *pkt;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;

    event_queue_init(&_ev_queue);
    /* register for IPv6 to have a target */
    gnrc_netreg_entry_init_pid(&_pktdump, GNRC_NETREG_DEMUX_CTX_ALL,
                               gnrc_pktdump_pid);
    gnrc_netreg_register(GNRC_NETTYPE_IPV6, &_pktdump);

    local.port = TEST_PORT;
    sock_udp_create(&_udp_sock, &local, NULL, 0);
    sock_ip_create(&_ip_sock, (sock_ip_ep_t *)&local, NULL, PROTNUM_UDP, 0);

    sock_udp_event_init(&_udp_sock, &_ev_queue, _recv_udp, "test");
    sock_ip_event_init(&_ip_sock, &_ev_queue, _recv_ip, "test");
    memcpy(remote.addr.ipv6, _test_remote, sizeof(_test_remote));
    remote.port = TEST_PORT - 1;

    sock_udp_send(&_udp_sock, _test_payload, sizeof(_test_payload), &remote);
    sock_ip_send(&_ip_sock, _test_payload, sizeof(_test_payload),
                 PROTNUM_RESERVED, (sock_ip_ep_t *)&remote);

    /* create packet to inject for reception */
    pkt = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    expect(pkt != NULL);
    memset(pkt->data, 0, pkt->size);
    pkt = gnrc_ipv6_hdr_build(pkt, (ipv6_addr_t *)&_test_remote,
                              (ipv6_addr_t *)&_test_local);
    expect(pkt != NULL);
    /* module is not compiled in, so set header type manually */
    pkt->type = GNRC_NETTYPE_IPV6;
    pkt = gnrc_udp_hdr_build(pkt, TEST_PORT - 1, TEST_PORT);
    expect(pkt != NULL);
    pkt = gnrc_pktbuf_add(pkt, _test_payload, sizeof(_test_payload),
                          GNRC_NETTYPE_UNDEF);
    expect(pkt != NULL);
    /* we dispatch twice, so hold one time */
    gnrc_pktbuf_hold(pkt, 1);

    /* trigger receive on UDP sock */
    gnrc_netapi_dispatch_receive(GNRC_NETTYPE_UDP, TEST_PORT, pkt);
    /* trigger receive on IP sock */
    gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6, PROTNUM_UDP, pkt);
    event_loop(&_ev_queue);
    return 0;
}

/** @} */
