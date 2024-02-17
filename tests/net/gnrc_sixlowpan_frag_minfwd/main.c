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
 * @brief       Tests 6LoWPAN minimal forwarding
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "cib.h"
#include "common.h"
#include "embUnit.h"
#include "embUnit/embUnit.h"
#include "mutex.h"
#include "net/ipv6.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6/nib/nc.h"
#include "net/gnrc/ipv6/nib/ft.h"
#include "net/gnrc/sixlowpan/frag.h"
#include "net/gnrc/sixlowpan/frag/rb.h"
#include "net/gnrc/sixlowpan/frag/minfwd.h"
#include "net/gnrc/sixlowpan/iphc.h"
#include "net/netdev_test.h"
/* for debugging _target_buf */
#include "od.h"
#include "utlist.h"
#include "xtimer.h"

#include "pktbuf_static.h"

#define SEND_PACKET_TIMEOUT                         (500U)

#define LOC_L2  { _LL0, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 }
#define LOC_LL  { 0xfe,         0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                  _LL0 ^ 0x2,   _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 \
    }
#define LOC_GB  { 0x20,         0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e, \
                  _LL0 ^ 0x2,   _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 \
    }
#define REM_L2  { _LL0, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 + 1 }
#define REM_LL  { 0xfe,         0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                  _LL0 ^ 0x2,   _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 + 1 \
    }
#define REM_GB  { 0x20,         0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e, \
                  _LL0 ^ 0x2,   _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 + 1 \
    }

#define LOC_GB_PFX_LEN                              (64U)
#define REM_GB_PFX_LEN                              (64U)
#define TEST_1ST_FRAG_UNCOMP_SIZE                   (80U)
#define TEST_1ST_FRAG_UNCOMP_PAYLOAD_POS            (4U)
#define TEST_1ST_FRAG_UNCOMP_IPV6_HDR_POS           (5U)
#define TEST_1ST_FRAG_UNCOMP_IPV6_PAYLOAD_SIZE      (40U)
#define TEST_1ST_FRAG_UNCOMP_IPV6_PAYLOAD_POS       (45U)
#define TEST_1ST_FRAG_UNCOMP_UDP_PAYLOAD_SIZE       (32U)
#define TEST_1ST_FRAG_UNCOMP_UDP_PAYLOAD_POS        (53U)
#define TEST_1ST_FRAG_COMP_FRAG_SIZE                (80U)
#define TEST_1ST_FRAG_COMP_ONLY_IPHC_FRAG_SIZE      (48U)
#define TEST_1ST_FRAG_COMP_SIZE                     (38U)
#define TEST_1ST_FRAG_COMP_PAYLOAD_POS              (4U)
#define TEST_1ST_FRAG_COMP_EXP_OFFSET               (6U)
#define TEST_1ST_FRAG_COMP_PREV_HOP_UDP_PAYLOAD_POS (43U)
#define TEST_NTH_FRAG_SIZE                          (32U)
#define TEST_NTH_FRAG_OFFSET_POS                    (4U)
#define TEST_NTH_FRAG_PAYLOAD_POS                   (5U)
#define TEST_SEND_DATAGRAM_SIZE                     (148U)
#define TEST_SEND_DATAGRAM_TAG                      (0x22ddU)
#define TEST_SEND_LL_PAYLOAD_POS                    (4U)
#define TEST_SEND_LL_PAYLOAD_SIZE                   (7U)
#define TEST_SEND_FRAG1_PAYLOAD_POS                 (4U)
#define TEST_SEND_FRAG1_PAYLOAD_SIZE                (35U)
#define TEST_SEND_FRAG1_SIZE                        (40U)
#define TEST_SEND_FRAG2_OFFSET_POS                  (4U)
#define TEST_SEND_FRAG2_OFFSET                      (5U)
#define TEST_SEND_FRAG2_PAYLOAD_POS                 (5U)
#define TEST_SEND_FRAG2_PAYLOAD_SIZE                (96U)
#define TEST_SEND_FRAG3_OFFSET_POS                  (4U)
#define TEST_SEND_FRAG3_OFFSET                      (17U)
#define TEST_SEND_FRAG3_PAYLOAD_POS                 (5U)
#define TEST_SEND_FRAG3_PAYLOAD_SIZE                (12U)

enum {
    FIRST_FRAGMENT = 0,
    FIRST_FRAGMENT_REST,
    NTH_FRAGMENT
};

static const uint8_t _test_1st_frag_uncomp[] = {
        0xc4, 0xd0, /* 1st fragment | datagram size: 1232 */
        0x67, 0x9d, /* tag: 0x679d */
        0x41,       /* uncompressed IPv6 */
        /* IPv6 header: payload length = 1192,
         * next header = UDP (17), hop limit = 65 */
        0x60, 0x00, 0x00, 0x00, 0x04, 0xa8, 0x11, 0x41,
        /* Source: 2001:db8:d6c3:acf:dc71:2b85:82f:75fb */
        0x20, 0x01, 0x0d, 0xb8, 0xd6, 0xc3, 0x0a, 0xcf,
        0xdc, 0x71, 0x2b, 0x85, 0x08, 0x2f, 0x75, 0xfb,
        /* Destination: REM_GB */
        0x20, 0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e,
        _LL0 ^ 0x2, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 + 1,
        /* UDP source: 0xf0b4, UDP destination: 0xf0ba,
         * length: 1192, (random) checksum: 0x47b8 */
        0xf0, 0xb4, 0xf0, 0xba, 0x04, 0xa8, 0x47, 0xb8,
        /* (random) payload of length 32 */
        0xba, 0xb3, 0x6e, 0x4f, 0xd8, 0x23, 0x40, 0xf3,
        0xfb, 0xb9, 0x05, 0xbf, 0xbe, 0x19, 0xf6, 0xa2,
        0xc7, 0x6e, 0x09, 0xf9, 0xba, 0x70, 0x3a, 0x38,
        0xd5, 0x2f, 0x08, 0x85, 0xb8, 0xc1, 0x1a, 0x31,
    };
static const uint8_t _test_1st_frag_comp[] = {
        0xc4, 0xd0, /* 1st fragment | datagram size: 1232 */
        0x67, 0x9d, /* tag: 0x679d */
        /* IPHC: TF: 0b11, NH: 0b1 (NHC), HLIM: 0b10 (64), CID: 0b0,
         * Source: uncompressed (SAC: 0b0, SAM: 0b00),
         * Destination: uncompressed (M:0, DAC: 0b0, DAM: 0b00) */
        0x7e, 0x00,
        /* (uncompressed) Source: 2001:db8:d6c3:acf:dc71:2b85:82f:75fb */
        0x20, 0x01, 0x0d, 0xb8, 0xd6, 0xc3, 0x0a, 0xcf,
        0xdc, 0x71, 0x2b, 0x85, 0x08, 0x2f, 0x75, 0xfb,
        /* (uncompressed) Destination: REM_GB */
        0x20, 0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e,
        _LL0 ^ 0x2, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 + 1,
        /* NHC UDP: ports: 0b11 (12 bytes elided), (random) Checksum inline */
        0xf3, 0x4a, 0x47, 0xb8,
    };
static const uint8_t _test_1st_frag_comp_prev_hop[] = {
        0xc4, 0xd0, /* 1st fragment | datagram size: 1232 */
        0x67, 0x9d, /* tag: 0x679d */
        /* IPHC: TF: 0b11, NH: 0b1 (NHC), HLIM: 0b00 (inline), CID: 0b0,
         * Source: uncompressed (SAC: 0b0, SAM: 0b00),
         * Destination: uncompressed (M:0, DAC: 0b0, DAM: 0b00) */
        0x7c, 0x00,
        /* Hop Limit: 65 */
        0x41,
        /* (uncompressed) Source: 2001:db8:d6c3:acf:dc71:2b85:82f:75fb */
        0x20, 0x01, 0x0d, 0xb8, 0xd6, 0xc3, 0x0a, 0xcf,
        0xdc, 0x71, 0x2b, 0x85, 0x08, 0x2f, 0x75, 0xfb,
        /* (uncompressed) Destination: REM_GB */
        0x20, 0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e,
        _LL0 ^ 0x2, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 + 1,
        /* NHC UDP: ports: 0b11 (12 bytes elided), (random) Checksum inline */
        0xf3, 0x4a, 0x47, 0xb8,
        /* (random) payload of length 32 */
        0xba, 0xb3, 0x6e, 0x4f, 0xd8, 0x23, 0x40, 0xf3,
        0xfb, 0xb9, 0x05, 0xbf, 0xbe, 0x19, 0xf6, 0xa2,
        0xc7, 0x6e, 0x09, 0xf9, 0xba, 0x70, 0x3a, 0x38,
        0xd5, 0x2f, 0x08, 0x85, 0xb8, 0xc1, 0x1a, 0x31,
    };
static const uint8_t _test_nth_frag[] = {
        0xe4, 0xd0, /* n-th fragment | datagram size: 1232 */
        0x67, 0x9d, /* tag: 0x679d */
        0x96,       /* offset: 1200 (divided by 8) */
        /* payload of length 32 */
        0x54, 0x26, 0x63, 0xab, 0x31, 0x0b, 0xa4, 0x4e,
        0x6e, 0xa9, 0x09, 0x02, 0x15, 0xbb, 0x24, 0xa9,
        0x56, 0x44, 0x4a, 0x84, 0xd1, 0x83, 0xb9, 0xdb,
        0x0e, 0x0d, 0xd6, 0x6a, 0x83, 0x31, 0x1d, 0x94,
    };
static const uint8_t _test_send_ipv6[] = {
        /* IPv6 header: payload length = 108,
         * next header = ICMPv6 (58), hop limit = 64 */
        0x60, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x3a, 0x40,
        /* Source: LOC_GB */
        0x20, 0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e,
        _LL0 ^ 0x2, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7,
        /* Destination: REM_GB */
        0x20, 0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e,
        _LL0 ^ 0x2, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 + 1,
    };
static const uint8_t _test_send_icmpv6[] = {
        /* ICMPv6 Echo request (128), Code 0, (Random) checksum: 0x7269,
         * random identifier: 0x59be, random sequence number: 15804 */
        0x80, 0x00, 0x72, 0x69, 0x59, 0xbe, 0x3d, 0xbc,
        /* random payload */
        0x49, 0x19, 0xe8, 0x0b, 0x25, 0xbb, 0x00, 0x13,
        0x45, 0x85, 0xbd, 0x4a, 0xbb, 0xf1, 0x3d, 0xe3,
        0x36, 0xff, 0x52, 0xea, 0xe8, 0xec, 0xec, 0x82,
        0x94, 0x5f, 0xa4, 0x30, 0x1f, 0x46, 0x28, 0xc7,
        0x41, 0xff, 0x50, 0x84, 0x00, 0x41, 0xc7, 0x8d,
        0xb0, 0xdc, 0x18, 0xff, 0xcd, 0xfa, 0xa7, 0x72,
        0x4b, 0xcf, 0x7c, 0xf7, 0x7c, 0x8b, 0x65, 0x78,
        0xb0, 0xa8, 0xe7, 0x8f, 0xbc, 0x1e, 0xba, 0x4a,
        0x92, 0x13, 0x81, 0x5e, 0x23, 0xd1, 0xde, 0x09,
        0x84, 0x8a, 0xd0, 0xe2, 0xdd, 0x01, 0xc8, 0xd7,
        0x08, 0x4c, 0xd8, 0xc2, 0x21, 0x5c, 0x21, 0xb9,
        0x43, 0xea, 0x52, 0xbd, 0x6a, 0x9a, 0xac, 0x48,
        0x94, 0x98, 0xd1, 0x95
    };
static const uint8_t _test_send_ll[] = {
        0xc0, 0x94, /* 1st fragment | datagram size: TEST_SEND_DATAGRAM_SIZE */
        0x22, 0xdd, /* tag: TEST_SEND_DATAGRAM_TAG */
        /* IPHC: TF: 0b11, NH: 0b0 (inline), HLIM: 0b10 (64), CID: 0b0,
         * Source: uncompressed (SAC: 0b0, SAM: 0b11),
         * Destination: uncompressed (M:0, DAC: 0b0, DAM: 0b11) */
        0x7a, 0x33,
        /* Next header: ICMPv6 (58) */
        0x3a,
    };
static const uint8_t _test_send_frag1_prev_hop[] = {
        0xc0, 0x94, /* 1st fragment | datagram size: TEST_SEND_DATAGRAM_SIZE */
        0x67, 0x9d, /* tag: 0x679d */
        /* IPHC: TF: 0b11, NH: 0b0 (inline), HLIM: 0b00 (inline), CID: 0b0,
         * Source: uncompressed (SAC: 0b0, SAM: 0b00),
         * Destination: uncompressed (M:0, DAC: 0b0, DAM: 0b00) */
        0x78, 0x00,
        /* Next header: ICMPv6 (58), Hop Limit: 65 */
        0x3a, 0x41,
        /* (uncompressed) Source: LOC_GB */
        0x20, 0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e,
        _LL0 ^ 0x2, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7,
        /* (uncompressed) Destination: REM_GB */
        0x20, 0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e,
        _LL0 ^ 0x2, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 + 1,
    };
static const uint8_t _test_send_frag1[] = {
        0xc0, 0x94, /* 1st fragment | datagram size: TEST_SEND_DATAGRAM_SIZE */
        0x22, 0xdd, /* tag: TEST_SEND_DATAGRAM_TAG */
        /* IPHC: TF: 0b11, NH: 0b0 (inline), HLIM: 0b10 (64), CID: 0b0,
         * Source: uncompressed (SAC: 0b0, SAM: 0b00),
         * Destination: uncompressed (M:0, DAC: 0b0, DAM: 0b00) */
        0x7a, 0x00,
        /* Next header: ICMPv6 (58) */
        0x3a,
        /* (uncompressed) Source: LOC_GB */
        0x20, 0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e,
        _LL0 ^ 0x2, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7,
        /* (uncompressed) Destination: REM_GB */
        0x20, 0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e,
        _LL0 ^ 0x2, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 + 1,
    };
static const uint8_t _test_send_frag2[] = {
        0xe0, 0x94, /* nth fragment | datagram size: TEST_SEND_DATAGRAM_SIZE */
        0x22, 0xdd, /* tag: TEST_SEND_DATAGRAM_TAG */
        0x05,       /* offset: 40 (divided by 8) */
        0x80, 0x00, 0x72, 0x69, 0x59, 0xbe, 0x3d, 0xbc,
        0x49, 0x19, 0xe8, 0x0b, 0x25, 0xbb, 0x00, 0x13,
        0x45, 0x85, 0xbd, 0x4a, 0xbb, 0xf1, 0x3d, 0xe3,
        0x36, 0xff, 0x52, 0xea, 0xe8, 0xec, 0xec, 0x82,
        0x94, 0x5f, 0xa4, 0x30, 0x1f, 0x46, 0x28, 0xc7,
        0x41, 0xff, 0x50, 0x84, 0x00, 0x41, 0xc7, 0x8d,
        0xb0, 0xdc, 0x18, 0xff, 0xcd, 0xfa, 0xa7, 0x72,
        0x4b, 0xcf, 0x7c, 0xf7, 0x7c, 0x8b, 0x65, 0x78,
        0xb0, 0xa8, 0xe7, 0x8f, 0xbc, 0x1e, 0xba, 0x4a,
        0x92, 0x13, 0x81, 0x5e, 0x23, 0xd1, 0xde, 0x09,
        0x84, 0x8a, 0xd0, 0xe2, 0xdd, 0x01, 0xc8, 0xd7,
        0x08, 0x4c, 0xd8, 0xc2, 0x21, 0x5c, 0x21, 0xb9,
    };
static const uint8_t _test_send_frag3[] = {
        0xe0, 0x94, /* nth fragment | datagram size: TEST_ASSERT_EQUAL_INT */
        0x22, 0xdd, /* tag: TEST_SEND_DATAGRAM_TAG */
        0x11,       /* offset: 136 (divided by 8) */
        0x43, 0xea, 0x52, 0xbd, 0x6a, 0x9a, 0xac, 0x48,
        0x94, 0x98, 0xd1, 0x95
    };
static const ipv6_addr_t _rem_ll = { .u8 = REM_LL };
static const ipv6_addr_t _rem_gb = { .u8 = REM_GB };
static const uint8_t _rem_l2[] = REM_L2;
static const gnrc_sixlowpan_frag_rb_base_t _vrbe_base = {
        .src = { 0xde, 0x71, 0x2b, 0x85, 0x08, 0x2f, 0x75, 0xfb },
        .src_len = IEEE802154_LONG_ADDRESS_LEN,
        .dst = LOC_L2,
        .dst_len = IEEE802154_LONG_ADDRESS_LEN,
        .tag = 0x679d,
        .datagram_size = 1232U,
        .current_size = 0U,
    };
static uint8_t _target_buf[128U];
static uint8_t _target_buf_len;
/* to protect _target_buf and _target_buf_len */
/* to wait for new data in _target_buf */
static mutex_t _target_buf_filled = MUTEX_INIT_LOCKED;
static mutex_t _target_buf_barrier = MUTEX_INIT;

static gnrc_pktsnip_t *_create_ipv6_hdr(const ipv6_hdr_t *hdr);
static gnrc_pktsnip_t *_create_recv_frag(const void *frag_data,
                                         size_t frag_size);
static int _set_route_and_nce(const ipv6_addr_t *route, unsigned pfx_len);
static gnrc_pktsnip_t *_create_send_datagram(bool compressed, bool payload);
static size_t _wait_for_packet(size_t exp_size);
static void _check_vrbe_values(gnrc_sixlowpan_frag_vrb_t *vrbe,
                               size_t mhr_len, int frag_type);
static void _check_1st_frag_uncomp(size_t mhr_len, uint8_t exp_hl_diff);
static void _check_send_frag1(size_t mhr_len, bool check_tag);
static void _check_send_frag2(size_t mhr_len, bool check_tag);
static void _check_send_frag3(size_t mhr_len, bool check_tag);
static const gnrc_sixlowpan_frag_rb_t *_first_non_empty_rbuf(void);
static int _mock_netdev_send(netdev_t *dev, const iolist_t *iolist);

static void _set_up(void)
{
    /* reset data-structures */
    gnrc_sixlowpan_frag_rb_reset();
    gnrc_sixlowpan_frag_vrb_reset();
    gnrc_pktbuf_init();
    memset(_mock_netif->ipv6.addrs, 0, sizeof(_mock_netif->ipv6.addrs));
    memset(_mock_netif->ipv6.addrs_flags, 0,
           sizeof(_mock_netif->ipv6.addrs_flags));
    gnrc_ipv6_nib_init();
    gnrc_ipv6_nib_init_iface(_mock_netif);
    gnrc_ipv6_nib_iface_up(_mock_netif);
    /* re-init for syncing */
    mutex_init(&_target_buf_filled);
    mutex_lock(&_target_buf_filled);
    mutex_init(&_target_buf_barrier);
}

static void _tear_down(void)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test, NULL);
    mutex_unlock(&_target_buf_barrier);
    /* wait in case mutex in _mock_netdev_send was already entered */
    mutex_lock(&_target_buf_barrier);
    memset(_target_buf, 0, sizeof(_target_buf));
    _target_buf_len = 0;
    mutex_unlock(&_target_buf_barrier);
}

static void test_minfwd_vrbe_from_route__success__given_netif(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    static gnrc_pktsnip_t *ipv6_snip;
    static const ipv6_hdr_t ipv6_hdr = {
            .dst = { .u8 = REM_GB }
        };

    TEST_ASSERT_EQUAL_INT(0, _set_route_and_nce(&ipv6_hdr.dst, REM_GB_PFX_LEN));
    TEST_ASSERT_NOT_NULL((ipv6_snip = _create_ipv6_hdr(&ipv6_hdr)));
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_from_route(
            &_vrbe_base, _mock_netif, ipv6_snip)));
    gnrc_pktbuf_release(ipv6_snip);
    TEST_ASSERT_EQUAL_INT(_vrbe_base.current_size, vrbe->super.current_size);
    TEST_ASSERT(_mock_netif == vrbe->out_netif);
    TEST_ASSERT_EQUAL_INT(sizeof(_rem_l2), vrbe->super.dst_len);
    TEST_ASSERT_MESSAGE(memcmp(_rem_l2, vrbe->super.dst, sizeof(_rem_l2)) == 0,
                        "_rem_l2 != vrbe->super.dst");
}

static void test_minfwd_vrbe_from_route__success__no_netif(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    static gnrc_pktsnip_t *ipv6_snip;
    static const ipv6_hdr_t ipv6_hdr = {
            .dst = { .u8 = REM_GB }
        };

    TEST_ASSERT_EQUAL_INT(0, _set_route_and_nce(&ipv6_hdr.dst, REM_GB_PFX_LEN));
    TEST_ASSERT_NOT_NULL((ipv6_snip = _create_ipv6_hdr(&ipv6_hdr)));
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_from_route(
            &_vrbe_base, NULL, ipv6_snip)));
    gnrc_pktbuf_release(ipv6_snip);
    TEST_ASSERT_EQUAL_INT(_vrbe_base.current_size, vrbe->super.current_size);
    TEST_ASSERT(_mock_netif == vrbe->out_netif);
    TEST_ASSERT_EQUAL_INT(sizeof(_rem_l2), vrbe->super.dst_len);
    TEST_ASSERT_MESSAGE(memcmp(_rem_l2, vrbe->super.dst, sizeof(_rem_l2)) == 0,
                        "_rem_l2 != vrbe->super.dst");
}

static void test_minfwd_vrbe_from_route__no_route1(void)
{
    static gnrc_pktsnip_t *ipv6_snip;
    static const ipv6_hdr_t ipv6_hdr = {
            .dst = { .u8 = REM_GB }
        };

    TEST_ASSERT_NOT_NULL((ipv6_snip = _create_ipv6_hdr(&ipv6_hdr)));
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_from_route(
            &_vrbe_base, NULL, ipv6_snip));
    gnrc_pktbuf_release(ipv6_snip);
}

static void test_minfwd_vrbe_from_route__no_route2(void)
{
    static gnrc_pktsnip_t *snip;
    /* fantasy header */
    static const uint8_t hdr[] = {
            0x40, 0xa9, 0xf4, 0xde, 0x6c, 0x87, 0x50, 0x9a, 0x54, 0x1f,
            0x79, 0xde, 0x6e, 0xd2, 0xb0, 0x82, 0x5c, 0x16, 0xdc, 0xd7
        };

    TEST_ASSERT_NOT_NULL((snip = gnrc_pktbuf_add(NULL, hdr, sizeof(hdr),
                                                 GNRC_NETTYPE_TEST)));
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_from_route(&_vrbe_base, NULL, snip));
    gnrc_pktbuf_release(snip);
}

static void test_minfwd_vrbe_from_route__local_addr(void)
{
    static gnrc_pktsnip_t *ipv6_snip;
    static ipv6_hdr_t ipv6_hdr = {
            .dst = { .u8 = LOC_GB }
        };

    /* add address to interface */
    TEST_ASSERT_EQUAL_INT(
            sizeof(ipv6_addr_t),
            gnrc_netif_ipv6_addr_add(_mock_netif, &ipv6_hdr.dst,
                                     LOC_GB_PFX_LEN,
                                     GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID)
        );
    TEST_ASSERT_NOT_NULL((ipv6_snip = _create_ipv6_hdr(&ipv6_hdr)));
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_from_route(
            &_vrbe_base, NULL, ipv6_snip));
    gnrc_pktbuf_release(ipv6_snip);
}

static void test_minfwd_vrbe_from_route__vrb_full(void)
{
    static gnrc_pktsnip_t *ipv6_snip;
    static ipv6_hdr_t ipv6_hdr = {
            .dst = { .u8 = REM_GB }
        };
    gnrc_sixlowpan_frag_rb_base_t base = _vrbe_base;

    TEST_ASSERT_EQUAL_INT(0, _set_route_and_nce(&ipv6_hdr.dst, REM_GB_PFX_LEN));
    /* fill up VRB */
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_SIZE; i++) {
        base.arrival = xtimer_now_usec();
        TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_add(&base,
                                                         _mock_netif,
                                                         _rem_l2,
                                                         sizeof(_rem_l2)));
        base.tag++;
    }
    TEST_ASSERT_NOT_NULL((ipv6_snip = _create_ipv6_hdr(&ipv6_hdr)));
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_from_route(
            &base, NULL, ipv6_snip));
    gnrc_pktbuf_release(ipv6_snip);
}

static void test_minfwd_forward__success__1st_frag_sixlo(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe = gnrc_sixlowpan_frag_vrb_add(
            &_vrbe_base, _mock_netif, _rem_l2, sizeof(_rem_l2)
        );
    gnrc_pktsnip_t *pkt, *frag;
    size_t mhr_len;

    vrbe->super.arrival = xtimer_now_usec();
    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, _test_1st_frag_uncomp,
                                                sizeof(_test_1st_frag_uncomp),
                                                GNRC_NETTYPE_SIXLOWPAN)));
    /* separate fragment header from payload */
    TEST_ASSERT_NOT_NULL((frag = gnrc_pktbuf_mark(pkt, sizeof(sixlowpan_frag_t),
                                                  GNRC_NETTYPE_SIXLOWPAN)));
    LL_DELETE(pkt, frag);
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT_EQUAL_INT(0, gnrc_sixlowpan_frag_minfwd_forward(pkt,
                                                                frag->data,
                                                                vrbe,
                                                                0));
    gnrc_pktbuf_release(frag);  /* delete separated fragment header */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_1st_frag_uncomp))));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    _check_vrbe_values(vrbe, mhr_len, FIRST_FRAGMENT);
    TEST_ASSERT(_target_buf[0] & IEEE802154_FCF_FRAME_PEND);
    _check_1st_frag_uncomp(mhr_len, 0U);
    /* VRB entry should not have been removed */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
}

static void test_minfwd_forward__success__1st_frag_iphc(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe = gnrc_sixlowpan_frag_vrb_add(
            &_vrbe_base, _mock_netif, _rem_l2, sizeof(_rem_l2)
        );
    gnrc_pktsnip_t *pkt, *frag;
    size_t mhr_len;

    vrbe->super.arrival = xtimer_now_usec();
    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, _test_1st_frag_comp,
                                                sizeof(_test_1st_frag_comp),
                                                GNRC_NETTYPE_SIXLOWPAN)));
    /* separate fragment header from payload */
    TEST_ASSERT_NOT_NULL((frag = gnrc_pktbuf_mark(pkt, sizeof(sixlowpan_frag_t),
                                                  GNRC_NETTYPE_SIXLOWPAN)));
    LL_DELETE(pkt, frag);
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT_EQUAL_INT(0, gnrc_sixlowpan_frag_minfwd_forward(pkt,
                                                                frag->data,
                                                                vrbe,
                                                                0));
    gnrc_pktbuf_release(frag);  /* delete separated fragment header */
    /* first wait and check IPHC part (we put some slack in the first fragment) */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_1st_frag_comp))));
    _check_vrbe_values(vrbe, mhr_len, FIRST_FRAGMENT);
    TEST_ASSERT(_target_buf[0] & IEEE802154_FCF_FRAME_PEND);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_1st_frag_comp[TEST_1ST_FRAG_COMP_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_frag_t)],
                   sizeof(_test_1st_frag_comp) - sizeof(sixlowpan_frag_t)) == 0,
            "unexpected IPHC header"
        );
    /* VRB entry should not have been removed */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
}

static void test_minfwd_forward__success__nth_frag_incomplete(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe = gnrc_sixlowpan_frag_vrb_add(
            &_vrbe_base, _mock_netif, _rem_l2, sizeof(_rem_l2)
        );
    gnrc_pktsnip_t *pkt, *frag;
    size_t mhr_len;

    vrbe->super.arrival = xtimer_now_usec();
    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, _test_nth_frag,
                                                sizeof(_test_nth_frag),
                                                GNRC_NETTYPE_SIXLOWPAN)));
    /* separate fragment header from payload */
    TEST_ASSERT_NOT_NULL((frag = gnrc_pktbuf_mark(pkt,
                                                  sizeof(sixlowpan_frag_n_t),
                                                  GNRC_NETTYPE_SIXLOWPAN)));
    LL_DELETE(pkt, frag);
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT_EQUAL_INT(0, gnrc_sixlowpan_frag_minfwd_forward(pkt,
                                                                frag->data,
                                                                vrbe,
                                                                0));
    gnrc_pktbuf_release(frag);  /* delete separated fragment header */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_nth_frag))));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    _check_vrbe_values(vrbe, mhr_len, NTH_FRAGMENT);
    TEST_ASSERT(_target_buf[0] & IEEE802154_FCF_FRAME_PEND);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_nth_frag[TEST_NTH_FRAG_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_frag_n_t)],
                   TEST_NTH_FRAG_SIZE) == 0,
            "unexpected forwarded packet payload"
        );
    /* VRB entry should not have been removed */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
}

static void test_minfwd_forward__success__nth_frag_complete(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe = gnrc_sixlowpan_frag_vrb_add(
            &_vrbe_base, _mock_netif, _rem_l2, sizeof(_rem_l2)
        );
    gnrc_pktsnip_t *pkt, *frag;

    vrbe->super.arrival = xtimer_now_usec();
    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, _test_nth_frag,
                                                sizeof(_test_nth_frag),
                                                GNRC_NETTYPE_SIXLOWPAN)));
    /* separate fragment header from payload */
    TEST_ASSERT_NOT_NULL((frag = gnrc_pktbuf_mark(pkt,
                                                  sizeof(sixlowpan_frag_n_t),
                                                  GNRC_NETTYPE_SIXLOWPAN)));
    LL_DELETE(pkt, frag);
    /* simulate current_size only missing the created fragment */
    vrbe->super.current_size = _vrbe_base.datagram_size;
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT_EQUAL_INT(0, gnrc_sixlowpan_frag_minfwd_forward(pkt,
                                                                frag->data,
                                                                vrbe,
                                                                0));
    gnrc_pktbuf_release(frag);  /* delete separated fragment header */
    TEST_ASSERT(_wait_for_packet(sizeof(_test_nth_frag)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    TEST_ASSERT(!(_target_buf[0] & IEEE802154_FCF_FRAME_PEND));
    /* VRB entry should have been removed since
     * vrbe->super.current_size became vrbe->super.datagram_size */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                 _vrbe_base.src_len,
                                                 _vrbe_base.tag));
}

static void test_minfwd_forward__ENOMEM__netif_hdr_build_fail(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe = gnrc_sixlowpan_frag_vrb_add(
            &_vrbe_base, _mock_netif, _rem_l2, sizeof(_rem_l2)
        );
    gnrc_pktsnip_t *pkt, *frag, *filled_space;

    vrbe->super.arrival = xtimer_now_usec();

    size_t test_pkt_size   = _align(sizeof(gnrc_pktsnip_t)) + _align(sizeof(_test_nth_frag));
    size_t marked_pkt_size = _align(sizeof(gnrc_pktsnip_t)) + _align(sizeof(sixlowpan_frag_n_t))
                             + _align(sizeof(_test_nth_frag) - sizeof(sixlowpan_frag_n_t));

    /* Calculate the maximum payload size to fill the buffer with the following three packets */
    size_t dummy_pkt_payload_size = CONFIG_GNRC_PKTBUF_SIZE - _align(sizeof(gnrc_pktsnip_t))
                                    - test_pkt_size - marked_pkt_size;

    TEST_ASSERT_NOT_NULL((filled_space = gnrc_pktbuf_add(NULL, NULL,
                                                         dummy_pkt_payload_size,
                                                         GNRC_NETTYPE_UNDEF)));

    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, _test_nth_frag,
                                                sizeof(_test_nth_frag),
                                                GNRC_NETTYPE_SIXLOWPAN)));
    /* separate fragment header from payload */
    TEST_ASSERT_NOT_NULL((frag = gnrc_pktbuf_mark(pkt,
                                                  sizeof(sixlowpan_frag_n_t),
                                                  GNRC_NETTYPE_SIXLOWPAN)));
    LL_DELETE(pkt, frag);

    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_sixlowpan_frag_minfwd_forward(pkt,
                                                                      frag->data,
                                                                      vrbe,
                                                                      0));
    gnrc_pktbuf_release(frag);  /* delete separated fragment header */
    gnrc_pktbuf_release(filled_space);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_minfwd_frag_iphc__success(void)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    gnrc_pktsnip_t *pkt;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL((pkt = _create_send_datagram(true, true)));

    TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get()));
    fbuf->datagram_size = TEST_SEND_DATAGRAM_SIZE;
    fbuf->tag = TEST_SEND_DATAGRAM_TAG;

    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT_EQUAL_INT(
            0, gnrc_sixlowpan_frag_minfwd_frag_iphc(pkt,
                                                    TEST_SEND_DATAGRAM_SIZE,
                                                    &_rem_gb, fbuf)
        );
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag1))));
    _check_send_frag1(mhr_len, true);
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag2))));
    _check_send_frag2(mhr_len, true);
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag3))));
    _check_send_frag3(mhr_len, true);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_minfwd_frag_iphc__no_frag(void)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    gnrc_pktsnip_t *pkt, *netif;
    gnrc_netif_hdr_t *netif_hdr;

    pkt = gnrc_pktbuf_add(NULL,
                          &_test_send_frag1[TEST_SEND_FRAG1_PAYLOAD_POS],
                          TEST_SEND_FRAG1_PAYLOAD_SIZE,
                          GNRC_NETTYPE_SIXLOWPAN);
    TEST_ASSERT_NOT_NULL(pkt);
    netif = gnrc_netif_hdr_build(_vrbe_base.src, _vrbe_base.src_len,
                                 _vrbe_base.dst, _vrbe_base.dst_len);
    TEST_ASSERT_NOT_NULL(netif);
    netif_hdr = netif->data;
    TEST_ASSERT_NOT_NULL(netif_hdr);
    netif_hdr->if_pid = _mock_netif->pid;
    LL_PREPEND(pkt, netif);

    TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get()));
    fbuf->datagram_size = TEST_SEND_FRAG1_PAYLOAD_SIZE;
    fbuf->tag = TEST_SEND_DATAGRAM_TAG;

    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT_EQUAL_INT(
            -1, gnrc_sixlowpan_frag_minfwd_frag_iphc(pkt,
                                                     TEST_SEND_DATAGRAM_SIZE,
                                                     &_rem_gb, fbuf)
        );
    /* should time out (as the packet should be handled by normal fragmentation)
     * now */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag1)));
    TEST_ASSERT_NULL(fbuf->pkt);
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_minfwd_frag_iphc__ll_dst(void)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    gnrc_pktsnip_t *pkt, *netif;
    gnrc_netif_hdr_t *netif_hdr;

    pkt = gnrc_pktbuf_add(NULL,
                          &_test_send_ll[TEST_SEND_LL_PAYLOAD_POS],
                          TEST_SEND_LL_PAYLOAD_SIZE,
                          GNRC_NETTYPE_SIXLOWPAN);
    TEST_ASSERT_NOT_NULL(pkt);
    netif = gnrc_netif_hdr_build(_vrbe_base.src, _vrbe_base.src_len,
                                 _vrbe_base.dst, _vrbe_base.dst_len);
    TEST_ASSERT_NOT_NULL(netif);
    netif_hdr = netif->data;
    TEST_ASSERT_NOT_NULL(netif_hdr);
    netif_hdr->if_pid = _mock_netif->pid;
    LL_PREPEND(pkt, netif);

    TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get()));
    fbuf->datagram_size = TEST_SEND_FRAG1_PAYLOAD_SIZE;
    fbuf->tag = TEST_SEND_DATAGRAM_TAG;

    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT_EQUAL_INT(
            -1, gnrc_sixlowpan_frag_minfwd_frag_iphc(pkt,
                                                     TEST_SEND_DATAGRAM_SIZE,
                                                     &_rem_ll, fbuf)
        );
    /* should time out (as the packet should be handled by normal fragmentation)
     * now */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag1)));
    TEST_ASSERT_NULL(fbuf->pkt);
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv__1st_frag_uncomp(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_uncomp,
                                      sizeof(_test_1st_frag_uncomp)))
        );
    /* configure route to destination of IP header in frag */
    TEST_ASSERT_EQUAL_INT(0, _set_route_and_nce(&_rem_gb, REM_GB_PFX_LEN));
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_1st_frag_uncomp))));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* but there was a VRB entry created */
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        )));
    _check_vrbe_values(vrbe, mhr_len, FIRST_FRAGMENT);
    TEST_ASSERT_EQUAL_INT(TEST_1ST_FRAG_UNCOMP_SIZE,
                          vrbe->super.current_size);
    /* only the received fragment is registered */
    TEST_ASSERT_NOT_NULL(vrbe->super.ints);
    TEST_ASSERT_NULL(vrbe->super.ints->next);
    TEST_ASSERT(_target_buf[0] & IEEE802154_FCF_FRAME_PEND);
    _check_1st_frag_uncomp(mhr_len, 1U);
}

static void test_sixlo_recv__1st_frag_uncomp__no_route(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_uncomp,
                                      sizeof(_test_1st_frag_uncomp)))
        );
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_uncomp)));
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(_vrbe_base.datagram_size, rbuf->pkt->size);
    gnrc_pktbuf_release(rbuf->pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* This is normal reassembly so the rest should have be tested in the
     * test for normal reassembly ;-) */
}

static void test_sixlo_recv__1st_frag_uncomp__after_nth_frag(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    /* configure route to destination of IP header in frag */
    TEST_ASSERT_EQUAL_INT(0, _set_route_and_nce(&_rem_gb, REM_GB_PFX_LEN));
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));

    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_nth_frag)));
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL(_first_non_empty_rbuf());
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_uncomp,
                                      sizeof(_test_1st_frag_uncomp)))
        );
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_uncomp)));
    /* reassembly buffer entry should still exist */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB still remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(_vrbe_base.datagram_size, rbuf->pkt->size);
    gnrc_pktbuf_release(rbuf->pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* This is normal reassembly so the rest should have be tested in the
     * test for normal reassembly ;-) */
}

static void test_sixlo_recv__1st_frag_comp(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp_prev_hop,
                                      sizeof(_test_1st_frag_comp_prev_hop)))
        );
    /* configure route to destination of IP header in frag */
    TEST_ASSERT_EQUAL_INT(0, _set_route_and_nce(&_rem_gb, REM_GB_PFX_LEN));
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* first wait and check IPHC part (we put some slack in the first fragment) */
    TEST_ASSERT((mhr_len = _wait_for_packet(
            sizeof(_test_1st_frag_comp) + TEST_1ST_FRAG_UNCOMP_UDP_PAYLOAD_SIZE
        )));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* but there was a VRB entry created */
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        )));
    _check_vrbe_values(vrbe, mhr_len, FIRST_FRAGMENT);
    TEST_ASSERT_EQUAL_INT(TEST_1ST_FRAG_COMP_FRAG_SIZE,
                          vrbe->super.current_size);
    /* only the received fragment is registered */
    TEST_ASSERT_NOT_NULL(vrbe->super.ints);
    TEST_ASSERT_NULL(vrbe->super.ints->next);
    TEST_ASSERT(_target_buf[0] & IEEE802154_FCF_FRAME_PEND);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_1st_frag_comp[TEST_1ST_FRAG_COMP_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_frag_t)],
                   TEST_1ST_FRAG_COMP_SIZE) == 0,
            "unexpected IPHC header"
        );
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_1st_frag_uncomp[TEST_1ST_FRAG_UNCOMP_UDP_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(_test_1st_frag_comp)],
                   TEST_1ST_FRAG_UNCOMP_UDP_PAYLOAD_SIZE) == 0,
            "unexpected forwarded packet payload"
        );
}

static void test_sixlo_recv__1st_frag_comp__only_iphc(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp_prev_hop,
                                      TEST_1ST_FRAG_COMP_PREV_HOP_UDP_PAYLOAD_POS))
        );
    /* configure route to destination of IP header in frag */
    TEST_ASSERT_EQUAL_INT(0, _set_route_and_nce(&_rem_gb, REM_GB_PFX_LEN));
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* first wait and check IPHC part (we put some slack in the first fragment) */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_1st_frag_comp))));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* but there was a VRB entry created */
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        )));
    _check_vrbe_values(vrbe, mhr_len, FIRST_FRAGMENT);
    TEST_ASSERT_EQUAL_INT(TEST_1ST_FRAG_COMP_ONLY_IPHC_FRAG_SIZE,
                          vrbe->super.current_size);
    /* only the received fragment is registered */
    TEST_ASSERT_NOT_NULL(vrbe->super.ints);
    TEST_ASSERT_NULL(vrbe->super.ints->next);
    TEST_ASSERT(_target_buf[0] & IEEE802154_FCF_FRAME_PEND);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_1st_frag_comp[TEST_1ST_FRAG_COMP_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_frag_t)],
                   TEST_1ST_FRAG_COMP_SIZE) == 0,
            "unexpected IPHC header"
        );
}

static void test_sixlo_recv__1st_frag_comp__only_iphc_no_nhc(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_send_frag1_prev_hop,
                                      sizeof(_test_send_frag1_prev_hop)))
        );
    /* configure route to destination of IP header in frag */
    TEST_ASSERT_EQUAL_INT(0, _set_route_and_nce(&_rem_gb, REM_GB_PFX_LEN));
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* first wait and check IPHC part (we put some slack in the first fragment) */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag1))));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* but there was a VRB entry created */
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        )));
    _check_vrbe_values(vrbe, mhr_len, FIRST_FRAGMENT);
    TEST_ASSERT_EQUAL_INT(TEST_SEND_FRAG1_SIZE, vrbe->super.current_size);
    /* only the received fragment is registered */
    TEST_ASSERT_NOT_NULL(vrbe->super.ints);
    TEST_ASSERT_NULL(vrbe->super.ints->next);
    TEST_ASSERT(_target_buf[0] & IEEE802154_FCF_FRAME_PEND);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_send_frag1[TEST_SEND_FRAG1_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_frag_t)],
                   TEST_SEND_FRAG1_PAYLOAD_SIZE) == 0,
            "unexpected IPHC header"
        );
}

static void test_sixlo_recv__1st_frag_comp__no_route(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp_prev_hop,
                                      sizeof(_test_1st_frag_comp_prev_hop)))
        );
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_comp)));
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(_vrbe_base.datagram_size, rbuf->pkt->size);
    gnrc_pktbuf_release(rbuf->pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* This is normal reassembly so the rest should have be tested in the
     * test for normal reassembly ;-) */
}

static void test_sixlo_recv__1st_frag_comp__no_route_only_iphc(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp_prev_hop,
                                      TEST_1ST_FRAG_COMP_PREV_HOP_UDP_PAYLOAD_POS))
        );
    /* configure route to destination of IP header in frag */
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_comp)));
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(_vrbe_base.datagram_size, rbuf->pkt->size);
    gnrc_pktbuf_release(rbuf->pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* This is normal reassembly so the rest should have be tested in the
     * test for normal reassembly ;-) */
}

static void test_sixlo_recv__1st_frag_comp__no_refrag(void)
{
    gnrc_sixlowpan_frag_fb_t *reserved[CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE];
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp_prev_hop,
                                      sizeof(_test_1st_frag_comp_prev_hop)))
        );
    /* consume all available gnrc_sixlowpan_frag_fb_t instances so creating
     * a fragment with extra slack is not possible */
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE; i++) {
        reserved[i] = gnrc_sixlowpan_frag_fb_get();
        reserved[i]->pkt = frag;
    }
    /* configure route to destination of IP header in frag */
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_comp)));
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(_vrbe_base.datagram_size, rbuf->pkt->size);
    gnrc_pktbuf_release(rbuf->pkt);
    /* release all gnrc_sixlowpan_frag_fb_t instances again */
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE; i++) {
        reserved[i]->pkt = NULL;
    }
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* This is normal reassembly so the rest should have be tested in the
     * test for normal reassembly ;-) */
}

static void test_sixlo_recv__1st_frag_comp__after_nth_frag(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag,
                                      sizeof(_test_nth_frag)))
        );
    /* configure route to destination of IP header in frag */
    TEST_ASSERT_EQUAL_INT(0, _set_route_and_nce(&_rem_gb, REM_GB_PFX_LEN));
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));

    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_nth_frag)));
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL(_first_non_empty_rbuf());
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp_prev_hop,
                                      sizeof(_test_1st_frag_comp_prev_hop)))
        );
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_comp)));
    /* reassembly buffer entry should still exist */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB still remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(_vrbe_base.datagram_size, rbuf->pkt->size);
    gnrc_pktbuf_release(rbuf->pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* This is normal reassembly so the rest should have be tested in the
     * test for normal reassembly ;-) */
}

static void test_sixlo_recv__nth_frag(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    TEST_ASSERT_NOT_NULL(
            (vrbe = gnrc_sixlowpan_frag_vrb_add(&_vrbe_base, _mock_netif,
                                                _rem_l2, sizeof(_rem_l2)))
        );
    vrbe->super.arrival = xtimer_now_usec();
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_nth_frag))));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    _check_vrbe_values(vrbe, mhr_len, NTH_FRAGMENT);
    TEST_ASSERT_EQUAL_INT(TEST_NTH_FRAG_SIZE, vrbe->super.current_size);
    TEST_ASSERT(_target_buf[0] & IEEE802154_FCF_FRAME_PEND);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_nth_frag[TEST_NTH_FRAG_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_frag_n_t)],
                   TEST_NTH_FRAG_SIZE) == 0,
            "unexpected forwarded packet payload"
        );
    /* VRB entry should not have been removed */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
}

static void test_sixlo_recv__nth_frag__datagram_complete(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    TEST_ASSERT_NOT_NULL(
            (vrbe = gnrc_sixlowpan_frag_vrb_add(&_vrbe_base, _mock_netif,
                                                _rem_l2, sizeof(_rem_l2)))
        );
    /* simulate current_size only missing the created fragment */
    vrbe->super.current_size = _vrbe_base.datagram_size - TEST_NTH_FRAG_SIZE;
    vrbe->super.arrival = xtimer_now_usec();
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_nth_frag))));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* VRB entry should have been removed */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* VRB entry should have been removed, so don't check */
    TEST_ASSERT(!(_target_buf[0] & IEEE802154_FCF_FRAME_PEND));
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_nth_frag[TEST_NTH_FRAG_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_frag_n_t)],
                   TEST_NTH_FRAG_SIZE) == 0,
            "unexpected forwarded packet payload"
        );
}

static void test_sixlo_recv__nth_frag__no_vrbe(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_nth_frag)));
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(_vrbe_base.datagram_size, rbuf->pkt->size);
    gnrc_pktbuf_release(rbuf->pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* This is normal reassembly so the rest should have be tested in the
     * test for normal reassembly ;-) */
}

static void test_sixlo_recv__nth_frag__duplicate(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    uint16_t exp_current_size;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    TEST_ASSERT_NOT_NULL(
            (vrbe = gnrc_sixlowpan_frag_vrb_add(&_vrbe_base, _mock_netif,
                                                _rem_l2, sizeof(_rem_l2)))
        );
    vrbe->super.arrival = xtimer_now_usec();
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    TEST_ASSERT(_wait_for_packet(sizeof(_test_nth_frag)));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* VRB entry should not have been removed */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
    /* rest was already tested */
    exp_current_size = vrbe->super.current_size;

    /* generate and receive duplicate */
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    _target_buf_len = 0;
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_nth_frag)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* VRB entry should not have been removed */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
    TEST_ASSERT_EQUAL_INT(exp_current_size, vrbe->super.current_size);
}

static void test_sixlo_recv__nth_frag__overlap(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    sixlowpan_frag_n_t *frag_hdr;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    TEST_ASSERT_NOT_NULL(
            (vrbe = gnrc_sixlowpan_frag_vrb_add(&_vrbe_base, _mock_netif,
                                                _rem_l2, sizeof(_rem_l2)))
        );
    vrbe->super.arrival = xtimer_now_usec();
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    TEST_ASSERT(_wait_for_packet(sizeof(_test_nth_frag)));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* rest was already tested */

    /* generate and receive overlapping fragment */
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    frag_hdr = frag->data;
    /* move offset to simulate overlap*/
    frag_hdr->offset--;
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    _target_buf_len = 0;
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_nth_frag)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* VRB entry was removed due to overlap error */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
}

static void test_sixlo_send(void)
{
    gnrc_pktsnip_t *pkt;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL((pkt = _create_send_datagram(false, true)));

    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_send(GNRC_NETTYPE_SIXLOWPAN,
                                              GNRC_NETREG_DEMUX_CTX_ALL,
                                              pkt));
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag1))));
    /* tags are generated by the stack so don't check */
    _check_send_frag1(mhr_len, false);
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag2))));
    _check_send_frag2(mhr_len, false);
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag3))));
    _check_send_frag3(mhr_len, false);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static Test *tests_gnrc_sixlowpan_frag_minfwd_api(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_minfwd_vrbe_from_route__success__given_netif),
        new_TestFixture(test_minfwd_vrbe_from_route__success__no_netif),
        new_TestFixture(test_minfwd_vrbe_from_route__no_route1),
        new_TestFixture(test_minfwd_vrbe_from_route__no_route2),
        new_TestFixture(test_minfwd_vrbe_from_route__local_addr),
        new_TestFixture(test_minfwd_vrbe_from_route__vrb_full),
        new_TestFixture(test_minfwd_forward__success__1st_frag_sixlo),
        new_TestFixture(test_minfwd_forward__success__1st_frag_iphc),
        new_TestFixture(test_minfwd_forward__success__nth_frag_incomplete),
        new_TestFixture(test_minfwd_forward__success__nth_frag_complete),
        new_TestFixture(test_minfwd_forward__ENOMEM__netif_hdr_build_fail),
        new_TestFixture(test_minfwd_frag_iphc__success),
        new_TestFixture(test_minfwd_frag_iphc__no_frag),
        new_TestFixture(test_minfwd_frag_iphc__ll_dst),
    };

    EMB_UNIT_TESTCALLER(tests, _set_up, _tear_down, fixtures);

    return (Test *)&tests;
}

static Test *tests_gnrc_sixlowpan_frag_minfwd_integration(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_sixlo_recv__1st_frag_uncomp),
        new_TestFixture(test_sixlo_recv__1st_frag_uncomp__no_route),
        new_TestFixture(test_sixlo_recv__1st_frag_uncomp__after_nth_frag),
        new_TestFixture(test_sixlo_recv__1st_frag_comp),
        new_TestFixture(test_sixlo_recv__1st_frag_comp__only_iphc),
        new_TestFixture(test_sixlo_recv__1st_frag_comp__only_iphc_no_nhc),
        new_TestFixture(test_sixlo_recv__1st_frag_comp__no_route),
        new_TestFixture(test_sixlo_recv__1st_frag_comp__no_route_only_iphc),
        new_TestFixture(test_sixlo_recv__1st_frag_comp__no_refrag),
        new_TestFixture(test_sixlo_recv__1st_frag_comp__after_nth_frag),
        new_TestFixture(test_sixlo_recv__nth_frag),
        new_TestFixture(test_sixlo_recv__nth_frag__datagram_complete),
        new_TestFixture(test_sixlo_recv__nth_frag__no_vrbe),
        new_TestFixture(test_sixlo_recv__nth_frag__duplicate),
        new_TestFixture(test_sixlo_recv__nth_frag__overlap),
        new_TestFixture(test_sixlo_send),
    };

    EMB_UNIT_TESTCALLER(tests, _set_up, _tear_down, fixtures);

    return (Test *)&tests;
}

int main(void)
{
    _tests_init();

    TESTS_START();
    TESTS_RUN(tests_gnrc_sixlowpan_frag_minfwd_api());
    TESTS_RUN(tests_gnrc_sixlowpan_frag_minfwd_integration());
    TESTS_END();
    return 0;
}

static gnrc_pktsnip_t *_create_ipv6_hdr(const ipv6_hdr_t *hdr)
{
    return gnrc_pktbuf_add(NULL, hdr, sizeof(*hdr), GNRC_NETTYPE_IPV6);
}

static gnrc_pktsnip_t *_create_recv_frag(const void *frag_data,
                                         size_t frag_size)
{
    gnrc_pktsnip_t *netif;
    gnrc_netif_hdr_t *netif_hdr;

    netif = gnrc_netif_hdr_build(_vrbe_base.src, _vrbe_base.src_len,
                                 _vrbe_base.dst, _vrbe_base.dst_len);
    if (netif == NULL) {
        return NULL;
    }
    netif_hdr = netif->data;
    netif_hdr->if_pid = _mock_netif->pid;
    netif_hdr->flags = GNRC_NETIF_HDR_FLAGS_MORE_DATA;
    return gnrc_pktbuf_add(netif, frag_data, frag_size,
                           GNRC_NETTYPE_SIXLOWPAN);
}

static int _set_route_and_nce(const ipv6_addr_t *route, unsigned pfx_len)
{
    /* add neighbor cache entry */
    if (gnrc_ipv6_nib_nc_set(&_rem_ll, _mock_netif->pid,
                             _rem_l2, sizeof(_rem_l2)) < 0) {
        return -1;
    }
    /* and route to neighbor */
    if (gnrc_ipv6_nib_ft_add(route, pfx_len, &_rem_ll, _mock_netif->pid,
                             0) < 0) {
        return -1;
    }
    return 0;
}

static gnrc_pktsnip_t *_create_send_datagram(bool compressed, bool payload)
{
    gnrc_pktsnip_t *pkt1 = NULL, *pkt2;
    gnrc_netif_hdr_t *netif_hdr;

    if (payload) {
        pkt1 = gnrc_pktbuf_add(NULL, _test_send_icmpv6, sizeof(_test_send_icmpv6),
                               GNRC_NETTYPE_ICMPV6);
        if (pkt1 == NULL) {
            return NULL;
        }
    }
    if (compressed) {
        /* Use IPHC header from expected data */
        pkt2 = gnrc_pktbuf_add(pkt1,
                               &_test_send_frag1[TEST_SEND_FRAG1_PAYLOAD_POS],
                               TEST_SEND_FRAG1_PAYLOAD_SIZE,
                               GNRC_NETTYPE_SIXLOWPAN);
    }
    else {
        pkt2 = gnrc_pktbuf_add(pkt1, _test_send_ipv6, sizeof(_test_send_ipv6),
                               GNRC_NETTYPE_IPV6);
    }
    if (pkt2 == NULL) {
        return NULL;
    }
    pkt1 = gnrc_netif_hdr_build(_vrbe_base.src, _vrbe_base.src_len,
                                _vrbe_base.dst, _vrbe_base.dst_len);
    if (pkt1 == NULL) {
        return NULL;
    }
    netif_hdr = pkt1->data;
    if (netif_hdr == NULL) {
        return NULL;
    }
    netif_hdr->if_pid = _mock_netif->pid;
    LL_PREPEND(pkt2, pkt1);
    return pkt2;
}

static size_t _wait_for_packet(size_t exp_size)
{
    size_t mhr_len;

    xtimer_mutex_lock_timeout(&_target_buf_filled,
                              SEND_PACKET_TIMEOUT);
    while ((mhr_len = ieee802154_get_frame_hdr_len(_target_buf))) {
        if (IS_USED(MODULE_OD) && _target_buf_len > 0) {
            puts("Sent packet: ");
            od_hex_dump(_target_buf, _target_buf_len, OD_WIDTH_DEFAULT);
        }
        if (exp_size == (_target_buf_len - mhr_len)) {
            /* found expected packet */
            break;
        }
        /* let packets in again at the device */
        mutex_unlock(&_target_buf_barrier);
        /* wait for next packet */
        if (xtimer_mutex_lock_timeout(&_target_buf_filled,
                                      SEND_PACKET_TIMEOUT) < 0) {
            return 0;
        }
    }
    return mhr_len;
}

static void _check_vrbe_values(gnrc_sixlowpan_frag_vrb_t *vrbe,
                               size_t mhr_len, int frag_type)
{
    uint8_t target_buf_dst[IEEE802154_LONG_ADDRESS_LEN];
    sixlowpan_frag_t *frag_hdr = (sixlowpan_frag_t *)&_target_buf[mhr_len];
    le_uint16_t tmp;

    TEST_ASSERT_EQUAL_INT(vrbe->super.dst_len,
                          ieee802154_get_dst(_target_buf,
                                             target_buf_dst,
                                             &tmp));
    TEST_ASSERT_MESSAGE(memcmp(vrbe->super.dst, target_buf_dst,
                               vrbe->super.dst_len) == 0,
                        "vrbe->out_dst != target_buf_dst");

    TEST_ASSERT_EQUAL_INT(vrbe->super.datagram_size,
                          byteorder_ntohs(frag_hdr->disp_size) &
                          SIXLOWPAN_FRAG_SIZE_MASK);
    TEST_ASSERT_EQUAL_INT(vrbe->out_tag,
                          byteorder_ntohs(frag_hdr->tag));
    switch (frag_type) {
        case FIRST_FRAGMENT: {
            TEST_ASSERT_EQUAL_INT(
                    SIXLOWPAN_FRAG_1_DISP,
                    _target_buf[mhr_len] & SIXLOWPAN_FRAG_DISP_MASK
                );
            break;
        }
        case FIRST_FRAGMENT_REST:
        case NTH_FRAGMENT: {
            sixlowpan_frag_n_t *frag_n_hdr = (sixlowpan_frag_n_t *)&_target_buf[mhr_len];
            uint8_t exp_offset = (frag_type == FIRST_FRAGMENT_REST)
                               ? TEST_1ST_FRAG_COMP_EXP_OFFSET
                               : _test_nth_frag[TEST_NTH_FRAG_OFFSET_POS];

            TEST_ASSERT_EQUAL_INT(
                    SIXLOWPAN_FRAG_N_DISP,
                    _target_buf[mhr_len] & SIXLOWPAN_FRAG_DISP_MASK
                );
            TEST_ASSERT_EQUAL_INT(exp_offset, frag_n_hdr->offset);
            break;
        }
        default:
            TEST_ASSERT_MESSAGE(false, "Unexpected frag_type");
            break;
    }
}

static void _check_1st_frag_uncomp(size_t mhr_len, uint8_t exp_hl_diff)
{
    static const ipv6_hdr_t *exp_ipv6_hdr = (ipv6_hdr_t *)&_test_1st_frag_uncomp[
            TEST_1ST_FRAG_UNCOMP_IPV6_HDR_POS
        ];
    ipv6_hdr_t *ipv6_hdr;

    TEST_ASSERT_EQUAL_INT(
            SIXLOWPAN_UNCOMP,
            _target_buf[mhr_len + TEST_1ST_FRAG_UNCOMP_PAYLOAD_POS]
        );
    ipv6_hdr = (ipv6_hdr_t *)&_target_buf[
            mhr_len + TEST_1ST_FRAG_UNCOMP_IPV6_HDR_POS
        ];
    TEST_ASSERT_EQUAL_INT(exp_ipv6_hdr->v_tc_fl.u32, ipv6_hdr->v_tc_fl.u32);
    TEST_ASSERT_EQUAL_INT(exp_ipv6_hdr->len.u16, ipv6_hdr->len.u16);
    TEST_ASSERT_EQUAL_INT(exp_ipv6_hdr->nh, ipv6_hdr->nh);
    /* hop-limit shall be decremented by 1 */
    TEST_ASSERT_EQUAL_INT(exp_ipv6_hdr->hl - exp_hl_diff, ipv6_hdr->hl);
    TEST_ASSERT(ipv6_addr_equal(&exp_ipv6_hdr->src, &ipv6_hdr->src));
    TEST_ASSERT(ipv6_addr_equal(&exp_ipv6_hdr->dst, &ipv6_hdr->dst));
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_1st_frag_uncomp[TEST_1ST_FRAG_UNCOMP_IPV6_PAYLOAD_POS],
                   ipv6_hdr + 1, TEST_1ST_FRAG_UNCOMP_IPV6_PAYLOAD_SIZE) == 0,
            "unexpected forwarded packet payload"
        );
}

static void _check_send_frag_datagram_fields(size_t mhr_len, bool check_tag)
{
    sixlowpan_frag_t *frag_hdr = (sixlowpan_frag_t *)&_target_buf[mhr_len];

    TEST_ASSERT_EQUAL_INT(TEST_SEND_DATAGRAM_SIZE,
                          byteorder_ntohs(frag_hdr->disp_size) &
                          SIXLOWPAN_FRAG_SIZE_MASK);
    if (check_tag) {
        TEST_ASSERT_EQUAL_INT(TEST_SEND_DATAGRAM_TAG,
                              byteorder_ntohs(frag_hdr->tag));
    }
}

static void _check_send_frag1(size_t mhr_len, bool check_tag)
{
    TEST_ASSERT_EQUAL_INT(
            SIXLOWPAN_FRAG_1_DISP,
            _target_buf[mhr_len] & SIXLOWPAN_FRAG_DISP_MASK
        );
    _check_send_frag_datagram_fields(mhr_len, check_tag);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_send_frag1[TEST_SEND_FRAG1_PAYLOAD_POS],
                   &_target_buf[TEST_SEND_FRAG1_PAYLOAD_POS + mhr_len],
                   TEST_SEND_FRAG1_PAYLOAD_SIZE) == 0,
            "unexpected IPHC header"
        );
}

static void _check_send_frag2(size_t mhr_len, bool check_tag)
{
    sixlowpan_frag_n_t *frag_hdr;

    TEST_ASSERT_EQUAL_INT(
            SIXLOWPAN_FRAG_N_DISP,
            _target_buf[mhr_len] & SIXLOWPAN_FRAG_DISP_MASK
        );
    frag_hdr = (sixlowpan_frag_n_t *)&_target_buf[mhr_len];
    _check_send_frag_datagram_fields(mhr_len, check_tag);
    TEST_ASSERT_EQUAL_INT(TEST_SEND_FRAG2_OFFSET,
                          frag_hdr->offset);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_send_frag2[TEST_SEND_FRAG2_PAYLOAD_POS],
                   &_target_buf[TEST_SEND_FRAG2_PAYLOAD_POS + mhr_len],
                   TEST_SEND_FRAG2_PAYLOAD_SIZE) == 0,
            "unexpected send packet payload"
        );
}

static void _check_send_frag3(size_t mhr_len, bool check_tag)
{
    sixlowpan_frag_n_t *frag_hdr;

    TEST_ASSERT_EQUAL_INT(
            SIXLOWPAN_FRAG_N_DISP,
            _target_buf[mhr_len] & SIXLOWPAN_FRAG_DISP_MASK
        );
    frag_hdr = (sixlowpan_frag_n_t *)&_target_buf[mhr_len];
    _check_send_frag_datagram_fields(mhr_len, check_tag);
    TEST_ASSERT_EQUAL_INT(TEST_SEND_FRAG3_OFFSET,
                          frag_hdr->offset);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_send_frag3[TEST_SEND_FRAG3_PAYLOAD_POS],
                   &_target_buf[TEST_SEND_FRAG3_PAYLOAD_POS + mhr_len],
                   TEST_SEND_FRAG3_PAYLOAD_SIZE) == 0,
            "unexpected send packet payload"
        );
}

static const gnrc_sixlowpan_frag_rb_t *_first_non_empty_rbuf(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf = gnrc_sixlowpan_frag_rb_array();

    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE; i++) {
        if (!gnrc_sixlowpan_frag_rb_entry_empty(&rbuf[i])) {
            return rbuf;
        }
    }
    return NULL;
}

static int _mock_netdev_send(netdev_t *dev, const iolist_t *iolist)
{
    (void)dev;
    mutex_lock(&_target_buf_barrier);
    _target_buf_len = 0;
    for (const iolist_t *ptr = iolist; ptr != NULL; ptr = ptr->iol_next) {
        if ((_target_buf_len + iolist->iol_len) > sizeof(_target_buf)) {
            return -ENOBUFS;
        }
        memcpy(&_target_buf[_target_buf_len], ptr->iol_base, ptr->iol_len);
        _target_buf_len += ptr->iol_len;
    }
    /* wake-up test thread */
    mutex_unlock(&_target_buf_filled);
    return _target_buf_len;
}
