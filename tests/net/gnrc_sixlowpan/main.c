/*
 * SPDX-FileCopyrightText: 2015-2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/conf.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pktdump.h"
#include "net/netdev_test.h"
#include "test_utils/expect.h"
#include "xtimer.h"

#define IEEE802154_MAX_FRAG_SIZE    (102)
#define IEEE802154_LOCAL_EUI64     { \
        0x02, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x01 \
    }
#define IEEE802154_REMOTE_EUI64     { \
        0x02, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x02 \
    }

static gnrc_netif_t _netif;
static char _netif_stack[THREAD_STACKSIZE_DEFAULT];
static netdev_test_t _ieee802154_dev;
static const uint8_t _ieee802154_local_eui64[] = IEEE802154_LOCAL_EUI64;

static int _get_netdev_device_type(netdev_t *netdev, void *value, size_t max_len)
{
    expect(max_len == sizeof(uint16_t));
    (void)netdev;

    *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
    return sizeof(uint16_t);
}

static int _get_netdev_proto(netdev_t *netdev, void *value, size_t max_len)
{
    expect(max_len == sizeof(gnrc_nettype_t));
    (void)netdev;

    *((gnrc_nettype_t *)value) = GNRC_NETTYPE_SIXLOWPAN;
    return sizeof(gnrc_nettype_t);
}

static int _get_netdev_max_packet_size(netdev_t *netdev, void *value,
                                       size_t max_len)
{
    expect(max_len == sizeof(uint16_t));
    (void)netdev;

    *((uint16_t *)value) = IEEE802154_MAX_FRAG_SIZE;
    return sizeof(uint16_t);
}

static int _get_netdev_src_len(netdev_t *netdev, void *value, size_t max_len)
{
    (void)netdev;
    expect(max_len == sizeof(uint16_t));
    *((uint16_t *)value) = sizeof(_ieee802154_local_eui64);
    return sizeof(uint16_t);
}

static int _get_netdev_addr_long(netdev_t *netdev, void *value, size_t max_len)
{
    (void)netdev;
    expect(max_len >= sizeof(_ieee802154_local_eui64));
    memcpy(value, _ieee802154_local_eui64, sizeof(_ieee802154_local_eui64));
    return sizeof(_ieee802154_local_eui64);
}

static void _init_interface(void)
{
    netdev_test_setup(&_ieee802154_dev, NULL);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_DEVICE_TYPE,
                           _get_netdev_device_type);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_PROTO,
                           _get_netdev_proto);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_MAX_PDU_SIZE,
                           _get_netdev_max_packet_size);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_SRC_LEN,
                           _get_netdev_src_len);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_ADDRESS_LONG,
                           _get_netdev_addr_long);
    gnrc_netif_ieee802154_create(&_netif,
            _netif_stack, THREAD_STACKSIZE_DEFAULT, GNRC_NETIF_PRIO,
            "dummy_netif", &_ieee802154_dev.netdev.netdev);
    ipv6_addr_t addr = IPV6_ADDR_UNSPECIFIED;

    /* fd01::01 */
    addr.u8[0] = 0xfd;
    addr.u8[1] = 0x01;
    addr.u8[15] = 0x01;

    xtimer_usleep(500); /* wait for thread to start */
    if (gnrc_netapi_set(_netif.pid, NETOPT_IPV6_ADDR, 64U << 8U, &addr,
                        sizeof(addr)) < 0) {
        printf("error: unable to add IPv6 address fd01::1/64 to interface %u\n",
               _netif.pid);
    }
}

static void _send_packet(void)
{
    gnrc_netif_t *netif = gnrc_netif_iter(NULL);

    struct {
        gnrc_netif_hdr_t netif_hdr;
        uint8_t src[8];
        uint8_t dst[8];
    } netif_hdr = {
        .src = IEEE802154_REMOTE_EUI64,
        .dst = IEEE802154_LOCAL_EUI64,
    };

    gnrc_netif_hdr_init(&(netif_hdr.netif_hdr), 8, 8);

    gnrc_netif_hdr_set_netif(&netif_hdr.netif_hdr, netif);

    uint8_t data1[] = {
        /* 6LoWPAN Header */
        /* Fragmentation Header (first) */
        0xc0, 0x94, /* 0b11000: frag1, 0b00010010100: datagram_size (148) */
        0x00, 0x01, /* datagram_tag */
        /* 0b011: LOWPAN_IPHC */
        /* 0b11: Traffic Class and Flow Label are elided */
        /* 0b1: Next Header is compressed */
        /* 0b11: The Hop Limit field is compressed and the hop limit is 255 */
        0x7f,
        /* 0b0: No additional 8-bit Context Identifier Extension is used */
        /* 0b0: Source address compression uses stateless compression */
        /* 0b11: source address mode is 0 bits */
        /* 0b0: Destination address is not a multicast address */
        /* 0x0: Destination address compression uses stateless compression */
        /* 0x00: destination address mode is 128 bits */
        0x30,

        /* destination address: fd01::1 */
        0xfd, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01,

        /* 0b11110: UDP LOWPAN_NHC */
        /* 0b0: Checksum is carried in-line */
        /* 0b11: First 12 bits of both Source Port and Destination Port are 0xf0b and elided */
        0xf3,
        0x00, /* Source Port and Destination Port (4 bits each) */
        0x23, 0x2f, /* Checksum */

        /* payload */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    uint8_t data2[] = {
        /* 6LoWPAN Header */
        /* Fragmentation Header (rest) */
        0xe0, 0x94, /* 0b11100: frag1, 0b00010010100: datagram_size (148) */
        0x00, 0x01, /* datagram_tag */
        0x0c,       /* datagram_offset (12 * 8 = 96) */

        /* payload */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    gnrc_netreg_entry_t dump_6lowpan = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL, gnrc_pktdump_pid);
    gnrc_netreg_entry_t dump_ipv6 = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL, gnrc_pktdump_pid);
    gnrc_netreg_entry_t dump_udp = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL, gnrc_pktdump_pid);
    gnrc_netreg_entry_t dump_udp_61616 = GNRC_NETREG_ENTRY_INIT_PID(61616, gnrc_pktdump_pid);

    gnrc_netreg_register(GNRC_NETTYPE_SIXLOWPAN, &dump_6lowpan);
    gnrc_netreg_register(GNRC_NETTYPE_IPV6, &dump_ipv6);
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &dump_udp);
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &dump_udp_61616);

    gnrc_pktsnip_t *netif1 = gnrc_pktbuf_add(NULL,
                                            &netif_hdr,
                                            sizeof(netif_hdr),
                                            GNRC_NETTYPE_NETIF);
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(netif1,
                                           data1,
                                           sizeof(data1),
                                           GNRC_NETTYPE_SIXLOWPAN);

    gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN, GNRC_NETREG_DEMUX_CTX_ALL, pkt1);

    gnrc_pktsnip_t *netif2 = gnrc_pktbuf_add(NULL,
                                             &netif_hdr,
                                             sizeof(netif_hdr),
                                             GNRC_NETTYPE_NETIF);
    gnrc_pktsnip_t *pkt2 = gnrc_pktbuf_add(netif2,
                                           data2,
                                           sizeof(data2),
                                           GNRC_NETTYPE_SIXLOWPAN);

    gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN, GNRC_NETREG_DEMUX_CTX_ALL, pkt2);
}

int main(void)
{
    puts("RIOT network stack example application");

    _init_interface();
    _send_packet();

    return 0;
}
