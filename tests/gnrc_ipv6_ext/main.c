/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Tests extension header handling of gnrc stack.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "net/ethernet.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/conf.h"
#include "net/gnrc/netif/ethernet.h"
#include "net/gnrc/netif/hdr.h"
#include "net/netdev_test.h"

static char _netif_stack[THREAD_STACKSIZE_SMALL];
static netdev_test_t _dev;

static int _get_netdev_device_type(netdev_t *netdev, void *value, size_t max_len)
{
    assert(max_len == sizeof(uint16_t));
    (void)netdev;

    *((uint16_t *)value) = NETDEV_TYPE_ETHERNET;
    return sizeof(uint16_t);
}

static int _get_netdev_max_packet_size(netdev_t *netdev, void *value,
                                       size_t max_len)
{
    assert(max_len == sizeof(uint16_t));
    (void)netdev;

    *((uint16_t *)value) = ETHERNET_DATA_LEN;
    return sizeof(uint16_t);
}

static void _init_interface(void)
{
    gnrc_netif_t *netif;
    ipv6_addr_t addr = IPV6_ADDR_UNSPECIFIED;

    netdev_test_setup(&_dev, NULL);
    netdev_test_set_get_cb(&_dev, NETOPT_DEVICE_TYPE,
                           _get_netdev_device_type);
    netdev_test_set_get_cb(&_dev, NETOPT_MAX_PACKET_SIZE,
                           _get_netdev_max_packet_size);
    netif = gnrc_netif_ethernet_create(
            _netif_stack, THREAD_STACKSIZE_SMALL, GNRC_NETIF_PRIO,
            "dummy_netif", (netdev_t *)&_dev);

    addr.u8[0] = 0xfd;
    addr.u8[1] = 0x01;
    addr.u8[15] = 0x02;

    xtimer_usleep(500); /* wait for thread to start */
    /* add addresses fd01::02/64 and fd01::3/64 to interface */
    for (uint8_t i = 0x2; i <= 0x3; i++) {
        addr.u8[15] = i;
        if (gnrc_netapi_set(netif->pid, NETOPT_IPV6_ADDR, 64U << 8U, &addr,
                            sizeof(addr)) < 0) {
            printf("error: unable to add IPv6 address fd01::%x/64 to interface %u\n",
                   addr.u8[15], netif->pid);
        }
    }
}

static void _send_packet_raw(void)
{
    gnrc_netif_t *iface = gnrc_netif_iter(NULL);

    gnrc_netif_hdr_t netif_hdr;

    gnrc_netif_hdr_init(&netif_hdr, 8, 8);

    netif_hdr.if_pid = iface->pid;

    uint8_t data[] = {
        /* IPv6 Header */
        0x60, 0x00, 0x00, 0x00, /* version, traffic class, flow label */
        0x00, 0x2a,             /* payload length: 42 */
        0x00,                   /* next header: Hop-by-Hop Option */
        0x10,                   /* hop limit: 16 */
        /* source address: fd01::1 */
        0xfd, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01,
        /* destination address: fd01::2 */
        0xfd, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x02,

        /* Hop-by-Hop Options Header */
        /* https://tools.ietf.org/html/rfc6553 */
        0x2b,       /* next header: IPv6-Route */
        0x00,       /* hdr ext len: 0 * 8 + 8 = 8 */
        0x63,       /* option type: RPL Option */
        0x04,       /* opt data len: 4 */
        0x80,       /* flags, Down: 1, Rank-Error: 0, Forwarding-Error: 0 */
        0x00,       /* RPLInstanceID */
        0x80, 0x00, /* SenderRank */

        /* RPL Routing Header */
        /* https://tools.ietf.org/html/rfc6554 */
        0x11,               /* next header: UDP */
        0x02,               /* hdr ext len: 2 * 8 + 8 = 24 */
        0x03,               /* routing type: SRH */
        0x02,               /* segments left: 2 */
        0xef,               /* ComprI: 14, ComprE: 15 */
        0xd0, 0x00, 0x00,   /* pad and reserved */
        /* address: fd01::3, fd01::2 */
        0x00, 0x03, 0x02, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,

        /* UDP (ignored) */
        0x1f, 0x90, /* source port: 8080 */
        0x1f, 0x90, /* destination port: 8080 */
        0x00, 0x0a, /* length: 10 */
        0xff, 0xff, /* checksum */
        0x00, 0x00, /* payload */
    };

    gnrc_pktsnip_t *netif = gnrc_pktbuf_add(NULL,
                                            &netif_hdr,
                                            sizeof(netif_hdr),
                                            GNRC_NETTYPE_NETIF);
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(netif,
                                          data,
                                          sizeof(data),
                                          GNRC_NETTYPE_UNDEF);

    gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL, pkt);

    printf("pkt->users: %d\n", pkt->users);
    assert(pkt->users == 0);
}

static void _send_packet_parsed(void)
{
    gnrc_netif_t *iface = gnrc_netif_iter(NULL);

    gnrc_netif_hdr_t netif_hdr;

    gnrc_netif_hdr_init(&netif_hdr, 8, 8);

    netif_hdr.if_pid = iface->pid;

    uint8_t ipv6_data[] = {
        /* IPv6 Header */
        0x60, 0x00, 0x00, 0x00, /* version, traffic class, flow label */
        0x00, 0x2a,             /* payload length: 42 */
        0x00,                   /* next header: Hop-by-Hop Option */
        0x10,                   /* hop limit: 16 */
        /* source address: fd01::1 */
        0xfd, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01,
        /* destination address: fd01::2 */
        0xfd, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x02,
    };

    uint8_t hop_by_hop_options_data[] = {
        /* Hop-by-Hop Options Header */
        /* https://tools.ietf.org/html/rfc6553 */
        0x2b,       /* next header: IPv6-Route */
        0x00,       /* hdr ext len: 0 * 8 + 8 = 8 */
        0x63,       /* option type: RPL Option */
        0x04,       /* opt data len: 4 */
        0x80,       /* flags, Down: 1, Rank-Error: 0, Forwarding-Error: 0 */
        0x00,       /* RPLInstanceID */
        0x80, 0x00, /* SenderRank */
    };

    uint8_t rpl_routing_data[] = {
        /* RPL Routing Header */
        /* https://tools.ietf.org/html/rfc6554 */
        0x11,               /* next header: UDP */
        0x02,               /* hdr ext len: 2 * 8 + 8 = 24 */
        0x03,               /* routing type: SRH */
        0x02,               /* segments left: 2 */
        0xef,               /* ComprI: 14, ComprE: 15 */
        0xd0, 0x00, 0x00,   /* pad and reserved */
        /* address: fd01::3, fd01::2 */
        0x00, 0x03, 0x02, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    uint8_t udp_data[] = {
        /* UDP (ignored) */
        0x1f, 0x90, /* source port: 8080 */
        0x1f, 0x90, /* destination port: 8080 */
        0x00, 0x0a, /* length: 10 */
        0xff, 0xff, /* checksum */
    };

    uint8_t udp_payload[] = {
        0x00, 0x00,
    };

    gnrc_pktsnip_t *netif =
        gnrc_pktbuf_add(NULL,
                        &netif_hdr,
                        sizeof(netif_hdr),
                        GNRC_NETTYPE_NETIF);
    gnrc_pktsnip_t *ipv6 =
        gnrc_pktbuf_add(netif,
                        &ipv6_data,
                        sizeof(ipv6_data),
                        GNRC_NETTYPE_IPV6);
    gnrc_pktsnip_t *hop_by_hop_options =
        gnrc_pktbuf_add(ipv6,
                        &hop_by_hop_options_data,
                        sizeof(hop_by_hop_options_data),
                        GNRC_NETTYPE_IPV6_EXT);
    gnrc_pktsnip_t *rpl_routing =
        gnrc_pktbuf_add(hop_by_hop_options,
                        &rpl_routing_data,
                        sizeof(rpl_routing_data),
                        GNRC_NETTYPE_IPV6_EXT);
    gnrc_pktsnip_t *udp =
        gnrc_pktbuf_add(rpl_routing,
                        udp_data,
                        sizeof(udp_data),
                        GNRC_NETTYPE_UDP);
    gnrc_pktsnip_t *pkt =
        gnrc_pktbuf_add(udp,
                        &udp_payload,
                        sizeof(udp_payload),
                        GNRC_NETTYPE_UNDEF);

    gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL, pkt);

    printf("pkt->users: %d\n", pkt->users);
    assert(pkt->users == 0);
}


int main(void)
{
    puts("RIOT network stack example application");

    _init_interface();
    _send_packet_raw();
    _send_packet_parsed();

    /* should be never reached */
    return 0;
}
