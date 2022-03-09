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
#include "net/gnrc/sixlowpan/frag/sfr.h"
#include "net/gnrc/sixlowpan/iphc.h"
#include "net/netdev_test.h"
#ifdef MODULE_OD
/* for debugging _target_buf */
#include "od.h"
#endif
#include "utlist.h"
#include "xtimer.h"

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
#define TEST_1ST_FRAG_UNCOMP_PAYLOAD_POS            (6U)
#define TEST_1ST_FRAG_UNCOMP_IPV6_HDR_POS           (7U)
#define TEST_1ST_FRAG_UNCOMP_IPV6_PAYLOAD_SIZE      (40U)
#define TEST_1ST_FRAG_UNCOMP_IPV6_PAYLOAD_POS       (47U)
#define TEST_1ST_FRAG_UNCOMP_UDP_PAYLOAD_SIZE       (32U)
#define TEST_1ST_FRAG_UNCOMP_UDP_PAYLOAD_POS        (55U)
#define TEST_1ST_FRAG_COMP_FRAG_SIZE                (80U)
#define TEST_1ST_FRAG_COMP_ONLY_IPHC_FRAG_SIZE      (48U)
#define TEST_1ST_FRAG_COMP_SIZE                     (38U)
#define TEST_1ST_FRAG_COMP_PAYLOAD_POS              (6U)
#define TEST_1ST_FRAG_COMP_UDP_PAYLOAD_POS          (44U)
#define TEST_1ST_FRAG_COMP_PREV_HOP_UDP_PAYLOAD_POS (45U)
#define TEST_NTH_FRAG_SIZE                          (32U)
#define TEST_NTH_FRAG_PAYLOAD_POS                   (6U)
#define TEST_FRAG_TAG                               (0xADU)
#define TEST_SEND_COMP_DATAGRAM_SIZE                (193U)
#define TEST_SEND_DATAGRAM_TAG                      (0x25U)
#define TEST_SEND_FRAG1_PAYLOAD_POS                 (6U)
#define TEST_SEND_FRAG1_ICMPV6_PAYLOAD_POS          (41U)
#define TEST_SEND_FRAG1_PAYLOAD_SIZE                (35U)
#define TEST_OFFSET_DIFF                            (6U)

static const uint8_t _test_1st_frag_uncomp[] = {
        0xe8,           /* RFRAG | no ECN */
        TEST_FRAG_TAG,  /* tag: TEST_FRAG_TAG */
        0x00, 0x51, /* no ACK REQ | sequence: 0 | fragment_size: 81 */
        0x04, 0xd1, /* compressed datagram size: 1233 */
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
        0xe8,           /* RFRAG | no ECN */
        TEST_FRAG_TAG,  /* tag: TEST_FRAG_TAG */
        0x00, 0x46, /* no ACK REQ | sequence: 0 | fragment_size: 70 */
        0x04, 0xc6, /* compressed datagram size: 1222 */
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
        /* (random) payload of length 32 */
        0xba, 0xb3, 0x6e, 0x4f, 0xd8, 0x23, 0x40, 0xf3,
        0xfb, 0xb9, 0x05, 0xbf, 0xbe, 0x19, 0xf6, 0xa2,
        0xc7, 0x6e, 0x09, 0xf9, 0xba, 0x70, 0x3a, 0x38,
        0xd5, 0x2f, 0x08, 0x85, 0xb8, 0xc1, 0x1a, 0x31,
    };
static const uint8_t _test_1st_frag_comp_prev_hop[] = {
        0xe8,           /* RFRAG | no ECN */
        TEST_FRAG_TAG,  /* tag: TEST_FRAG_TAG */
        0x00, 0x47, /* no ACK REQ | sequence: 0 | fragment_size: 71 */
        0x04, 0xc7, /* compressed datagram size: 1223 */
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
static const uint8_t _test_abort_frag[] = {
        0xe8,           /* RFRAG | no ECN */
        TEST_FRAG_TAG,  /* tag: TEST_FRAG_TAG */
        0x00, 0x00, /* no ACK REQ | sequence: 0 | fragment_size: 0 */
        0x00, 0x00, /* offset: 0 */
    };
static const uint8_t _test_nth_frag[] = {
        0xe8,           /* RFRAG | no ECN */
        TEST_FRAG_TAG,  /* tag: TEST_FRAG_TAG */
        0x06, 0x20, /* no ACK REQ | sequence: 6 | fragment_size: 32 */
        0x04, 0xb0, /* offset: 1200 */
        /* payload of length 32 */
        0x54, 0x26, 0x63, 0xab, 0x31, 0x0b, 0xa4, 0x4e,
        0x6e, 0xa9, 0x09, 0x02, 0x15, 0xbb, 0x24, 0xa9,
        0x56, 0x44, 0x4a, 0x84, 0xd1, 0x83, 0xb9, 0xdb,
        0x0e, 0x0d, 0xd6, 0x6a, 0x83, 0x31, 0x1d, 0x94,
    };
static const uint8_t _test_ack[] = {
        0xea,       /* RFRAG-ACK | no ECN */
        0xf1,       /* tag: 0xf1 */
        /* randomly set bitmap */
        0xbb, 0x6d, 0x5d, 0x94
    };
static const uint8_t _test_send_ipv6[] = {
        /* IPv6 header: payload length = 158,
         * next header = ICMPv6 (58), hop limit = 64 */
        0x60, 0x00, 0x00, 0x00, 0x00, 0x9e, 0x3a, 0x40,
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
        0x94, 0x98, 0xd1, 0x95, 0x6a, 0x0e, 0x10, 0xf9,
        0x2d, 0xe3, 0x53, 0xe4, 0x84, 0xb0, 0x8a, 0x92,
        0xaa, 0xe0, 0x5a, 0x63, 0x8b, 0x7d, 0x17, 0x51,
        0x22, 0x58, 0xa5, 0x6e, 0x87, 0x18, 0x32, 0x46,
        0x91, 0xd0, 0x59, 0xda, 0xc4, 0x9b, 0xa9, 0xde,
        0x20, 0xf4, 0xc8, 0xc4, 0xef, 0x1d, 0x9e, 0x13,
        0x6c, 0x28, 0x16, 0x59, 0xcc, 0x06
    };
static const uint8_t _test_send_frag1_prev_hop[] = {
        0xe8,       /* RFRAG | no ECN */
        TEST_SEND_DATAGRAM_TAG, /* tag: TEST_SEND_DATAGRAM_TAG */
        0x00, 0x24, /* no ACK REQ | sequence: 0 | fragment_size: 36 */
        /* compressed datagram size: 144 */
        0x00, TEST_SEND_COMP_DATAGRAM_SIZE + 1,
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
        0xe8,       /* RFRAG | no ECN */
        TEST_SEND_DATAGRAM_TAG, /* tag: TEST_SEND_DATAGRAM_TAG */
        0x00, 0x5b, /* no ACK REQ | sequence: 0 | fragment_size: 91 */
        /* compressed datagram size: 143 */
        0x00, TEST_SEND_COMP_DATAGRAM_SIZE,
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
        0x80, 0x00, 0x72, 0x69, 0x59, 0xbe, 0x3d, 0xbc,
        0x49, 0x19, 0xe8, 0x0b, 0x25, 0xbb, 0x00, 0x13,
        0x45, 0x85, 0xbd, 0x4a, 0xbb, 0xf1, 0x3d, 0xe3,
        0x36, 0xff, 0x52, 0xea, 0xe8, 0xec, 0xec, 0x82,
        0x94, 0x5f, 0xa4, 0x30, 0x1f, 0x46, 0x28, 0xc7,
        0x41, 0xff, 0x50, 0x84, 0x00, 0x41, 0xc7, 0x8d,
        0xb0, 0xdc, 0x18, 0xff, 0xcd, 0xfa, 0xa7, 0x72,
    };
static const uint8_t _test_send_frag2[] = {
        0xe8,       /* RFRAG | no ECN */
        TEST_SEND_DATAGRAM_TAG, /* tag: TEST_SEND_DATAGRAM_TAG */
        0x04, 0x60, /* no ACK REQ | sequence: 1 | fragment_size: 96 */
        0x00, 0x5b, /* offset: 91 */
        0x4b, 0xcf, 0x7c, 0xf7, 0x7c, 0x8b, 0x65, 0x78,
        0xb0, 0xa8, 0xe7, 0x8f, 0xbc, 0x1e, 0xba, 0x4a,
        0x92, 0x13, 0x81, 0x5e, 0x23, 0xd1, 0xde, 0x09,
        0x84, 0x8a, 0xd0, 0xe2, 0xdd, 0x01, 0xc8, 0xd7,
        0x08, 0x4c, 0xd8, 0xc2, 0x21, 0x5c, 0x21, 0xb9,
        0x43, 0xea, 0x52, 0xbd, 0x6a, 0x9a, 0xac, 0x48,
        0x94, 0x98, 0xd1, 0x95, 0x6a, 0x0e, 0x10, 0xf9,
        0x2d, 0xe3, 0x53, 0xe4, 0x84, 0xb0, 0x8a, 0x92,
        0xaa, 0xe0, 0x5a, 0x63, 0x8b, 0x7d, 0x17, 0x51,
        0x22, 0x58, 0xa5, 0x6e, 0x87, 0x18, 0x32, 0x46,
        0x91, 0xd0, 0x59, 0xda, 0xc4, 0x9b, 0xa9, 0xde,
        0x20, 0xf4, 0xc8, 0xc4, 0xef, 0x1d, 0x9e, 0x13,
    };
static const uint8_t _test_send_frag3[] = {
        0xe8,       /* RFRAG | no ECN */
        TEST_SEND_DATAGRAM_TAG, /* tag: TEST_SEND_DATAGRAM_TAG */
        0x08, 0x06, /* no ACK REQ | sequence: 2 | fragment_size: 6 */
        0x00, 0xbb, /* offset: 187 */
        0x6c, 0x28, 0x16, 0x59, 0xcc, 0x06
    };
static const uint8_t _loc_l2[] = LOC_L2;
static const ipv6_addr_t _rem_ll = { .u8 = REM_LL };
static const ipv6_addr_t _rem_gb = { .u8 = REM_GB };
static const uint8_t _rem_l2[] = REM_L2;
static const gnrc_sixlowpan_frag_rb_base_t _vrbe_base = {
        .src = { 0xde, 0x71, 0x2b, 0x85, 0x08, 0x2f, 0x75, 0xfb },
        .src_len = IEEE802154_LONG_ADDRESS_LEN,
        .dst = LOC_L2,
        .dst_len = sizeof(_loc_l2),
        .tag = TEST_FRAG_TAG,
        .datagram_size = 1232U,
        .current_size = 0U,
    };
static uint8_t _target_buf[128U];
static uint8_t _target_buf_len;
/* to protect _target_buf and _target_buf_len */
/* to wait for new data in _target_buf */
static mutex_t _target_buf_filled = MUTEX_INIT_LOCKED;
static mutex_t _target_buf_barrier = MUTEX_INIT;
uint32_t _last_sent_frame;

static gnrc_pktsnip_t *_create_recv_frag(const void *frag_data,
                                         size_t frag_size);
static gnrc_pktsnip_t *_create_recv_ack(const void *ack_data,
                                        size_t ack_size);
static int _set_route_and_nce(const ipv6_addr_t *route, unsigned pfx_len);
static int _add_dst(const ipv6_addr_t *dst, unsigned pfx_len);
static gnrc_pktsnip_t *_create_send_datagram(bool compressed, bool payload);
static size_t _wait_for_packet(size_t exp_size);
static inline void _wait_arq_timeout(gnrc_sixlowpan_frag_fb_t *fb);
static void _check_vrbe_values(gnrc_sixlowpan_frag_vrb_t *vrbe,
                               size_t mhr_len, bool check_offset_diff,
                               int16_t exp_offset_diff);
static void _check_ack(size_t mhr_len, uint8_t exp_tag,
                       const uint8_t *exp_bitmap);
static void _check_abort_ack(size_t mhr_len, uint8_t exp_tag);
static void _check_1st_frag_uncomp(size_t mhr_len, uint8_t exp_hl_diff);
static void _check_send_frag1(size_t mhr_len, bool ack_req);
static void _check_send_frag2(size_t mhr_len, bool ack_req);
static void _check_send_frag3(size_t mhr_len, bool ack_req);
static const gnrc_sixlowpan_frag_rb_t *_first_non_empty_rbuf(void);
static int _mock_netdev_send(netdev_t *dev, const iolist_t *iolist);

static void _set_up(void)
{
    /* reset data-structures */
    _last_sent_frame = xtimer_now_usec() - CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US;
    gnrc_sixlowpan_frag_rb_reset();
    gnrc_sixlowpan_frag_vrb_reset();
    gnrc_pktbuf_init();
    memset(_mock_netif->ipv6.addrs, 0, sizeof(_mock_netif->ipv6.addrs));
    memset(_mock_netif->ipv6.addrs_flags, 0,
           sizeof(_mock_netif->ipv6.addrs_flags));
    gnrc_ipv6_nib_init();
    gnrc_ipv6_nib_init_iface(_mock_netif);
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

static void test_sfr_forward__success__1st_frag_sixlo(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe = gnrc_sixlowpan_frag_vrb_add(
            &_vrbe_base, _mock_netif, _rem_l2, sizeof(_rem_l2)
        );
    gnrc_pktsnip_t *pkt, *frag;
    size_t mhr_len;

    vrbe->in_netif = _mock_netif;
    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, _test_1st_frag_uncomp,
                                                sizeof(_test_1st_frag_uncomp),
                                                GNRC_NETTYPE_SIXLOWPAN)));
    /* separate fragment header from payload */
    TEST_ASSERT_NOT_NULL((frag = gnrc_pktbuf_mark(pkt, sizeof(sixlowpan_sfr_rfrag_t),
                                                  GNRC_NETTYPE_SIXLOWPAN)));
    LL_DELETE(pkt, frag);
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT_EQUAL_INT(0, gnrc_sixlowpan_frag_sfr_forward(pkt,
                                                                frag->data,
                                                                vrbe,
                                                                0));
    gnrc_pktbuf_release(frag);  /* delete separated fragment header */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_1st_frag_uncomp))));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    _check_vrbe_values(vrbe, mhr_len, false, 0);
    _check_1st_frag_uncomp(mhr_len, 0U);
    /* VRB entry should not have been removed */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
}

static void test_sfr_forward__success__1st_frag_iphc(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe = gnrc_sixlowpan_frag_vrb_add(
            &_vrbe_base, _mock_netif, _rem_l2, sizeof(_rem_l2)
        );
    gnrc_pktsnip_t *pkt, *frag;
    size_t mhr_len;

    vrbe->in_netif = _mock_netif;
    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, _test_1st_frag_comp,
                                                sizeof(_test_1st_frag_comp),
                                                GNRC_NETTYPE_SIXLOWPAN)));
    /* separate fragment header from payload */
    TEST_ASSERT_NOT_NULL((frag = gnrc_pktbuf_mark(pkt, sizeof(sixlowpan_sfr_rfrag_t),
                                                  GNRC_NETTYPE_SIXLOWPAN)));
    LL_DELETE(pkt, frag);
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT_EQUAL_INT(0, gnrc_sixlowpan_frag_sfr_forward(pkt,
                                                                frag->data,
                                                                vrbe,
                                                                0));
    gnrc_pktbuf_release(frag);  /* delete separated fragment header */
    /* first wait and check IPHC part (we put some slack in the first fragment) */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_1st_frag_comp))));
    _check_vrbe_values(vrbe, mhr_len, false, 0);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_1st_frag_comp[TEST_1ST_FRAG_COMP_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_sfr_rfrag_t)],
                   sizeof(_test_1st_frag_comp) - sizeof(sixlowpan_sfr_rfrag_t)) == 0,
            "unexpected payload"
        );
    /* VRB entry should not have been removed */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
}

static void test_sfr_forward__success__nth_frag_incomplete(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe = gnrc_sixlowpan_frag_vrb_add(
            &_vrbe_base, _mock_netif, _rem_l2, sizeof(_rem_l2)
        );
    gnrc_pktsnip_t *pkt, *frag;
    size_t mhr_len;

    vrbe->in_netif = _mock_netif;
    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, _test_nth_frag,
                                                sizeof(_test_nth_frag),
                                                GNRC_NETTYPE_SIXLOWPAN)));
    /* separate fragment header from payload */
    TEST_ASSERT_NOT_NULL((frag = gnrc_pktbuf_mark(pkt,
                                                  sizeof(sixlowpan_sfr_rfrag_t),
                                                  GNRC_NETTYPE_SIXLOWPAN)));
    LL_DELETE(pkt, frag);
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT_EQUAL_INT(0, gnrc_sixlowpan_frag_sfr_forward(pkt,
                                                                frag->data,
                                                                vrbe,
                                                                0));
    gnrc_pktbuf_release(frag);  /* delete separated fragment header */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_nth_frag))));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    _check_vrbe_values(vrbe, mhr_len, false, 0);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_nth_frag[TEST_NTH_FRAG_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_sfr_rfrag_t)],
                   TEST_NTH_FRAG_SIZE) == 0,
            "unexpected forwarded packet payload"
        );
    /* VRB entry should not have been removed */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
}

static void test_sfr_forward__success__nth_frag_complete(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe = gnrc_sixlowpan_frag_vrb_add(
            &_vrbe_base, _mock_netif, _rem_l2, sizeof(_rem_l2)
        );
    gnrc_pktsnip_t *pkt, *frag;

    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, _test_nth_frag,
                                                sizeof(_test_nth_frag),
                                                GNRC_NETTYPE_SIXLOWPAN)));
    /* separate fragment header from payload */
    TEST_ASSERT_NOT_NULL((frag = gnrc_pktbuf_mark(pkt,
                                                  sizeof(sixlowpan_sfr_rfrag_t),
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
    TEST_ASSERT_EQUAL_INT(0, gnrc_sixlowpan_frag_sfr_forward(pkt,
                                                                frag->data,
                                                                vrbe,
                                                                0));
    gnrc_pktbuf_release(frag);  /* delete separated fragment header */
    TEST_ASSERT(_wait_for_packet(sizeof(_test_nth_frag)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* VRB entry still exists, as with SFR it is removed by ACK or timeout */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
}

static void test_sfr_forward__ENOMEM__netif_hdr_build_fail(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe = gnrc_sixlowpan_frag_vrb_add(
            &_vrbe_base, _mock_netif, _rem_l2, sizeof(_rem_l2)
        );
    gnrc_pktsnip_t *pkt, *frag, *filled_space;

    TEST_ASSERT_NOT_NULL((filled_space = gnrc_pktbuf_add(
            NULL, NULL,
            /* 115U == 2 * sizeof(gnrc_pktsnip_t) + movement due to mark */
            CONFIG_GNRC_PKTBUF_SIZE - sizeof(_test_nth_frag) - 115U,
            GNRC_NETTYPE_UNDEF
        )));
    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, _test_nth_frag,
                                                sizeof(_test_nth_frag),
                                                GNRC_NETTYPE_SIXLOWPAN)));
    /* separate fragment header from payload */
    TEST_ASSERT_NOT_NULL((frag = gnrc_pktbuf_mark(pkt,
                                                  sizeof(sixlowpan_sfr_rfrag_t),
                                                  GNRC_NETTYPE_SIXLOWPAN)));
    LL_DELETE(pkt, frag);

    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_sixlowpan_frag_sfr_forward(pkt,
                                                                      frag->data,
                                                                      vrbe,
                                                                      0));
    gnrc_pktbuf_release(frag);  /* delete separated fragment header */
    gnrc_pktbuf_release(filled_space);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__1st_frag_uncomp(void)
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
    _check_vrbe_values(vrbe, mhr_len, true, 0);
    _check_1st_frag_uncomp(mhr_len, 1U);
}

static void test_sixlo_recv_fwd__1st_frag_uncomp__req_ack(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_uncomp,
                                      sizeof(_test_1st_frag_uncomp)))
        );
    /* set ACK Req flag in RFRAG header */
    sixlowpan_sfr_rfrag_set_ack_req(frag->data);
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
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* but there was a VRB entry created */
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        )));
    _check_vrbe_values(vrbe, mhr_len, true, 0);
    _check_1st_frag_uncomp(mhr_len, 1U);
    /* should time out as only the reassembling endpoint is supposed to send
     * an abort ACK or if no VRB exists on a forwarding node. See
     * - https://tools.ietf.org/html/rfc8931#section-6.1.2
     * - https://tools.ietf.org/html/rfc8931#section-6.3
     */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_comp)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__1st_frag_uncomp__no_route(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_uncomp,
                                      sizeof(_test_1st_frag_uncomp)))
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
}

static void test_sixlo_recv_fwd__1st_frag_uncomp__after_nth_frag(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

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

    /* expect abort ACK, see
     * https://tools.ietf.org/html/rfc8931#section-6.1.2 */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_nth_frag))));
    /* no reassembly buffer entry */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    _check_abort_ack(mhr_len, _vrbe_base.tag);
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_uncomp,
                                      sizeof(_test_1st_frag_uncomp)))
        );
    _target_buf_len = 0;
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
    _check_vrbe_values(vrbe, mhr_len, true, 0);
    _check_1st_frag_uncomp(mhr_len, 1U);
}

static void test_sixlo_recv_fwd__1st_frag_comp(void)
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
    /* wait for recompressed fragment (hop-limit is now compressed) */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_1st_frag_comp))));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* but there was a VRB entry created */
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        )));
    _check_vrbe_values(vrbe, mhr_len, true,
                       (int16_t)sizeof(_test_1st_frag_comp) -
                       (int16_t)sizeof(_test_1st_frag_comp_prev_hop));
    TEST_ASSERT_EQUAL_INT(TEST_1ST_FRAG_COMP_FRAG_SIZE,
                          vrbe->super.current_size);
    /* frag_size changed to the size of the expected fragment */
    TEST_ASSERT_EQUAL_INT(
            sixlowpan_sfr_rfrag_get_frag_size(
                    (sixlowpan_sfr_rfrag_t *)_test_1st_frag_comp
                ),
            sixlowpan_sfr_rfrag_get_frag_size(
                    (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len]
                )
        );
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_1st_frag_comp[TEST_1ST_FRAG_COMP_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_sfr_rfrag_t)],
                   TEST_1ST_FRAG_COMP_SIZE) == 0,
            "unexpected IPHC header"
        );
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_1st_frag_comp_prev_hop[TEST_1ST_FRAG_COMP_PREV_HOP_UDP_PAYLOAD_POS],
                   &_target_buf[mhr_len + TEST_1ST_FRAG_COMP_UDP_PAYLOAD_POS],
                   TEST_1ST_FRAG_UNCOMP_UDP_PAYLOAD_SIZE) == 0,
            "unexpected forwarded packet payload"
        );
}

/* a regression test to check for the bugs fixed in
 * https://github.com/RIOT-OS/RIOT/pull/12848 */
static void test_sixlo_recv_fwd__1st_frag_comp__resend(void)
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
    /* wait for recompressed fragment (hop-limit is now compressed) */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_1st_frag_comp))));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* but there was a VRB entry created */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
    /* receive fragment for a second time (e.g. resent) */
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp_prev_hop,
                                      sizeof(_test_1st_frag_comp_prev_hop)))
        );
    _target_buf_len = 0;
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* wait for recompressed fragment (hop-limit is now compressed) */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_1st_frag_comp))));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    gnrc_pktbuf_stats();
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB entry still exists */
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        )));
    /* and if removed ... */
    gnrc_sixlowpan_frag_vrb_rm(vrbe);
    /* the fragment interval pool is empty again */
    TEST_ASSERT(gnrc_sixlowpan_frag_rb_ints_empty());
}

static void test_sixlo_recv_fwd__1st_frag_comp__only_iphc(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp_prev_hop,
                                      TEST_1ST_FRAG_COMP_PREV_HOP_UDP_PAYLOAD_POS))
        );
    sixlowpan_sfr_rfrag_set_frag_size(frag->data,
                                      TEST_1ST_FRAG_COMP_PREV_HOP_UDP_PAYLOAD_POS -
                                      sizeof(sixlowpan_sfr_rfrag_t));
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
    /* wait for recompressed fragment (hop-limit is now compressed) */
    TEST_ASSERT((mhr_len = _wait_for_packet(TEST_1ST_FRAG_COMP_UDP_PAYLOAD_POS)));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* but there was a VRB entry created */
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        )));
    _check_vrbe_values(vrbe, mhr_len, true,
                       (int16_t)TEST_1ST_FRAG_COMP_UDP_PAYLOAD_POS -
                       (int16_t)TEST_1ST_FRAG_COMP_PREV_HOP_UDP_PAYLOAD_POS);
    TEST_ASSERT_EQUAL_INT(TEST_1ST_FRAG_COMP_ONLY_IPHC_FRAG_SIZE,
                          vrbe->super.current_size);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_1st_frag_comp[TEST_1ST_FRAG_COMP_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_sfr_rfrag_t)],
                   TEST_1ST_FRAG_COMP_SIZE) == 0,
            "unexpected IPHC header"
        );
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__1st_frag_comp__only_iphc_no_nhc(void)
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
    /* wait for recompressed fragment (hop-limit is now compressed) */
    TEST_ASSERT((mhr_len = _wait_for_packet(TEST_SEND_FRAG1_ICMPV6_PAYLOAD_POS)));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* but there was a VRB entry created */
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, TEST_SEND_DATAGRAM_TAG
        )));
    _check_vrbe_values(vrbe, mhr_len, true,
                       (int16_t)TEST_SEND_FRAG1_ICMPV6_PAYLOAD_POS -
                       (int16_t)sizeof(_test_send_frag1_prev_hop));
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_send_frag1[TEST_SEND_FRAG1_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_sfr_rfrag_t)],
                   TEST_SEND_FRAG1_PAYLOAD_SIZE) == 0,
            "unexpected IPHC header"
        );
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__1st_frag_comp__no_route(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp_prev_hop,
                                      sizeof(_test_1st_frag_comp_prev_hop)))
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
}

static void test_sixlo_recv_fwd__1st_frag_comp__no_route_only_iphc(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp_prev_hop,
                                      TEST_1ST_FRAG_COMP_PREV_HOP_UDP_PAYLOAD_POS))
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
}

static void test_sixlo_recv_fwd__1st_frag_comp__no_refrag(void)
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

static void test_sixlo_recv_fwd__1st_frag_comp__after_nth_frag(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

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

    /* expect abort ACK, see
     * https://tools.ietf.org/html/rfc8931#section-6.1.2 */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_nth_frag))));
    /* no reassembly buffer entry */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    _check_abort_ack(mhr_len, _vrbe_base.tag);
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp_prev_hop,
                                      sizeof(_test_1st_frag_comp_prev_hop)))
        );
    _target_buf_len = 0;
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* wait for recompressed fragment (hop-limit is now compressed) */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_1st_frag_comp))));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* but there was a VRB entry created */
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        )));
    _check_vrbe_values(vrbe, mhr_len, true,
                       (int16_t)sizeof(_test_1st_frag_comp) -
                       (int16_t)sizeof(_test_1st_frag_comp_prev_hop));
    TEST_ASSERT_EQUAL_INT(TEST_1ST_FRAG_COMP_FRAG_SIZE,
                          vrbe->super.current_size);
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_1st_frag_comp[TEST_1ST_FRAG_COMP_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_sfr_rfrag_t)],
                   TEST_1ST_FRAG_COMP_SIZE) == 0,
            "unexpected IPHC header"
        );
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_1st_frag_comp_prev_hop[TEST_1ST_FRAG_COMP_PREV_HOP_UDP_PAYLOAD_POS],
                   &_target_buf[mhr_len + TEST_1ST_FRAG_COMP_UDP_PAYLOAD_POS],
                   TEST_1ST_FRAG_UNCOMP_UDP_PAYLOAD_SIZE) == 0,
            "unexpected forwarded packet payload"
        );
}

static void test_sixlo_recv_fwd__1st_frag_abort(void)
{
    uint8_t target_buf_dst[IEEE802154_LONG_ADDRESS_LEN];
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    sixlowpan_sfr_rfrag_t *rfrag_hdr;
    size_t mhr_len;
    le_uint16_t tmp;
    uint8_t exp_tag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_abort_frag,
                                      sizeof(_test_abort_frag)))
        );
    TEST_ASSERT_NOT_NULL(
            (vrbe = gnrc_sixlowpan_frag_vrb_add(&_vrbe_base, _mock_netif,
                                                _rem_l2, sizeof(_rem_l2)))
        );
    vrbe->in_netif = _mock_netif;
    exp_tag = vrbe->out_tag & 0xff;
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
    /* should be forwarded */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_abort_frag))));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB entry was deleted */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(sizeof(_rem_l2), ieee802154_get_dst(_target_buf,
                                                              target_buf_dst,
                                                              &tmp));
    TEST_ASSERT_MESSAGE(memcmp(_rem_l2, target_buf_dst, sizeof(_rem_l2)) == 0,
                        "Unexpected destination address");
    rfrag_hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT(sixlowpan_sfr_rfrag_is(&rfrag_hdr->base));
    TEST_ASSERT_EQUAL_INT(exp_tag, rfrag_hdr->base.tag);
    TEST_ASSERT(!sixlowpan_sfr_rfrag_ack_req(rfrag_hdr));
    TEST_ASSERT_EQUAL_INT(0, sixlowpan_sfr_rfrag_get_seq(rfrag_hdr));
    TEST_ASSERT_EQUAL_INT(0, sixlowpan_sfr_rfrag_get_frag_size(rfrag_hdr));
    TEST_ASSERT_EQUAL_INT(0, sixlowpan_sfr_rfrag_get_offset(rfrag_hdr));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__1st_frag_nalp(void)
{
    gnrc_pktsnip_t *frag;
    uint8_t *data;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp,
                                      sizeof(sixlowpan_sfr_rfrag_t) + 1))
        );
    data = frag->data;
    /* mark dispatch after RFRAG header a non-6LoWPAN frame */
    data[sizeof(sixlowpan_sfr_rfrag_t)] &= ~(0xc0);
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
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_comp)));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB entry was deleted */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__1st_frag_nalp__req_ack(void)
{
    gnrc_pktsnip_t *frag;
    uint8_t *data;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp,
                                      sizeof(sixlowpan_sfr_rfrag_t) + 1))
        );
    data = frag->data;
    /* mark dispatch after RFRAG header a non-6LoWPAN frame */
    data[sizeof(sixlowpan_sfr_rfrag_t)] &= ~(0xc0);
    /* set ACK request flag */
    sixlowpan_sfr_rfrag_set_ack_req(frag->data);
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
    /* should time out as only the reassembling endpoint is supposed to send
     * an abort ACK or if no VRB exists on a forwarding node. See
     * - https://tools.ietf.org/html/rfc8931#section-6.1.2
     * - https://tools.ietf.org/html/rfc8931#section-6.3
     */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_comp)));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB entry was deleted */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__1st_frag_nalp__no_vrb(void)
{
    gnrc_pktsnip_t *frag;
    uint8_t *data;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp,
                                      sizeof(sixlowpan_sfr_rfrag_t) + 1))
        );
    data = frag->data;
    /* mark dispatch after RFRAG header a non-6LoWPAN frame */
    data[sizeof(sixlowpan_sfr_rfrag_t)] &= ~(0xc0);
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
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_comp)));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB remains entry */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__nth_frag(void)
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
    vrbe->in_netif = _mock_netif;
    /* set offset_diff to test it in the outgoing RFRAG */
    vrbe->offset_diff = TEST_OFFSET_DIFF;
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
    /* and VRB entry still exist */
    TEST_ASSERT_NOT_NULL((vrbe = gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        )));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    _check_vrbe_values(vrbe, mhr_len, false, 0);
    /* offset changed according to offset_diff */
    TEST_ASSERT_EQUAL_INT(
            sixlowpan_sfr_rfrag_get_offset((sixlowpan_sfr_rfrag_t *)&_test_nth_frag) +
                                           TEST_OFFSET_DIFF,
            sixlowpan_sfr_rfrag_get_offset((sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len])
        );
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_nth_frag[TEST_NTH_FRAG_PAYLOAD_POS],
                   &_target_buf[mhr_len + sizeof(sixlowpan_sfr_rfrag_t)],
                   TEST_NTH_FRAG_SIZE) == 0,
            "unexpected forwarded packet payload"
        );
}

static void test_sixlo_recv_fwd__nth_frag__no_vrbe(void)
{
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

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
    /* expect abort ACK, see
     * https://tools.ietf.org/html/rfc8931#section-6.1.2 */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_nth_frag))));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB too */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    _check_abort_ack(mhr_len, _vrbe_base.tag);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());

}

static void test_sixlo_recv_fwd__nth_frag__duplicate(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    TEST_ASSERT_NOT_NULL(
            (vrbe = gnrc_sixlowpan_frag_vrb_add(&_vrbe_base, _mock_netif,
                                                _rem_l2, sizeof(_rem_l2)))
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
    TEST_ASSERT(_wait_for_packet(sizeof(_test_nth_frag)));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* VRB entry should not have been removed */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));

    /* generate and receive duplicate */
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* with SFR the forwarder does not care about duplicates and just forwards
     * them as well */
    _target_buf_len = 0;
    TEST_ASSERT(_wait_for_packet(sizeof(_test_nth_frag)));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* VRB entry should not have been removed */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
}

static void test_sixlo_recv_fwd__nth_frag__overlap(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *frag;
    sixlowpan_sfr_rfrag_t *frag_hdr;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    TEST_ASSERT_NOT_NULL(
            (vrbe = gnrc_sixlowpan_frag_vrb_add(&_vrbe_base, _mock_netif,
                                                _rem_l2, sizeof(_rem_l2)))
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
    sixlowpan_sfr_rfrag_set_offset(frag_hdr,
                                   sixlowpan_sfr_rfrag_get_offset(frag_hdr) - 1);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    _target_buf_len = 0;
    /* should time out */
    TEST_ASSERT(_wait_for_packet(sizeof(_test_nth_frag)));
    /* with SFR the forwarder does not care about overlapping fragments and just
     * forwards them as well */
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* VRB entry should not have been removed */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(_vrbe_base.src,
                                                     _vrbe_base.src_len,
                                                     _vrbe_base.tag));
    /* rest was already tested */
}

static void test_sixlo_recv_fwd__frag__too_short(void)
{
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp,
                                      sizeof(sixlowpan_sfr_rfrag_t) - 1))
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
    /* should be forwarded */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_abort_frag)));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__ack(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *ack;
    sixlowpan_sfr_ack_t *ack_hdr;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (ack = _create_recv_ack(_test_ack, sizeof(_test_ack)))
        );
    TEST_ASSERT_NOT_NULL(
            (vrbe = gnrc_sixlowpan_frag_vrb_add(&_vrbe_base, _mock_netif,
                                                _rem_l2, sizeof(_rem_l2)))
        );
    vrbe->in_netif = _mock_netif;
    ack_hdr = ack->data;
    ack_hdr->base.tag = vrbe->out_tag;
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 ack));
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_ack))));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB entry still exist */
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    _check_ack(mhr_len, _vrbe_base.tag, &_test_ack[2]);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__NULL_ack(void)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *ack;
    sixlowpan_sfr_ack_t *ack_hdr;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (ack = _create_recv_ack(_test_ack, sizeof(_test_ack)))
        );
    TEST_ASSERT_NOT_NULL(
            (vrbe = gnrc_sixlowpan_frag_vrb_add(&_vrbe_base, _mock_netif,
                                                _rem_l2, sizeof(_rem_l2)))
        );
    vrbe->in_netif = _mock_netif;
    ack_hdr = ack->data;
    ack_hdr->base.tag = vrbe->out_tag;
    /* set ACK bitmap to NULL */
    memset(ack_hdr->bitmap, 0, sizeof(ack_hdr->bitmap));
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 ack));
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_ack))));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB entry was removed */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    /* NULL bitmap == abort ACK */
    _check_abort_ack(mhr_len, _vrbe_base.tag);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__FULL_ack(void)
{
    const uint8_t full_bitmap[] = { 0xff, 0xff, 0xff, 0xff };
    gnrc_sixlowpan_frag_vrb_t *vrbe;
    gnrc_pktsnip_t *ack;
    sixlowpan_sfr_ack_t *ack_hdr;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (ack = _create_recv_ack(_test_ack, sizeof(_test_ack)))
        );
    TEST_ASSERT_NOT_NULL(
            (vrbe = gnrc_sixlowpan_frag_vrb_add(&_vrbe_base, _mock_netif,
                                                _rem_l2, sizeof(_rem_l2)))
        );
    vrbe->in_netif = _mock_netif;
    ack_hdr = ack->data;
    ack_hdr->base.tag = vrbe->out_tag;
    /* set ACK bitmap to NULL */
    memcpy(ack_hdr->bitmap, full_bitmap, sizeof(ack_hdr->bitmap));
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 ack));
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_ack))));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB entry was removed */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    _check_ack(mhr_len, _vrbe_base.tag, full_bitmap);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__ack__no_vrbe(void)
{
    gnrc_pktsnip_t *ack;

    TEST_ASSERT_NOT_NULL(
            (ack = _create_recv_ack(_test_ack, sizeof(_test_ack)))
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
                                                 ack));
    /*
     * https://tools.ietf.org/html/rfc8931#section-6.2
     * >    If the Reverse LSP is not found, the router MUST silently drop the
     * >    RFRAG-ACK message.
     */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_ack)));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_fwd__ack__too_short(void)
{
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_ack,
                                      sizeof(sixlowpan_sfr_ack_t) - 1))
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
    /* should be forwarded */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_abort_frag)));
    /* reassembly buffer remains empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_ep__1st_frag_uncomp(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_uncomp,
                                      sizeof(_test_1st_frag_uncomp)))
        );
    /* configure interface to be endpoint of route */
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_addr_t),
                          _add_dst(&_rem_gb, REM_GB_PFX_LEN));
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
    /* shouldn't trigger any sending */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_uncomp)));
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(_vrbe_base.datagram_size, rbuf->pkt->size);
    /* check if IPv6 packet is started in reassembly buffer */
    TEST_ASSERT_EQUAL_INT(0x60, ((uint8_t *)rbuf->pkt->data)[0] & 0xf0);
    /* check `received` bitmap (used for ACKs) */
    TEST_ASSERT_EQUAL_INT(0x80, rbuf->received[0]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[1]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[2]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[3]);
    gnrc_pktbuf_release(rbuf->pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_ep__1st_frag_uncomp__req_ack(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;
    BITFIELD(exp_bitmap, 32U) = { 0 };

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_uncomp,
                                      sizeof(_test_1st_frag_uncomp)))
        );
    /* set ACK Req flag in RFRAG header */
    sixlowpan_sfr_rfrag_set_ack_req(frag->data);
    /* configure interface to be endpoint of route */
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_addr_t),
                          _add_dst(&_rem_gb, REM_GB_PFX_LEN));
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
    /* expect RFRAG-ACK */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(sixlowpan_sfr_ack_t))));
    /* only sequence number 0 (first fragment) should be ACK'd */
    bf_set(exp_bitmap, 0);
    _check_ack(mhr_len, _vrbe_base.tag, exp_bitmap);
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(_vrbe_base.datagram_size, rbuf->pkt->size);
    /* check if IPv6 packet is started in reassembly buffer */
    TEST_ASSERT_EQUAL_INT(0x60, ((uint8_t *)rbuf->pkt->data)[0] & 0xf0);
    /* check `received` bitmap (used for ACKs) */
    TEST_ASSERT_EQUAL_INT(0x80, rbuf->received[0]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[1]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[2]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[3]);
    gnrc_pktbuf_release(rbuf->pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_ep__1st_frag_uncomp__after_nth_frag(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    /* configure interface to be endpoint of route */
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_addr_t),
                          _add_dst(&_rem_gb, REM_GB_PFX_LEN));
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

    /* expect abort ACK, see
     * https://tools.ietf.org/html/rfc8931#section-6.1.2 */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_nth_frag))));
    /* no reassembly buffer entry */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    _check_abort_ack(mhr_len, _vrbe_base.tag);
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_uncomp,
                                      sizeof(_test_1st_frag_uncomp)))
        );
    _target_buf_len = 0;
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* shouldn't trigger any sending */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_uncomp)));
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(_vrbe_base.datagram_size, rbuf->pkt->size);
    /* check if IPv6 packet is started in reassembly buffer */
    TEST_ASSERT_EQUAL_INT(0x60, ((uint8_t *)rbuf->pkt->data)[0] & 0xf0);
    /* check `received` bitmap (used for ACKs) */
    TEST_ASSERT_EQUAL_INT(0x80, rbuf->received[0]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[1]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[2]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[3]);
    gnrc_pktbuf_release(rbuf->pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_ep__1st_frag_comp(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp,
                                      sizeof(_test_1st_frag_comp)))
        );
    /* configure interface to be endpoint of route */
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_addr_t),
                          _add_dst(&_rem_gb, REM_GB_PFX_LEN));
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
    /* shouldn't trigger any sending */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_comp)));
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(_vrbe_base.datagram_size, rbuf->pkt->size);
    /* check if IPv6 packet is started in reassembly buffer */
    TEST_ASSERT_EQUAL_INT(0x60, ((uint8_t *)rbuf->pkt->data)[0] & 0xf0);
    /* check `received` bitmap (used for ACKs) */
    TEST_ASSERT_EQUAL_INT(0x80, rbuf->received[0]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[1]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[2]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[3]);
    gnrc_pktbuf_release(rbuf->pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_ep__1st_frag_comp__after_nth_frag(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_nth_frag, sizeof(_test_nth_frag)))
        );
    /* configure interface to be endpoint of route */
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_addr_t),
                          _add_dst(&_rem_gb, REM_GB_PFX_LEN));
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

    /* expect abort ACK, see
     * https://tools.ietf.org/html/rfc8931#section-6.1.2 */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_nth_frag))));
    /* no reassembly buffer entry */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    _check_abort_ack(mhr_len, _vrbe_base.tag);
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp,
                                      sizeof(_test_1st_frag_comp)))
        );
    _target_buf_len = 0;
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* shouldn't trigger any sending */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_comp)));
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_EQUAL_INT(_vrbe_base.datagram_size, rbuf->pkt->size);
    /* check if IPv6 packet is started in reassembly buffer */
    TEST_ASSERT_EQUAL_INT(0x60, ((uint8_t *)rbuf->pkt->data)[0] & 0xf0);
    /* check `received` bitmap (used for ACKs) */
    TEST_ASSERT_EQUAL_INT(0x80, rbuf->received[0]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[1]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[2]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[3]);
    gnrc_pktbuf_release(rbuf->pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_ep__1st_frag_abort(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp,
                                      sizeof(_test_1st_frag_comp)))
        );
    /* configure interface to be endpoint of route */
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_addr_t),
                          _add_dst(&_rem_gb, REM_GB_PFX_LEN));
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
    /* shouldn't trigger any sending */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_comp)));
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_abort_frag,
                                      sizeof(_test_abort_frag)))
        );
    /* expect abort ACK, see
     * https://tools.ietf.org/html/rfc8931#section-6.3 */
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    _target_buf_len = 0;
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(sixlowpan_sfr_ack_t)));
    /* reassembly buffer entry was deleted */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_ep__1st_frag_abort__req_ack(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_pktsnip_t *frag;
    size_t mhr_len;

    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_1st_frag_comp,
                                      sizeof(_test_1st_frag_comp)))
        );
    /* configure interface to be endpoint of route */
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_addr_t),
                          _add_dst(&_rem_gb, REM_GB_PFX_LEN));
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
    /* shouldn't trigger any sending */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_1st_frag_comp)));
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_abort_frag,
                                      sizeof(_test_abort_frag)))
        );
    /* set ACK request flag */
    sixlowpan_sfr_rfrag_set_ack_req(frag->data);
    /* expect abort ACK, see
     * https://tools.ietf.org/html/rfc8931#section-6.3 */
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(sixlowpan_sfr_ack_t))));
    /* reassembly buffer entry was deleted */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, _vrbe_base.tag
        ));
    _check_abort_ack(mhr_len, _vrbe_base.tag);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_recv_ep__complete_datagram(void)
{
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    gnrc_netreg_entry_t netreg = GNRC_NETREG_ENTRY_INIT_PID(
            GNRC_NETREG_DEMUX_CTX_ALL,
            thread_getpid()
        );
    gnrc_pktsnip_t *frag;
    size_t mhr_len;
    BITFIELD(exp_bitmap, 32U) = { 0 };
    uint8_t exp_seq;

    /* configure interface to be endpoint of route */
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_addr_t),
                          _add_dst(&_rem_gb, REM_GB_PFX_LEN));
    netdev_ieee802154_t *netdev_ieee802154 = container_of(_mock_netif->dev,
                                                          netdev_ieee802154_t,
                                                          netdev);
    netdev_test_t *netdev_test = container_of(netdev_ieee802154, netdev_test_t,
                                              netdev);
    netdev_test_set_send_cb(netdev_test,
                            _mock_netdev_send);
    /* ==== FIRST FRAGMENT ==== */
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_send_frag1,
                                      sizeof(_test_send_frag1)))
        );
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* shouldn't trigger any sending */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag1)));
    /* normal reassembly should have started */
    /* reassembly buffer entry should have been created */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* and VRB remains empty */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(
            _vrbe_base.src, _vrbe_base.src_len, TEST_SEND_DATAGRAM_TAG
        ));
    TEST_ASSERT_EQUAL_INT(sizeof(_test_send_ipv6) + sizeof(_test_send_icmpv6),
                          rbuf->pkt->size);
    /* check if IPv6 packet is started in reassembly buffer */
    TEST_ASSERT_EQUAL_INT(0x60, ((uint8_t *)rbuf->pkt->data)[0] & 0xf0);
    /* check `received` bitmap (used for ACKs) */
    TEST_ASSERT_EQUAL_INT(0x80, rbuf->received[0]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[1]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[2]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[3]);

    /* ==== SECOND FRAGMENT ==== */
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_send_frag2,
                                      sizeof(_test_send_frag2)))
        );
    /* set ACK Req flag in RFRAG header */
    sixlowpan_sfr_rfrag_set_ack_req(frag->data);
    exp_seq = sixlowpan_sfr_rfrag_get_seq(frag->data);
    _target_buf_len = 0;
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* expect RFRAG-ACK */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag2))));
    bf_set(exp_bitmap, 0);          /* first fragment was received */
    bf_set(exp_bitmap, exp_seq);    /* and second fragment */
    _check_ack(mhr_len, TEST_SEND_DATAGRAM_TAG, exp_bitmap);
    /* reassembly buffer entry should still exist */
    TEST_ASSERT_NOT_NULL((rbuf = _first_non_empty_rbuf()));
    /* check `received` bitmap (used for ACKs) */
    TEST_ASSERT_EQUAL_INT(0xc0, rbuf->received[0]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[1]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[2]);
    TEST_ASSERT_EQUAL_INT(0x00, rbuf->received[3]);

    gnrc_netreg_register(GNRC_NETTYPE_IPV6, &netreg);
    /* ==== THIRD FRAGMENT ==== */
    TEST_ASSERT_NOT_NULL(
            (frag = _create_recv_frag(_test_send_frag3,
                                      sizeof(_test_send_frag3)))
        );
    _target_buf_len = 0;
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 frag));
    /* expect RFRAG-ACK */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag3))));
    /* reassembly buffer should have been removed */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    /* expect FULL bitmap, as the datagram is now complete */
    memset(exp_bitmap, 0xff, sizeof(exp_bitmap));
    _check_ack(mhr_len, TEST_SEND_DATAGRAM_TAG, exp_bitmap);

    /* retry 5 times to receive then give up (IPv6 thread might send NDP
     * messages to itself) */
    for (unsigned i = 0; i < 5U; i++) {
        msg_t msg;

        if (xtimer_msg_receive_timeout(&msg, 1000) < 0) {
            continue;
        }
        if (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) {
            gnrc_pktsnip_t *pkt;
            uint8_t *data;

            gnrc_netreg_unregister(GNRC_NETTYPE_IPV6, &netreg);
            pkt = msg.content.ptr;
            data = pkt->data;
            TEST_ASSERT_EQUAL_INT(sizeof(_test_send_ipv6) +
                                  sizeof(_test_send_icmpv6), pkt->size);
            TEST_ASSERT_MESSAGE(
                memcmp(data, _test_send_ipv6, sizeof(_test_send_ipv6)) == 0,
                "Unexpected IPv6 header in reassembly"
            );
            TEST_ASSERT_MESSAGE(
                memcmp(&data[sizeof(_test_send_ipv6)], _test_send_icmpv6,
                       sizeof(_test_send_icmpv6)) == 0,
                "Unexpected ICMPv6 packet in reassembly"
            );
            gnrc_pktbuf_release(pkt);
            break;
        }
        else if (msg.type == GNRC_NETAPI_MSG_TYPE_SND) {
            /* release packet sent by network stack */
            gnrc_pktbuf_release(msg.content.ptr);
        }
    }
    xtimer_usleep(1000);    /* give GNRC time to clean up */
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_send(void)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    gnrc_pktsnip_t *pkt;
    sixlowpan_sfr_rfrag_t *hdr;
    size_t mhr_len;
    uint8_t tag;

    /* window is large enough to send all fragments at once */
    TEST_ASSERT(3 <= CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE);
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
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    tag = hdr->base.tag;
    _target_buf_len = 0;
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag2))));
    _check_send_frag2(mhr_len, false);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    _target_buf_len = 0;
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag3))));
    /* last fragment should request an ACK */
    _check_send_frag3(mhr_len, true);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES; i++) {
        /* fragmentation buffer still exists for re-sending */
        TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag)));
        /* three fragments wait for potential re-sending */
        TEST_ASSERT_EQUAL_INT(3U, clist_count(&fbuf->sfr.window));
        _target_buf_len = 0;
        _wait_arq_timeout(fbuf);
        /* resend of fragment 3 */
        TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag3))));
        /* last fragment should request an ACK */
        _check_send_frag3(mhr_len, true);
        hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
        TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    }
    _target_buf_len = 0;
    _wait_arq_timeout(fbuf);
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag3)));
    /* fragmentation buffer should have been deleted after
     * CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES retries */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_fb_get_by_tag(tag));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_send__first_ackd(void)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    gnrc_pktsnip_t *pkt;
    sixlowpan_sfr_rfrag_t *hdr;
    sixlowpan_sfr_ack_t *ack_hdr;
    size_t mhr_len;
    uint8_t tag;
    BITFIELD(ack_bitmap, 32U) = { 0 };

    /* window is large enough to send all fragments at once */
    TEST_ASSERT(3 <= CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE);
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
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    tag = hdr->base.tag;
    /* simulate successful reception for this fragment */
    bf_set(ack_bitmap, sixlowpan_sfr_rfrag_get_seq(hdr));
    _target_buf_len = 0;
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag2))));
    _check_send_frag2(mhr_len, false);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    _target_buf_len = 0;
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag3))));
    /* last fragment should request an ACK */
    _check_send_frag3(mhr_len, true);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    /* fragmentation buffer still exists for re-sending */
    TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag)));
    /* three fragments wait for potential re-sending */
    TEST_ASSERT_EQUAL_INT(3U, clist_count(&fbuf->sfr.window));
    TEST_ASSERT_NOT_NULL(
            (pkt = _create_recv_ack(_test_ack, sizeof(_test_ack)))
        );
    ack_hdr = pkt->data;
    ack_hdr->base.tag = tag;
    memcpy(ack_hdr->bitmap, ack_bitmap, sizeof(ack_hdr->bitmap));
    _target_buf_len = 0;
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 pkt));
    /* fragmentation buffer still exists for re-sending */
    TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag)));
    /* two fragments wait for potential re-sending */
    TEST_ASSERT_EQUAL_INT(2U, clist_count(&fbuf->sfr.window));
    /* resend of fragment 2 (fragment 1 is not resent) */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag2))));
    _check_send_frag2(mhr_len, false);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES; i++) {
        /* fragmentation buffer still exists for re-sending */
        TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag)));
        /* two fragments wait for potential re-sending */
        TEST_ASSERT_EQUAL_INT(2U, clist_count(&fbuf->sfr.window));
        /* resend of fragment 3 */
        TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag3))));
        /* last fragment should request an ACK */
        _check_send_frag3(mhr_len, true);
        hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
        TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
        _target_buf_len = 0;
        _wait_arq_timeout(fbuf);
    }
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag3)));
    /* fragmentation buffer should have been deleted after
     * CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES retries */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_fb_get_by_tag(tag));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_send__middle_ackd(void)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    gnrc_pktsnip_t *pkt;
    sixlowpan_sfr_rfrag_t *hdr;
    sixlowpan_sfr_ack_t *ack_hdr;
    size_t mhr_len;
    uint8_t tag;
    BITFIELD(ack_bitmap, 32U) = { 0 };

    /* window is large enough to send all fragments at once */
    TEST_ASSERT(3 <= CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE);
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
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    tag = hdr->base.tag;
    _target_buf_len = 0;
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag2))));
    _check_send_frag2(mhr_len, false);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    /* simulate successful reception for this fragment */
    bf_set(ack_bitmap, sixlowpan_sfr_rfrag_get_seq(hdr));
    _target_buf_len = 0;
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag3))));
    /* last fragment should request an ACK */
    _check_send_frag3(mhr_len, true);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    /* fragmentation buffer still exists for re-sending */
    TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag)));
    /* three fragments wait for potential re-sending */
    TEST_ASSERT_EQUAL_INT(3U, clist_count(&fbuf->sfr.window));
    TEST_ASSERT_NOT_NULL(
            (pkt = _create_recv_ack(_test_ack, sizeof(_test_ack)))
        );
    ack_hdr = pkt->data;
    ack_hdr->base.tag = tag;
    memcpy(ack_hdr->bitmap, ack_bitmap, sizeof(ack_hdr->bitmap));
    _target_buf_len = 0;
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 pkt));
    /* fragmentation buffer still exists for re-sending */
    TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag)));
    /* two fragments wait for potential re-sending */
    TEST_ASSERT_EQUAL_INT(2U, clist_count(&fbuf->sfr.window));
    /* resend of fragment 1 */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag1))));
    _check_send_frag1(mhr_len, false);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    /* should time out since fragment 2 is not resent */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag2)));
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES; i++) {
        /* fragmentation buffer still exists for re-sending */
        TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag)));
        /* two fragments wait for potential re-sending */
        TEST_ASSERT_EQUAL_INT(2U, clist_count(&fbuf->sfr.window));
        /* resend of fragment 3 */
        TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag3))));
        /* last fragment should request an ACK */
        _check_send_frag3(mhr_len, true);
        hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
        TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
        _target_buf_len = 0;
        _wait_arq_timeout(fbuf);
    }
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag3)));
    /* fragmentation buffer should have been deleted after
     * CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES retries */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_fb_get_by_tag(tag));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_send__last_ackd(void)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    gnrc_pktsnip_t *pkt;
    sixlowpan_sfr_rfrag_t *hdr;
    sixlowpan_sfr_ack_t *ack_hdr;
    size_t mhr_len;
    uint8_t tag;
    BITFIELD(ack_bitmap, 32U) = { 0 };

    /* window is large enough to send all fragments at once */
    TEST_ASSERT(3 <= CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE);
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
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    tag = hdr->base.tag;
    _target_buf_len = 0;
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag2))));
    _check_send_frag2(mhr_len, false);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    _target_buf_len = 0;
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag3))));
    /* last fragment should request an ACK */
    _check_send_frag3(mhr_len, true);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    /* fragmentation buffer still exists for re-sending */
    TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag)));
    /* three fragments wait for potential re-sending */
    TEST_ASSERT_EQUAL_INT(3U, clist_count(&fbuf->sfr.window));
    /* simulate successful reception for this fragment */
    bf_set(ack_bitmap, sixlowpan_sfr_rfrag_get_seq(hdr));
    TEST_ASSERT_NOT_NULL(
            (pkt = _create_recv_ack(_test_ack, sizeof(_test_ack)))
        );
    ack_hdr = pkt->data;
    ack_hdr->base.tag = tag;
    memcpy(ack_hdr->bitmap, ack_bitmap, sizeof(ack_hdr->bitmap));
    _target_buf_len = 0;
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 pkt));
    /* fragmentation buffer still exists for re-sending */
    TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag)));
    /* two fragments wait for potential re-sending */
    TEST_ASSERT_EQUAL_INT(2U, clist_count(&fbuf->sfr.window));
    /* resend of fragment 1 */
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag1))));
    _check_send_frag1(mhr_len, false);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES; i++) {
        /* fragmentation buffer still exists for re-sending */
        TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag)));
        /* two fragments wait for potential re-sending */
        TEST_ASSERT_EQUAL_INT(2U, clist_count(&fbuf->sfr.window));
        /* resend of fragment 2 (fragment 3 was ACK'd so it does not need
         * to be resent) */
        TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag2))));
        /* second fragment should now request the ACK */
        _check_send_frag2(mhr_len, true);
        hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
        TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
        TEST_ASSERT(sixlowpan_sfr_rfrag_ack_req(hdr));
        _target_buf_len = 0;
        _wait_arq_timeout(fbuf);
    }
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag3)));
    /* fragmentation buffer should have been deleted after
     * CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES retries */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_fb_get_by_tag(tag));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_send__all_ackd(void)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    gnrc_pktsnip_t *pkt;
    sixlowpan_sfr_rfrag_t *hdr;
    sixlowpan_sfr_ack_t *ack_hdr;
    size_t mhr_len;
    uint8_t tag;
    BITFIELD(ack_bitmap, 32U) = { 0 };

    /* window is large enough to send all fragments at once */
    TEST_ASSERT(3 <= CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE);
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
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    tag = hdr->base.tag;
    /* simulate successful reception for this fragment */
    bf_set(ack_bitmap, sixlowpan_sfr_rfrag_get_seq(hdr));
    _target_buf_len = 0;
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag2))));
    _check_send_frag2(mhr_len, false);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    /* simulate successful reception for this fragment */
    bf_set(ack_bitmap, sixlowpan_sfr_rfrag_get_seq(hdr));
    _target_buf_len = 0;
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag3))));
    /* last fragment should request an ACK */
    _check_send_frag3(mhr_len, true);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    /* simulate successful reception for this fragment */
    bf_set(ack_bitmap, sixlowpan_sfr_rfrag_get_seq(hdr));
    /* fragmentation buffer still exists for re-sending */
    TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag)));
    /* three fragments wait for potential re-sending */
    TEST_ASSERT_EQUAL_INT(3U, clist_count(&fbuf->sfr.window));
    TEST_ASSERT_NOT_NULL(
            (pkt = _create_recv_ack(_test_ack, sizeof(_test_ack)))
        );
    ack_hdr = pkt->data;
    ack_hdr->base.tag = tag;
    memcpy(ack_hdr->bitmap, ack_bitmap, sizeof(ack_hdr->bitmap));
    _target_buf_len = 0;
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 pkt));
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag3)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_send__FULL_ack_recv(void)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    gnrc_pktsnip_t *pkt;
    sixlowpan_sfr_rfrag_t *hdr;
    sixlowpan_sfr_ack_t *ack_hdr;
    size_t mhr_len;
    uint8_t tag;

    /* window is large enough to send all fragments at once */
    TEST_ASSERT(3 <= CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE);
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
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    tag = hdr->base.tag;
    /* simulate successful reception for this fragment */
    _target_buf_len = 0;
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag2))));
    _check_send_frag2(mhr_len, false);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    /* simulate successful reception for this fragment */
    _target_buf_len = 0;
    TEST_ASSERT((mhr_len = _wait_for_packet(sizeof(_test_send_frag3))));
    /* last fragment should request an ACK */
    _check_send_frag3(mhr_len, true);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT_EQUAL_INT(tag, hdr->base.tag);
    /* fragmentation buffer still exists for re-sending */
    TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag)));
    /* three fragments wait for potential re-sending */
    TEST_ASSERT_EQUAL_INT(3U, clist_count(&fbuf->sfr.window));
    /* simulate successful reception for this fragment */
    TEST_ASSERT_NOT_NULL(
            (pkt = _create_recv_ack(_test_ack, sizeof(_test_ack)))
        );
    ack_hdr = pkt->data;
    ack_hdr->base.tag = tag;
    /* FULL ACK == all ones */
    memset(ack_hdr->bitmap, 0xff, sizeof(ack_hdr->bitmap));
    _target_buf_len = 0;
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 pkt));
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag3)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static Test *tests_gnrc_sixlowpan_frag_sfr_api(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_sfr_forward__success__1st_frag_sixlo),
        new_TestFixture(test_sfr_forward__success__1st_frag_iphc),
        new_TestFixture(test_sfr_forward__success__nth_frag_incomplete),
        new_TestFixture(test_sfr_forward__success__nth_frag_complete),
        new_TestFixture(test_sfr_forward__ENOMEM__netif_hdr_build_fail),
    };

    EMB_UNIT_TESTCALLER(tests, _set_up, _tear_down, fixtures);

    return (Test *)&tests;
}

static Test *tests_gnrc_sixlowpan_frag_sfr_integration(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_uncomp),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_uncomp__req_ack),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_uncomp__no_route),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_uncomp__after_nth_frag),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_comp),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_comp__resend),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_comp__only_iphc),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_comp__only_iphc_no_nhc),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_comp__no_route),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_comp__no_route_only_iphc),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_comp__no_refrag),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_comp__after_nth_frag),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_abort),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_nalp),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_nalp__req_ack),
        new_TestFixture(test_sixlo_recv_fwd__1st_frag_nalp__no_vrb),
        new_TestFixture(test_sixlo_recv_fwd__nth_frag),
        new_TestFixture(test_sixlo_recv_fwd__nth_frag__no_vrbe),
        new_TestFixture(test_sixlo_recv_fwd__nth_frag__duplicate),
        new_TestFixture(test_sixlo_recv_fwd__nth_frag__overlap),
        new_TestFixture(test_sixlo_recv_fwd__frag__too_short),
        new_TestFixture(test_sixlo_recv_fwd__ack),
        new_TestFixture(test_sixlo_recv_fwd__NULL_ack),
        new_TestFixture(test_sixlo_recv_fwd__FULL_ack),
        new_TestFixture(test_sixlo_recv_fwd__ack__no_vrbe),
        new_TestFixture(test_sixlo_recv_fwd__ack__too_short),
        new_TestFixture(test_sixlo_recv_ep__1st_frag_uncomp),
        new_TestFixture(test_sixlo_recv_ep__1st_frag_uncomp__req_ack),
        new_TestFixture(test_sixlo_recv_ep__1st_frag_uncomp__after_nth_frag),
        new_TestFixture(test_sixlo_recv_ep__1st_frag_comp),
        new_TestFixture(test_sixlo_recv_ep__1st_frag_comp__after_nth_frag),
        new_TestFixture(test_sixlo_recv_ep__1st_frag_abort),
        new_TestFixture(test_sixlo_recv_ep__1st_frag_abort__req_ack),
        new_TestFixture(test_sixlo_recv_ep__complete_datagram),
        new_TestFixture(test_sixlo_send),
        new_TestFixture(test_sixlo_send__first_ackd),
        new_TestFixture(test_sixlo_send__middle_ackd),
        new_TestFixture(test_sixlo_send__last_ackd),
        new_TestFixture(test_sixlo_send__all_ackd),
        new_TestFixture(test_sixlo_send__FULL_ack_recv),
    };

    EMB_UNIT_TESTCALLER(tests, _set_up, _tear_down, fixtures);

    return (Test *)&tests;
}

int main(void)
{
    _tests_init();

    TESTS_START();
    TESTS_RUN(tests_gnrc_sixlowpan_frag_sfr_api());
    TESTS_RUN(tests_gnrc_sixlowpan_frag_sfr_integration());
    TESTS_END();
    return 0;
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
    gnrc_netif_hdr_set_netif(netif_hdr, _mock_netif);
    netif_hdr->flags = GNRC_NETIF_HDR_FLAGS_MORE_DATA;
    return gnrc_pktbuf_add(netif, frag_data, frag_size,
                           GNRC_NETTYPE_SIXLOWPAN);
}

static gnrc_pktsnip_t *_create_recv_ack(const void *ack_data,
                                        size_t ack_size)
{
    gnrc_pktsnip_t *netif;
    gnrc_netif_hdr_t *netif_hdr;

    netif = gnrc_netif_hdr_build(_rem_l2, sizeof(_rem_l2),
                                 _vrbe_base.dst, _vrbe_base.dst_len);
    if (netif == NULL) {
        return NULL;
    }
    netif_hdr = netif->data;
    gnrc_netif_hdr_set_netif(netif_hdr, _mock_netif);
    return gnrc_pktbuf_add(netif, ack_data, ack_size,
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

static int _add_dst(const ipv6_addr_t *dst, unsigned pfx_len)
{
    /* discarding const qualifier should be safe */
    return gnrc_netif_ipv6_addr_add(_mock_netif, (ipv6_addr_t *)dst, pfx_len,
                                    GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID);
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
    gnrc_netif_hdr_set_netif(netif_hdr, _mock_netif);
    LL_PREPEND(pkt2, pkt1);
    return pkt2;
}

static size_t _wait_for_packet(size_t exp_size)
{
    size_t mhr_len;
    uint32_t now = 0U;

    xtimer_mutex_lock_timeout(&_target_buf_filled,
                              SEND_PACKET_TIMEOUT);
    while ((mhr_len = ieee802154_get_frame_hdr_len(_target_buf))) {
        now = xtimer_now_usec();
        size_t size = _target_buf_len - mhr_len;
#ifdef MODULE_OD
        if (_target_buf_len > 0) {
            puts("Sent packet: ");
            od_hex_dump(_target_buf, _target_buf_len, OD_WIDTH_DEFAULT);
        }
#endif /* MODULE_OD */
        if ((sizeof(sixlowpan_sfr_ack_t) == size) &&
            (sixlowpan_sfr_ack_is((sixlowpan_sfr_t *)&_target_buf[mhr_len]))) {
            /* found ACK */
            break;
        }
        if (exp_size == size) {
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
    if (mhr_len > 0) {
        if ((CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US > 0U) &&
            (now - _last_sent_frame) < CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US) {
            puts("(now - _last_sent_frame) < CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US");
            return 0;
        }
        _last_sent_frame = now;
    }
    return mhr_len;
}

static inline void _wait_arq_timeout(gnrc_sixlowpan_frag_fb_t *fbuf)
{
    /* wait 1.25 * arq_timeout of datagram fbuf->tag */
    xtimer_usleep((fbuf->sfr.arq_timeout +
                   (fbuf->sfr.arq_timeout >> 2)) * US_PER_MS);
}

static void _check_vrbe_values(gnrc_sixlowpan_frag_vrb_t *vrbe,
                               size_t mhr_len, bool check_offset_diff,
                               int16_t exp_offset_diff)
{
    uint8_t target_buf_dst[IEEE802154_LONG_ADDRESS_LEN];
    sixlowpan_sfr_rfrag_t *frag_hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    le_uint16_t tmp;

    TEST_ASSERT(sixlowpan_sfr_rfrag_is(&frag_hdr->base));
    TEST_ASSERT_EQUAL_INT(vrbe->super.dst_len,
                          ieee802154_get_dst(_target_buf,
                                             target_buf_dst,
                                             &tmp));
    TEST_ASSERT_MESSAGE(memcmp(vrbe->super.dst, target_buf_dst,
                               vrbe->super.dst_len) == 0,
                        "vrbe->out_dst != target_buf_dst");

    TEST_ASSERT_EQUAL_INT(vrbe->out_tag,
                          frag_hdr->base.tag);
    TEST_ASSERT_NOT_NULL(vrbe->in_netif);
    if (check_offset_diff) {
        TEST_ASSERT_EQUAL_INT(exp_offset_diff, vrbe->offset_diff);
    }
}

static void _check_ack(size_t mhr_len, uint8_t exp_tag,
                       const uint8_t *exp_bitmap)
{
    sixlowpan_sfr_ack_t *ack = (sixlowpan_sfr_ack_t *)&_target_buf[mhr_len];

    TEST_ASSERT(sixlowpan_sfr_ack_is(&ack->base));
    TEST_ASSERT_EQUAL_INT(exp_tag, ack->base.tag);
    TEST_ASSERT_MESSAGE(memcmp(ack->bitmap, exp_bitmap,
                               sizeof(ack->bitmap)) == 0,
                        "Unexpected RFRAG-ACK bitmap");
}

static void _check_abort_ack(size_t mhr_len, uint8_t exp_tag)
{
    static const uint8_t _null_bitmap[] = { 0, 0, 0, 0 };

    _check_ack(mhr_len, exp_tag, _null_bitmap);
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

static void _check_send_frag_datagram_fields(size_t mhr_len)
{
    sixlowpan_sfr_rfrag_t *frag_hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];

    if (sixlowpan_sfr_rfrag_get_seq(frag_hdr) == 0) {
        TEST_ASSERT_EQUAL_INT(TEST_SEND_COMP_DATAGRAM_SIZE,
                              sixlowpan_sfr_rfrag_get_offset(frag_hdr));
    }
}

static void _check_send_frag1(size_t mhr_len, bool ack_req)
{
    sixlowpan_sfr_rfrag_t *frag_hdr;

    frag_hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT(sixlowpan_sfr_rfrag_is(&frag_hdr->base));
    _check_send_frag_datagram_fields(mhr_len);
    TEST_ASSERT_EQUAL_INT(
            sizeof(_test_send_frag1) - sizeof(sixlowpan_sfr_rfrag_t),
            sixlowpan_sfr_rfrag_get_frag_size(frag_hdr)
        );
    TEST_ASSERT_EQUAL_INT(0, sixlowpan_sfr_rfrag_get_seq(frag_hdr));
    TEST_ASSERT_EQUAL_INT(ack_req, sixlowpan_sfr_rfrag_ack_req(frag_hdr));
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_send_frag1[TEST_SEND_FRAG1_PAYLOAD_POS],
                   &_target_buf[TEST_SEND_FRAG1_PAYLOAD_POS + mhr_len],
                   sixlowpan_sfr_rfrag_get_frag_size(frag_hdr)) == 0,
            "unexpected IPHC header"
        );
}

static void _check_send_frag2(size_t mhr_len, bool ack_req)
{
    sixlowpan_sfr_rfrag_t *frag_hdr;

    frag_hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT(sixlowpan_sfr_rfrag_is(&frag_hdr->base));
    _check_send_frag_datagram_fields(mhr_len);
    TEST_ASSERT_EQUAL_INT(
            sizeof(_test_send_frag2) - sizeof(sixlowpan_sfr_rfrag_t),
            sixlowpan_sfr_rfrag_get_frag_size(frag_hdr)
        );
    TEST_ASSERT_EQUAL_INT(
            sixlowpan_sfr_rfrag_get_offset((sixlowpan_sfr_rfrag_t *)&_test_send_frag2),
            sixlowpan_sfr_rfrag_get_offset(frag_hdr)
        );
    TEST_ASSERT(sixlowpan_sfr_rfrag_get_seq(frag_hdr) > 0);
    TEST_ASSERT_EQUAL_INT(ack_req, sixlowpan_sfr_rfrag_ack_req(frag_hdr));
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_send_frag2[sizeof(sixlowpan_sfr_rfrag_t)],
                   &_target_buf[sizeof(sixlowpan_sfr_rfrag_t) + mhr_len],
                   sixlowpan_sfr_rfrag_get_frag_size(frag_hdr)) == 0,
            "unexpected send packet payload"
        );
}

static void _check_send_frag3(size_t mhr_len, bool ack_req)
{
    sixlowpan_sfr_rfrag_t *frag_hdr;

    frag_hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    TEST_ASSERT(sixlowpan_sfr_rfrag_is(&frag_hdr->base));
    _check_send_frag_datagram_fields(mhr_len);
    TEST_ASSERT_EQUAL_INT(
            sizeof(_test_send_frag3) - sizeof(sixlowpan_sfr_rfrag_t),
            sixlowpan_sfr_rfrag_get_frag_size(frag_hdr)
        );
    TEST_ASSERT_EQUAL_INT(
            sixlowpan_sfr_rfrag_get_offset((sixlowpan_sfr_rfrag_t *)&_test_send_frag3),
            sixlowpan_sfr_rfrag_get_offset(frag_hdr)
        );
    TEST_ASSERT(sixlowpan_sfr_rfrag_get_seq(frag_hdr) > 0);
    TEST_ASSERT_EQUAL_INT(ack_req, sixlowpan_sfr_rfrag_ack_req(frag_hdr));
    TEST_ASSERT_MESSAGE(
            memcmp(&_test_send_frag3[sizeof(sixlowpan_sfr_rfrag_t)],
                   &_target_buf[sizeof(sixlowpan_sfr_rfrag_t) + mhr_len],
                   sixlowpan_sfr_rfrag_get_frag_size(frag_hdr)) == 0,
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
