/*
 * Copyright (C) 2019-2021 Freie Universität Berlin
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
 * @brief       Tests 6LoWPAN Selective Fragment Recovery with Congestion
 *              Control
 *
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "cib.h"
#include "common.h"
#include "congure/mock.h"
#include "embUnit.h"
#include "embUnit/embUnit.h"
#include "mutex.h"
#include "net/ipv6.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6/nib/nc.h"
#include "net/gnrc/ipv6/nib/ft.h"
#include "net/gnrc/sixlowpan/config.h"
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
#define REM_L2  { _LL0, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7 + 1 }

#define TEST_FRAG_TAG                               (0xADU)
#define TEST_SEND_COMP_DATAGRAM_SIZE                (193U)
#define TEST_SEND_DATAGRAM_TAG                      (0x25U)
#define TEST_SEND_FRAG1_PAYLOAD_POS                 (6U)
#define TEST_SEND_FRAG1_PAYLOAD_SIZE                (35U)

enum {
    TEST_SEND_FRAG_SEQ1 = 0,
    TEST_SEND_FRAG_SEQ2,
    TEST_SEND_FRAG_SEQ3,
};

#define TEST_NULL_ACK           ((intptr_t)0x0)
#define TEST_FULL_ACK           ((intptr_t)0xff)

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
        /* Source: Global address generated from LOC_L2 */
        0x20, 0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e,
        _LL0 ^ 0x2, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7,
        /* Destination: Global address generated from REM_L2 */
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
        /* (uncompressed) Source: Global address generated from LOC_L2 */
        0x20, 0x01, 0x0d, 0xb8, 0xd3, 0x35, 0x91, 0x7e,
        _LL0 ^ 0x2, _LL1, _LL2, _LL3, _LL4, _LL5, _LL6, _LL7,
        /* (uncompressed) Destination: Global address generated from REM_L2 */
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
static const uint8_t _rem_l2[] = REM_L2;
static const gnrc_sixlowpan_frag_rb_base_t _vrbe_base = {
        .src = { 0xde, 0x71, 0x2b, 0x85, 0x08, 0x2f, 0x75, 0xfb },
        .src_len = IEEE802154_LONG_ADDRESS_LEN,
        .dst = LOC_L2,
        .dst_len = IEEE802154_LONG_ADDRESS_LEN,
        .tag = TEST_FRAG_TAG,
        .datagram_size = 1232U,
        .current_size = 0U,
    };
static congure_mock_snd_t _sfr_congure_mocks[CONFIG_GNRC_SIXLOWPAN_FRAG_FB_SIZE];
static uint8_t _target_buf[128U];
static uint8_t _target_buf_len;
/* to protect _target_buf and _target_buf_len */
/* to wait for new data in _target_buf */
static mutex_t _target_buf_filled = MUTEX_INIT_LOCKED;
static mutex_t _target_buf_barrier = MUTEX_INIT;
uint32_t _last_sent_frame;
unsigned _in_flight_frags;

static void _congure_init(congure_snd_t *cong, void *ctx);
static void _congure_report_msg_sent(congure_snd_t *cong, unsigned msg_size);
static void _congure_report_msg_discarded(congure_snd_t *cong,
                                          unsigned msg_size);
static void _congure_report_msgs_timeout_lost(congure_snd_t *cong,
                                              congure_snd_msg_t *msgs);
static void _congure_report_msg_acked(congure_snd_t *cong,
                                      congure_snd_msg_t *msg,
                                      congure_snd_ack_t *ack);
static congure_mock_snd_t *_get_congure_by_fb(const gnrc_sixlowpan_frag_fb_t *fb);
static gnrc_pktsnip_t *_create_recv_ack(const void *ack_data,
                                        size_t ack_size);
static gnrc_pktsnip_t *_create_send_datagram(bool compressed, bool payload);
static size_t _wait_for_packet(size_t exp_size);
static void _check_send_frag1(size_t mhr_len, bool ack_req, bool *finished);
static void _check_send_frag2(size_t mhr_len, bool ack_req, bool *finished);
static void _check_send_frag3(size_t mhr_len, bool ack_req, bool *finished);
static void _check_congure_snd_msg(congure_snd_msg_t *msg);
static int _mock_netdev_send(netdev_t *dev, const iolist_t *iolist);

static const congure_snd_driver_t _congure_test_driver = {
    .init = _congure_init,
    .report_msg_sent = _congure_report_msg_sent,
    .report_msg_discarded = _congure_report_msg_discarded,
    .report_msgs_timeout = _congure_report_msgs_timeout_lost,
    .report_msgs_lost = _congure_report_msgs_timeout_lost,
    .report_msg_acked = _congure_report_msg_acked,
};

congure_snd_t *gnrc_sixlowpan_frag_sfr_congure_snd_get(void)
{
    for (unsigned i = 0; i < ARRAY_SIZE(_sfr_congure_mocks); i++) {
        if (_sfr_congure_mocks[i].super.driver == NULL) {
            congure_mock_snd_setup(&_sfr_congure_mocks[i],
                                   &_congure_test_driver);
            return &_sfr_congure_mocks[i].super;
        }
    }
    return NULL;
}

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
    memset(_sfr_congure_mocks, 0, sizeof(_sfr_congure_mocks));
    _in_flight_frags = 0U;
    gnrc_ipv6_nib_init();
    gnrc_ipv6_nib_init_iface(_mock_netif);
    /* re-init for syncing */
    mutex_init(&_target_buf_filled);
    mutex_lock(&_target_buf_filled);
    mutex_init(&_target_buf_barrier);
}

static void _tear_down(void)
{
    netdev_test_set_send_cb((netdev_test_t *)_mock_netif->dev, NULL);
    mutex_unlock(&_target_buf_barrier);
    /* wait in case mutex in _mock_netdev_send was already entered */
    mutex_lock(&_target_buf_barrier);
    memset(_target_buf, 0, sizeof(_target_buf));
    _target_buf_len = 0;
    mutex_unlock(&_target_buf_barrier);
}

static int _check_congure_snd_msgs_list(clist_node_t *node, void *arg)
{
    (void)arg;
    _check_congure_snd_msg((congure_snd_msg_t *)node);
    return 0;
}

static void _test_send_frag(unsigned frag_seq, const uint8_t acked_frags,
                            const uint8_t ack_req, uint8_t *tag,
                            uint8_t *res_bitmap, bool *finished)
{
    static void (*const _check_send_func[])(size_t, bool, bool *) = {
        _check_send_frag1,
        _check_send_frag2,
        _check_send_frag3,
    };
    static const size_t exp_size[] = {
        sizeof(_test_send_frag1),
        sizeof(_test_send_frag2),
        sizeof(_test_send_frag3),
    };
    sixlowpan_sfr_rfrag_t *hdr;
    bool send_func_finished;
    size_t mhr_len;

    *finished = false;
    TEST_ASSERT((mhr_len = _wait_for_packet(exp_size[frag_seq])));
    /* tags are generated by the stack so don't check */
    _check_send_func[frag_seq](mhr_len, ack_req & (1 << frag_seq),
                               &send_func_finished);
    TEST_ASSERT(send_func_finished);
    hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];
    if (frag_seq == 0) {
        *tag = hdr->base.tag;
    }
    else {
        TEST_ASSERT_EQUAL_INT(*tag, hdr->base.tag);
    }
    if (res_bitmap && (acked_frags & (1 << frag_seq))) {
        /* simulate successful reception for this fragment */
        bf_set(res_bitmap, sixlowpan_sfr_rfrag_get_seq(hdr));
    }
    _target_buf_len = 0;
    *finished = true;
}

static void _test_initial_send(const uint8_t acked_frags, const uint8_t ack_req,
                               uint8_t *tag, uint8_t *res_bitmap,
                               bool *finished)
{
    gnrc_pktsnip_t *pkt;

    *finished = false;
    /* window is large enough to send all fragments at once */
    TEST_ASSERT(3 <= CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE);
    TEST_ASSERT_NOT_NULL((pkt = _create_send_datagram(false, true)));

    netdev_test_set_send_cb((netdev_test_t *)_mock_netif->dev,
                            _mock_netdev_send);
    TEST_ASSERT(0 < gnrc_netapi_dispatch_send(GNRC_NETTYPE_SIXLOWPAN,
                                              GNRC_NETREG_DEMUX_CTX_ALL,
                                              pkt));
    /* test send for fragments 1 to 3 */
    for (unsigned frag_seq = TEST_SEND_FRAG_SEQ1;
         frag_seq <= TEST_SEND_FRAG_SEQ3;
         frag_seq++) {
        bool _frag_was_sent_correctly;
        _test_send_frag(frag_seq, acked_frags, ack_req, tag, res_bitmap,
                        &_frag_was_sent_correctly);
        TEST_ASSERT(_frag_was_sent_correctly);
    }
    *finished = true;
}

static void _recv_ack(const uint8_t *ack_bitmap, uint8_t tag, bool ecn,
                      bool *finished)
{
    gnrc_pktsnip_t *pkt;
    sixlowpan_sfr_ack_t *ack_hdr;

    *finished = false;
    TEST_ASSERT_NOT_NULL(
            (pkt = _create_recv_ack(_test_ack, sizeof(_test_ack)))
        );
    ack_hdr = pkt->data;
    ack_hdr->base.tag = tag;
    if (ecn) {
        sixlowpan_sfr_set_ecn(&ack_hdr->base);
    }
    switch ((intptr_t)ack_bitmap) {
        case TEST_NULL_ACK:
        case TEST_FULL_ACK:
            memset(ack_hdr->bitmap, (intptr_t)ack_bitmap, sizeof(ack_hdr->bitmap));
            break;
        default:
            memcpy(ack_hdr->bitmap, ack_bitmap, sizeof(ack_hdr->bitmap));
            break;
    }
    TEST_ASSERT(0 < gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                                 GNRC_NETREG_DEMUX_CTX_ALL,
                                                 pkt));
    *finished = true;
}

#define TEST_ASSERT_FBUF_CONGURE_EXISTS(fbuf, cong) \
    TEST_ASSERT_NOT_NULL((fbuf = gnrc_sixlowpan_frag_fb_get_by_tag(tag))); \
    cong = _get_congure_by_fb(fbuf); \
    TEST_ASSERT(&cong->super == fbuf->sfr.congure)

#define TEST_ASSERT_FBUF_CONGURE_DESTROYED(fbuf, cong) \
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_fb_get_by_tag(tag)); \
    TEST_ASSERT_NULL(fbuf->sfr.congure); \
    TEST_ASSERT_NULL(c->super.driver)

#define TEST_ASSERT_REPORT_MSG_SENT(cong, num) \
    TEST_ASSERT_EQUAL_INT(num, (cong)->report_msg_sent_calls); \
    TEST_ASSERT(&(cong)->super == (cong)->report_msg_sent_args.c); \
    TEST_ASSERT_EQUAL_INT(1U, (cong)->report_msg_sent_args.msg_size)

#define TEST_ASSERT_REPORT_MSG_DISCARDED(cong, num) \
    TEST_ASSERT_EQUAL_INT(num, (cong)->report_msg_discarded_calls); \
    TEST_ASSERT(&(cong)->super == (cong)->report_msg_discarded_args.c); \
    TEST_ASSERT_EQUAL_INT(1, (cong)->report_msg_discarded_args.msg_size)

#define TEST_ASSERT_REPORT_MSGS_LOST_TIMEOUT(cong, num, m, method) \
    TEST_ASSERT_EQUAL_INT(num, (cong)->method ## _calls); \
    TEST_ASSERT(&(cong)->super == (cong)->method ## _args.c); \
    m = (cong)->method ## _args.msgs; \
    TEST_ASSERT_NOT_NULL(m)

#define TEST_ASSERT_REPORT_MSGS_LOST(cong, num, m) \
    TEST_ASSERT_REPORT_MSGS_LOST_TIMEOUT(cong, num, m, report_msgs_lost)

#define TEST_ASSERT_REPORT_MSGS_TIMEOUT(cong, num, m) \
    TEST_ASSERT_REPORT_MSGS_LOST_TIMEOUT(cong, num, m, report_msgs_timeout)

#define TEST_ASSERT_REPORT_MSG_ACKED(cong, num) \
    TEST_ASSERT_EQUAL_INT(num, (cong)->report_msg_acked_calls); \
    TEST_ASSERT(&(cong)->super == (cong)->report_msg_acked_args.c); \
    TEST_ASSERT_NOT_NULL((cong)->report_msg_acked_args.msg); \
    TEST_ASSERT_NOT_NULL((cong)->report_msg_acked_args.ack)

static void test_sixlo_send__no_ack(void)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    congure_mock_snd_t *c = NULL;
    congure_snd_msg_t *msgs = NULL;
    bool sent_frags_correct;
    uint8_t tag;
    static const uint8_t ack_req = 1 << TEST_SEND_FRAG_SEQ3;

    _test_initial_send(0, ack_req, &tag, NULL, &sent_frags_correct);
    TEST_ASSERT(sent_frags_correct);
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES; i++) {
        TEST_ASSERT_FBUF_CONGURE_EXISTS(fbuf, c);
        /* three fragments wait for potential re-sending */
        TEST_ASSERT_EQUAL_INT(3U, clist_count(&fbuf->sfr.window));
        /* report_msg_sent has been called 3 * (i + 1) times:
         * - 3 for each initial send of the fragments
         * - 3 each retry (while only fragment 3 is sent, 1 and 2 are also
         *   marked as in flight)
         */
        TEST_ASSERT_REPORT_MSG_SENT(c, 3U * (i + 1));
        gnrc_sixlowpan_frag_sfr_arq_timeout(fbuf);
        /* resend of fragment 3 */
        _test_send_frag(TEST_SEND_FRAG_SEQ3, 0, ack_req, &tag,
                        NULL, &sent_frags_correct);
        TEST_ASSERT(sent_frags_correct);
        /* report_msg_timeout was called i + 1 times; once for each ARQ
         * timeout */
        TEST_ASSERT_REPORT_MSGS_TIMEOUT(c, i + 1U, msgs);
        /* there is one message for each fragment */
        TEST_ASSERT_EQUAL_INT(3U, clist_count(&msgs->super));
        clist_foreach(&msgs->super, _check_congure_snd_msgs_list, NULL);
        /* first fragment has no resends */
        TEST_ASSERT_EQUAL_INT(
            0, ((congure_snd_msg_t *)msgs->super.next->next)->resends
        );
        /* second fragment has no resends */
        TEST_ASSERT_EQUAL_INT(
            0, ((congure_snd_msg_t *)msgs->super.next->next->next)->resends
        );
        /* third fragment has i + 1 resends */
        TEST_ASSERT_EQUAL_INT(
            i + 1,
            ((congure_snd_msg_t *)msgs->super.next->next->next->next)->resends
        );
        /* there are three fragments in flight */
        TEST_ASSERT_EQUAL_INT(3U, _in_flight_frags);
    }
    gnrc_sixlowpan_frag_sfr_arq_timeout(fbuf);
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag3)));
    /* fragmentation buffer and congure state should have been destroyed and
     * freed after CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES retries */
    TEST_ASSERT_FBUF_CONGURE_DESTROYED(fbuf, c);
    /* report_msg_sent has been called 3 * (RETRIES + 1) times:
     * - 3 for each initial send of the fragments
     * - 3 each retry (while only fragment 3 is sent, 1 and 2 are marked as
     *   in flight)
     */
    TEST_ASSERT_REPORT_MSG_SENT(
        c, 3U * (CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES + 1)
    );
    /* report_msg_timeout was called CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES + 1
     * times; once for each ARQ timeout */
    TEST_ASSERT_REPORT_MSGS_TIMEOUT(c,
                                    CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES + 1U,
                                    msgs);
    /* inter_msg_interval was called several times to calculate inter-frame gap
     * TODO */
    TEST_ASSERT(0 < c->inter_msg_interval_calls);
    /* none of the other CongURE report methods should have been called */
    TEST_ASSERT_EQUAL_INT(0, c->report_msg_discarded_calls);
    TEST_ASSERT_EQUAL_INT(0, c->report_msgs_lost_calls);
    TEST_ASSERT_EQUAL_INT(0, c->report_msg_acked_calls);
    TEST_ASSERT_EQUAL_INT(0, c->report_ecn_ce_calls);
    /* no fragments reported in flight anymore */
    TEST_ASSERT_EQUAL_INT(0, _in_flight_frags);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_send__first_ackd(void)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    congure_mock_snd_t *c;
    congure_snd_msg_t *msgs = NULL;
    BITFIELD(ack_bitmap, 32U) = { 0 };
    bool sent_frags_correct, ack_received;
    uint8_t tag;
    static const uint8_t ack_req = 1 << TEST_SEND_FRAG_SEQ3;
    static const uint8_t acked_frags = 1 << TEST_SEND_FRAG_SEQ1;

    _test_initial_send(acked_frags, ack_req, &tag, ack_bitmap,
                       &sent_frags_correct);
    TEST_ASSERT(sent_frags_correct);
    TEST_ASSERT_FBUF_CONGURE_EXISTS(fbuf, c);
    /* three fragments wait for potential re-sending */
    TEST_ASSERT_EQUAL_INT(3U, clist_count(&fbuf->sfr.window));
    _recv_ack(ack_bitmap, tag, false, &ack_received);
    TEST_ASSERT(ack_received);
    /* resend of fragment 2 (fragment 1 is ACK'd so not resent) */
    _test_send_frag(TEST_SEND_FRAG_SEQ2, acked_frags, ack_req, &tag, ack_bitmap,
                    &sent_frags_correct);
    TEST_ASSERT(sent_frags_correct);
    /* resend of fragment 3 */
    _test_send_frag(TEST_SEND_FRAG_SEQ3, 0, ack_req, &tag,
                    NULL, &sent_frags_correct);
    TEST_ASSERT(sent_frags_correct);
    TEST_ASSERT_FBUF_CONGURE_EXISTS(fbuf, c);
    /* two fragments wait for potential re-sending */
    TEST_ASSERT_EQUAL_INT(2U, clist_count(&fbuf->sfr.window));
    /* fragments 2 and 3 were reported lost (in one call) */
    TEST_ASSERT_REPORT_MSGS_LOST(c, 1U, msgs);
    /* msgs is `not_received` in SFR code which is allocated on stack,
     * so we can't safely check the contents */
    /* fragment 1 was ACK'd */
    TEST_ASSERT_REPORT_MSG_ACKED(c, 1U);
    /* report_msg_sent has been called 5 times:
     * - 3 for each initial send of the fragments
     * - 2 for resends of fragments 2 and 3 which were marked lost in the ACK
     */
    TEST_ASSERT_REPORT_MSG_SENT(c, 5U);
    TEST_ASSERT_FBUF_CONGURE_EXISTS(fbuf, c);
    /* Due to the ACK, we only expect RETRIES - 1 retries so start iteration
     * with 1 */
    for (unsigned i = 1U; i < CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES; i++) {
        gnrc_sixlowpan_frag_sfr_arq_timeout(fbuf);
        /* resend of fragment 3 */
        _test_send_frag(TEST_SEND_FRAG_SEQ3, 0, ack_req, &tag,
                        NULL, &sent_frags_correct);
        TEST_ASSERT(sent_frags_correct);
        TEST_ASSERT_FBUF_CONGURE_EXISTS(fbuf, c);
        /* two fragments wait for potential re-sending */
        TEST_ASSERT_EQUAL_INT(2U, clist_count(&fbuf->sfr.window));
        /* nothing should have changed for report_msgs_lost */
        TEST_ASSERT_REPORT_MSGS_LOST(c, 1U, msgs);
        /* nothing should have changed for report_msg_acked */
        TEST_ASSERT_REPORT_MSG_ACKED(c, 1U);
        /* report_msg_sent has been called 5 + (2 * i) times:
         * - 3 for each initial send of the fragments
         * - 2 for resends of fragments 2 and 3 which were marked lost in the ACK
         * - 3 each retry (while only fragment 3 is sent, 2 is also marked as
         *   in flight)
         */
        TEST_ASSERT_REPORT_MSG_SENT(c, 5U + (2U * i));
        /* report_msg_timeout was called i times; once for each ARQ
         * timeout */
        TEST_ASSERT_REPORT_MSGS_TIMEOUT(c, i, msgs);
        /* fbuf and thus msgs will be deleted in second to last iteration due to
         * fragment 3 having been resent too many times already. */
        if (i <= (CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES - 1)) {
            /* there is one message for each fragment 2 and 3 */
            TEST_ASSERT_EQUAL_INT(2U, clist_count(&msgs->super));
            clist_foreach(&msgs->super, _check_congure_snd_msgs_list, NULL);
            /* second fragment has 1 resends (due to the ACK) */
            TEST_ASSERT_EQUAL_INT(
                1, ((congure_snd_msg_t *)msgs->super.next->next)->resends
            );
            /* third fragment has i + 1 resends (one due to the ACK, one due to
             * the timeout) */
            TEST_ASSERT_EQUAL_INT(
                i + 1,
                ((congure_snd_msg_t *)msgs->super.next->next->next)->resends
            );
        }
    }
    /* generate final timeout */
    gnrc_sixlowpan_frag_sfr_arq_timeout(fbuf);
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag3)));
    /* nothing should have changed for report_msgs_lost */
    TEST_ASSERT_REPORT_MSGS_LOST(c, 1U, msgs);
    /* nothing should have changed for report_msg_acked */
    TEST_ASSERT_REPORT_MSG_ACKED(c, 1U);
    /* report_msg_timeout was called CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES
     * times; once for each ARQ timeout */
    TEST_ASSERT_REPORT_MSGS_TIMEOUT(
        c, CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES, msgs
    );
    /* report_msg_sent has been called 3 + (2 * RETRIES) times:
     * - 3 for each initial send of the fragments
     * - 2 each retry due to ACK or timeout (1 was ACK'd so is not sent anymore
     *   and while only fragment 3 is sent, 2 is marked as in flight)
     */
    TEST_ASSERT_REPORT_MSG_SENT(
        c, 3U + (2U * CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES)
    );
    /* fragmentation buffer and congure state should have been destroyed and
     * freed after CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES retries */
    TEST_ASSERT_FBUF_CONGURE_DESTROYED(fbuf, c);
    /* inter_msg_interval was called several times to calculate inter-frame gap
     */
    TEST_ASSERT(0 < c->inter_msg_interval_calls);
    /* none of the other CongURE report methods should have been called */
    TEST_ASSERT_EQUAL_INT(0, c->report_msg_discarded_calls);
    TEST_ASSERT_EQUAL_INT(0, c->report_ecn_ce_calls);
    /* no fragments reported in flight anymore */
    TEST_ASSERT_EQUAL_INT(0, _in_flight_frags);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_send__last_ackd_ecn(void)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    congure_mock_snd_t *c;
    congure_snd_msg_t *msgs = NULL;
    BITFIELD(ack_bitmap, 32U) = { 0 };
    uint32_t pre_send_time;
    bool sent_frags_correct, ack_received;
    uint8_t tag;
    static const uint8_t ack_req = 1 << TEST_SEND_FRAG_SEQ3;
    static const uint8_t acked_frags = 1 << TEST_SEND_FRAG_SEQ3;

    pre_send_time = xtimer_now_usec() / US_PER_MS;
    _test_initial_send(acked_frags, ack_req, &tag, ack_bitmap,
                       &sent_frags_correct);
    TEST_ASSERT(sent_frags_correct);
    TEST_ASSERT_FBUF_CONGURE_EXISTS(fbuf, c);
    TEST_ASSERT_EQUAL_INT(3U, clist_count(&fbuf->sfr.window));
    _recv_ack(ack_bitmap, tag, true, &ack_received);
    TEST_ASSERT(ack_received);
    /* resend of fragment 1 */
    _test_send_frag(TEST_SEND_FRAG_SEQ1, acked_frags, ack_req, &tag, ack_bitmap,
                    &sent_frags_correct);
    TEST_ASSERT(sent_frags_correct);
    /* resend of fragment 2 (which now requests an ACK) */
    _test_send_frag(TEST_SEND_FRAG_SEQ2, 0, 1 << TEST_SEND_FRAG_SEQ2, &tag,
                    NULL, &sent_frags_correct);
    TEST_ASSERT(sent_frags_correct);
    /* (fragment 3 is ACK'd so not resent) */
    TEST_ASSERT_FBUF_CONGURE_EXISTS(fbuf, c);
    /* two fragments wait for potential re-sending */
    TEST_ASSERT_EQUAL_INT(2U, clist_count(&fbuf->sfr.window));
    /* fragments 1 and 2 were reported lost (in one call) */
    TEST_ASSERT_REPORT_MSGS_LOST(c, 1U, msgs);
    /* msgs is `not_received` in SFR code which is allocated on stack,
     * so we can't safely check the contents */
    /* fragment 3 was ACK'd */
    TEST_ASSERT_REPORT_MSG_ACKED(c, 1U);
    /* report_ecn_ce was called with a timestamp after the time we sent the
     * original message */
    TEST_ASSERT_EQUAL_INT(1U, c->report_ecn_ce_calls);
    TEST_ASSERT((xtimer_now_usec() / US_PER_MS) >= pre_send_time);
    TEST_ASSERT(c->report_ecn_ce_args.time >= pre_send_time);
    /* report_msg_sent has been called 5 times:
     * - 3 for each initial send of the fragments
     * - 2 for resends of fragments 2 and 3 which were marked lost in the ACK
     */
    TEST_ASSERT_REPORT_MSG_SENT(c, 5U);
    /* Due to the ACK, we only expect RETRIES - 1 retries so start iteration
     * with 1 */
    for (unsigned i = 1U; i < CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES; i++) {
        gnrc_sixlowpan_frag_sfr_arq_timeout(fbuf);
        /* resend of fragment 2 */
        _test_send_frag(TEST_SEND_FRAG_SEQ2, 0, 1 << TEST_SEND_FRAG_SEQ2, &tag,
                        NULL, &sent_frags_correct);
        TEST_ASSERT(sent_frags_correct);
        TEST_ASSERT_FBUF_CONGURE_EXISTS(fbuf, c);
        /* two fragments wait for potential re-sending */
        TEST_ASSERT_EQUAL_INT(2U, clist_count(&fbuf->sfr.window));
        /* nothing should have changed for report_msgs_lost */
        TEST_ASSERT_REPORT_MSGS_LOST(c, 1U, msgs);
        /* nothing should have changed for report_msg_acked */
        TEST_ASSERT_REPORT_MSG_ACKED(c, 1U);
        /* report_msg_sent has been called 5 + (2 * i) times:
         * - 3 for each initial send of the fragments
         * - 2 for resends of fragments 1 and 2 which were marked lost in the ACK
         * - 3 each retry (while only fragment 3 is sent, 2 is also marked as
         *   in flight)
         */
        TEST_ASSERT_REPORT_MSG_SENT(c, 5U + (2U * i));
        /* report_msg_timeout was called i times; once for each ARQ
         * timeout */
        TEST_ASSERT_REPORT_MSGS_TIMEOUT(c, i, msgs);
        /* fbuf and thus msgs will be deleted in second to last iteration due to
         * fragment 3 having been resent too many times already. */
        if (i <= (CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES - 1)) {
            /* there is one message for each fragment 2 and 3 */
            TEST_ASSERT_EQUAL_INT(2U, clist_count(&msgs->super));
            clist_foreach(&msgs->super, _check_congure_snd_msgs_list, NULL);
            /* second fragment has 1 resends (due to the ACK) */
            TEST_ASSERT_EQUAL_INT(
                1, ((congure_snd_msg_t *)msgs->super.next->next)->resends
            );
            /* third fragment has i + 1 resends (one due to the ACK, one due to
             * the timeout) */
            TEST_ASSERT_EQUAL_INT(
                i + 1,
                ((congure_snd_msg_t *)msgs->super.next->next->next)->resends
            );
        }
    }
    /* generate final timeout */
    gnrc_sixlowpan_frag_sfr_arq_timeout(fbuf);
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag3)));
    /* nothing should have changed for report_msgs_lost */
    TEST_ASSERT_REPORT_MSGS_LOST(c, 1U, msgs);
    /* nothing should have changed for report_msg_acked */
    TEST_ASSERT_REPORT_MSG_ACKED(c, 1U);
    /* report_msg_timeout was called CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES
     * times; once for each ARQ timeout */
    TEST_ASSERT_REPORT_MSGS_TIMEOUT(
        c, CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES, msgs
    );
    /* report_msg_sent has been called 3 + (2 * RETRIES) times:
     * - 3 for each initial send of the fragments
     * - 2 each retry due to ACK or timeout (1 was ACK'd so is not sent anymore
     *   and while only fragment 2 is sent, 1 is marked as in flight)
     */
    TEST_ASSERT_REPORT_MSG_SENT(
        c, 3U + (2U * CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES)
    );
    /* fragmentation buffer and congure state should have been destroyed and
     * freed after CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES retries */
    TEST_ASSERT_FBUF_CONGURE_DESTROYED(fbuf, c);
    /* inter_msg_interval was called several times to calculate inter-frame gap
     */
    TEST_ASSERT(0 < c->inter_msg_interval_calls);
    /* none of the other CongURE report methods should have been called */
    TEST_ASSERT_EQUAL_INT(0, c->report_msg_discarded_calls);
    /* no fragments reported in flight anymore */
    TEST_ASSERT_EQUAL_INT(0, _in_flight_frags);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void _test_sixlo_send__all_or_FULL_ackd(const uint8_t acked_frags)
{
    gnrc_sixlowpan_frag_fb_t *fbuf;
    BITFIELD(ack_bitmap, 32U) = { 0 };
    congure_mock_snd_t *c;
    bool sent_frags_correct, ack_received;
    uint8_t tag;
    static const uint8_t ack_req = 1 << TEST_SEND_FRAG_SEQ3;

    if (acked_frags == TEST_FULL_ACK) {
        _test_initial_send(0, ack_req, &tag, NULL, &sent_frags_correct);
    }
    else {
        _test_initial_send(acked_frags, ack_req, &tag, ack_bitmap,
                           &sent_frags_correct);
    }
    TEST_ASSERT(sent_frags_correct);
    TEST_ASSERT_FBUF_CONGURE_EXISTS(fbuf, c);
    if (acked_frags == TEST_FULL_ACK) {
        _recv_ack((uint8_t *)TEST_FULL_ACK, tag, false, &ack_received);
    }
    else {
        _recv_ack(ack_bitmap, tag, false, &ack_received);
    }
    TEST_ASSERT(ack_received);
    /* all three fragments were reported as sent exactly once */
    TEST_ASSERT_REPORT_MSG_SENT(c, 3U);
    /* all three fragments were ACK'd */
    TEST_ASSERT_REPORT_MSG_ACKED(c, 3U);
    /* should time out */
    TEST_ASSERT_EQUAL_INT(0, _wait_for_packet(sizeof(_test_send_frag3)));
    /* fragmentation buffer and congure state should have been destroyed and
     * freed after CONFIG_GNRC_SIXLOWPAN_SFR_FRAG_RETRIES retries */
    TEST_ASSERT_FBUF_CONGURE_DESTROYED(fbuf, c);
    /* inter_msg_interval was called several times to calculate inter-frame gap
     */
    TEST_ASSERT(0 < c->inter_msg_interval_calls);
    /* none of the other CongURE report methods should have been called */
    TEST_ASSERT_EQUAL_INT(0, c->report_msg_discarded_calls);
    TEST_ASSERT_EQUAL_INT(0, c->report_msgs_timeout_calls);
    TEST_ASSERT_EQUAL_INT(0, c->report_msgs_lost_calls);
    TEST_ASSERT_EQUAL_INT(0, c->report_msgs_timeout_calls);
    /* no fragments reported in flight anymore */
    TEST_ASSERT_EQUAL_INT(0, _in_flight_frags);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_sixlo_send__all_ackd(void)
{
    _test_sixlo_send__all_or_FULL_ackd(
            (1 << TEST_SEND_FRAG_SEQ1) |
            (1 << TEST_SEND_FRAG_SEQ2) |
            (1 << TEST_SEND_FRAG_SEQ3)
        );
}

static void test_sixlo_send__FULL_ack_recv(void)
{
    _test_sixlo_send__all_or_FULL_ackd(TEST_FULL_ACK);
}

static Test *tests_gnrc_sixlowpan_frag_sfr_congure_integration(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_sixlo_send__no_ack),
        new_TestFixture(test_sixlo_send__first_ackd),
        new_TestFixture(test_sixlo_send__last_ackd_ecn),
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
    TESTS_RUN(tests_gnrc_sixlowpan_frag_sfr_congure_integration());
    TESTS_END();
    return 0;
}

static void _congure_init(congure_snd_t *cong, void *ctx)
{
    cong->ctx = ctx;
    cong->cwnd = CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE;
}

static void _congure_report_msg_sent(congure_snd_t *cong, unsigned msg_size)
{
    (void)cong;
    _in_flight_frags += msg_size;
}

static void _congure_report_msg_discarded(congure_snd_t *cong,
                                          unsigned msg_size)
{
    (void)cong;
    _in_flight_frags -= msg_size;
}

static int _report_msg_discarded(clist_node_t *node, void *arg)
{
    congure_snd_msg_t *msg = container_of(node, congure_snd_msg_t, super);
    _congure_report_msg_discarded(arg, msg->size);
    return 0;
}

static void _congure_report_msgs_timeout_lost(congure_snd_t *cong,
                                             congure_snd_msg_t *msgs)
{
    (void)cong;
    clist_foreach(&msgs->super, _report_msg_discarded, cong);
}

static void _congure_report_msg_acked(congure_snd_t *cong,
                                      congure_snd_msg_t *msg,
                                      congure_snd_ack_t *ack)
{
    (void)cong;
    (void)ack;
    _in_flight_frags -= msg->size;
}

static congure_mock_snd_t *_get_congure_by_fb(const gnrc_sixlowpan_frag_fb_t *fb)
{
    for (unsigned i = 0; i < ARRAY_SIZE(_sfr_congure_mocks); i++) {
        if ((_sfr_congure_mocks[i].super.driver != NULL) &&
            /* ctx is set to the fragment buffer */
            (_sfr_congure_mocks[i].super.ctx == fb)) {
            return &_sfr_congure_mocks[i];
        }
    }
    return NULL;
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

static void _check_send_frag_datagram_fields(size_t mhr_len)
{
    sixlowpan_sfr_rfrag_t *frag_hdr = (sixlowpan_sfr_rfrag_t *)&_target_buf[mhr_len];

    if (sixlowpan_sfr_rfrag_get_seq(frag_hdr) == 0) {
        TEST_ASSERT_EQUAL_INT(TEST_SEND_COMP_DATAGRAM_SIZE,
                              sixlowpan_sfr_rfrag_get_offset(frag_hdr));
    }
}

static void _check_send_frag1(size_t mhr_len, bool ack_req, bool *finished)
{
    sixlowpan_sfr_rfrag_t *frag_hdr;

    *finished = false;
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
    *finished = true;
}

static void _check_send_frag2(size_t mhr_len, bool ack_req, bool *finished)
{
    sixlowpan_sfr_rfrag_t *frag_hdr;

    *finished = false;
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
    *finished = true;
}

static void _check_send_frag3(size_t mhr_len, bool ack_req, bool *finished)
{
    sixlowpan_sfr_rfrag_t *frag_hdr;

    *finished = false;
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
    *finished = true;
}

static void _check_congure_snd_msg(congure_snd_msg_t *msg)
{
    TEST_ASSERT(msg->send_time > 0U);
    TEST_ASSERT_EQUAL_INT(msg->size, 1);
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
