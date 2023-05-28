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
 * @brief       Tests 6LoWPAN fragmentation handling of gnrc stack.
 *
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include "embUnit.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/sixlowpan/frag.h"
#include "net/gnrc/sixlowpan/frag/rb.h"
#include "xtimer.h"

#define TEST_NETIF_HDR_SRC      { 0xb3, 0x47, 0x60, 0x49, \
                                  0x78, 0xfe, 0x95, 0x48 }
#define TEST_NETIF_HDR_DST      { 0xa4, 0xf2, 0xd2, 0xc9, \
                                  0x13, 0xb9, 0xbb, 0x25 }
#define TEST_NETIF_IFACE        (9)
#define TEST_TAG                (0x690e)
#define TEST_PAGE               (0)
#define TEST_RECEIVE_TIMEOUT    (100U)
#define TEST_GC_TIMEOUT         (CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US + TEST_RECEIVE_TIMEOUT)

/* test date taken from an experimental run (uncompressed ICMPv6 echo reply with
 * 300 byte payload)*/
#define TEST_DATAGRAM_SIZE      (348U)
#ifdef MODULE_GNRC_IPV6
#define TEST_DATAGRAM_NETTYPE   (GNRC_NETTYPE_IPV6)
#else  /* MODULE_GNRC_IPV6 */
#define TEST_DATAGRAM_NETTYPE   (GNRC_NETTYPE_UNDEF)
#endif /* MODULE_GNRC_IPV6 */
#define TEST_FRAGMENT1_OFFSET   (0U)
#define TEST_FRAGMENT2_OFFSET   (96U)
#define TEST_FRAGMENT3_OFFSET   (192U)
#define TEST_FRAGMENT4_OFFSET   (288U)
#define TEST_FRAGMENT1 { \
        0xc1, 0x5c, 0x00, 0x05, 0x41, 0x60, 0x00, 0x00, \
        0x00, 0x01, 0x34, 0x3a, 0x40, 0xfe, 0x80, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x7b, 0x65, 0x08, \
        0x22, 0x86, 0x93, 0x9d, 0x5a, 0xfe, 0x80, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x7b, 0x79, 0x7f, \
        0x7f, 0xa4, 0xb1, 0x55, 0x2e, 0x81, 0x00, 0x7a, \
        0x81, 0x00, 0x54, 0x00, 0x02, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54                    \
    }
#define TEST_FRAGMENT2 { \
        0xe1, 0x5c, 0x00, 0x05, 0x0c, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54                    \
    }
#define TEST_FRAGMENT3 { \
        0xe1, 0x5c, 0x00, 0x05, 0x18, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54                    \
    }
#define TEST_FRAGMENT4 { \
        0xe1, 0x5c, 0x00, 0x05, 0x24, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54                                            \
}
#define TEST_DATAGRAM { \
        0x60, 0x00, 0x00, 0x00, 0x01, 0x34, 0x3a, 0x40, \
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
        0x7b, 0x65, 0x08, 0x22, 0x86, 0x93, 0x9d, 0x5a, \
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
        0x7b, 0x79, 0x7f, 0x7f, 0xa4, 0xb1, 0x55, 0x2e, \
        0x81, 0x00, 0x7a, 0x81, 0x00, 0x54, 0x00, 0x02, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, \
        0x54, 0x54, 0x54, 0x54 \
}

static const uint8_t _test_netif_hdr_src[] = TEST_NETIF_HDR_SRC;
static const uint8_t _test_netif_hdr_dst[] = TEST_NETIF_HDR_SRC;
static struct {
    gnrc_netif_hdr_t hdr;
    uint8_t src[GNRC_NETIF_HDR_L2ADDR_MAX_LEN];
    uint8_t dst[GNRC_NETIF_HDR_L2ADDR_MAX_LEN];
} _test_netif_hdr;

static uint8_t _fragment1[] = TEST_FRAGMENT1;
static uint8_t _fragment2[] = TEST_FRAGMENT2;
static uint8_t _fragment3[] = TEST_FRAGMENT3;
static uint8_t _fragment4[] = TEST_FRAGMENT4;
static const uint8_t _datagram[] = TEST_DATAGRAM;
static msg_t _msg_queue;

static inline void _set_fragment_tag(void *frag, uint16_t tag)
{
    sixlowpan_frag_t *f = frag;

    f->tag = byteorder_htons(tag);
}

static inline void _set_fragment_offset(void *frag, uint16_t offset)
{
    sixlowpan_frag_n_t *f = frag;

    TEST_ASSERT_EQUAL_INT(SIXLOWPAN_FRAG_N_DISP,
                          f->disp_size.u8[0] & SIXLOWPAN_FRAG_DISP_MASK);
    f->offset = offset / 8;
}

static void _set_up(void)
{
    gnrc_sixlowpan_frag_rb_reset();
    gnrc_pktbuf_init();
    gnrc_netif_hdr_init(&_test_netif_hdr.hdr,
                        GNRC_NETIF_HDR_L2ADDR_MAX_LEN,
                        GNRC_NETIF_HDR_L2ADDR_MAX_LEN);
    _test_netif_hdr.hdr.if_pid = TEST_NETIF_IFACE;
    gnrc_netif_hdr_set_src_addr(&_test_netif_hdr.hdr,
                                (uint8_t *)_test_netif_hdr_src,
                                sizeof(_test_netif_hdr_src));
    gnrc_netif_hdr_set_dst_addr(&_test_netif_hdr.hdr,
                                (uint8_t *)_test_netif_hdr_dst,
                                sizeof(_test_netif_hdr_dst));
    _set_fragment_tag(_fragment1, TEST_TAG);
    _set_fragment_tag(_fragment2, TEST_TAG);
    _set_fragment_tag(_fragment3, TEST_TAG);
    _set_fragment_tag(_fragment4, TEST_TAG);
    _set_fragment_offset(_fragment2, TEST_FRAGMENT2_OFFSET);
    _set_fragment_offset(_fragment3, TEST_FRAGMENT3_OFFSET);
    _set_fragment_offset(_fragment4, TEST_FRAGMENT4_OFFSET);
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

static void _test_entry(const gnrc_sixlowpan_frag_rb_t *entry,
                        unsigned exp_current_size,
                        unsigned exp_int_start, unsigned exp_int_end)
{
    TEST_ASSERT_NOT_NULL(entry);
    TEST_ASSERT_NOT_NULL(entry->pkt);
    TEST_ASSERT_EQUAL_INT(TEST_DATAGRAM_SIZE, entry->pkt->size);
    TEST_ASSERT_EQUAL_INT(sizeof(_test_netif_hdr_src),
                          entry->super.src_len);
    TEST_ASSERT_MESSAGE(memcmp(entry->super.src, _test_netif_hdr_src,
                               entry->super.src_len) == 0,
                        "entry->super.src != TEST_NETIF_HDR_SRC");
    TEST_ASSERT_EQUAL_INT(sizeof(_test_netif_hdr_dst),
                          entry->super.dst_len);
    TEST_ASSERT_MESSAGE(memcmp(entry->super.dst, _test_netif_hdr_dst,
                               entry->super.dst_len) == 0,
                        "entry->super.dst != TEST_NETIF_HDR_DST");
    TEST_ASSERT_EQUAL_INT(TEST_TAG, entry->super.tag);
    TEST_ASSERT_EQUAL_INT(exp_current_size, entry->super.current_size);
    TEST_ASSERT_NOT_NULL(entry->super.ints);
    TEST_ASSERT_NULL(entry->super.ints->next);
    TEST_ASSERT_EQUAL_INT(exp_int_start, entry->super.ints->start);
    TEST_ASSERT_EQUAL_INT(exp_int_end, entry->super.ints->end);
}

static void _check_pktbuf(const gnrc_sixlowpan_frag_rb_t *entry)
{
    if (entry != NULL) {
        gnrc_pktbuf_release(entry->pkt);
    }
    TEST_ASSERT_MESSAGE(gnrc_pktbuf_is_empty(), "Packet buffer is not empty");
}

static void _rbuf_create_first_fragment(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, _fragment1, sizeof(_fragment1),
                                          GNRC_NETTYPE_SIXLOWPAN);
    const gnrc_sixlowpan_frag_rb_t *entry;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL((entry = gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt, TEST_FRAGMENT1_OFFSET, TEST_PAGE
        )));
    /* current_size must be the offset of fragment 2, not the size of
     * fragment 1 (fragment dispatch was removed, IPHC was applied etc.). */
    _test_entry(entry, TEST_FRAGMENT2_OFFSET,
                TEST_FRAGMENT1_OFFSET, TEST_FRAGMENT2_OFFSET - 1);
}

static void test_rbuf_add__success_first_fragment(void)
{
    const gnrc_sixlowpan_frag_rb_t *entry;

    _rbuf_create_first_fragment();
    /* get entry to release entry->pkt it in `_check_pktbuf()` */
    TEST_ASSERT_NOT_NULL((entry = _first_non_empty_rbuf()));
    _check_pktbuf(entry);
}

static void test_rbuf_add__success_subsequent_fragment(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, _fragment2, sizeof(_fragment2),
                                          GNRC_NETTYPE_SIXLOWPAN);
    const gnrc_sixlowpan_frag_rb_t *entry;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL((entry = gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt, TEST_FRAGMENT2_OFFSET, TEST_PAGE
        )));
    /* current_size must be the offset of fragment 3, not the size of
     * fragment 2 (fragment dispatch was removed, IPHC was applied etc.). */
    _test_entry(entry, TEST_FRAGMENT3_OFFSET - TEST_FRAGMENT2_OFFSET,
                TEST_FRAGMENT2_OFFSET, TEST_FRAGMENT3_OFFSET - 1);
    _check_pktbuf(entry);
}

static void test_rbuf_add__success_duplicate_fragments(void)
{
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, _fragment3, sizeof(_fragment3),
                                           GNRC_NETTYPE_SIXLOWPAN);
    gnrc_pktsnip_t *pkt2 = gnrc_pktbuf_add(NULL, _fragment3, sizeof(_fragment3),
                                           GNRC_NETTYPE_SIXLOWPAN);
    const gnrc_sixlowpan_frag_rb_t *entry;

    TEST_ASSERT_NOT_NULL(pkt1);
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt1, TEST_FRAGMENT3_OFFSET, TEST_PAGE
        ));
    TEST_ASSERT_NOT_NULL(pkt2);
    TEST_ASSERT_NOT_NULL((entry = gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt2, TEST_FRAGMENT3_OFFSET, TEST_PAGE
        )));
    /* current_size must be the offset of fragment 4, not the size of
     * fragment 3 (fragment dispatch was removed, IPHC was applied etc.). */
    _test_entry(entry, TEST_FRAGMENT4_OFFSET - TEST_FRAGMENT3_OFFSET,
                TEST_FRAGMENT3_OFFSET, TEST_FRAGMENT4_OFFSET - 1);
    _check_pktbuf(entry);
}

static void test_rbuf_add__success_complete(void)
{
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, _fragment1, sizeof(_fragment1),
                                           GNRC_NETTYPE_SIXLOWPAN);
    gnrc_pktsnip_t *pkt2 = gnrc_pktbuf_add(NULL, _fragment2, sizeof(_fragment2),
                                           GNRC_NETTYPE_SIXLOWPAN);
    gnrc_pktsnip_t *pkt3 = gnrc_pktbuf_add(NULL, _fragment3, sizeof(_fragment3),
                                           GNRC_NETTYPE_SIXLOWPAN);
    gnrc_pktsnip_t *pkt4 = gnrc_pktbuf_add(NULL, _fragment4, sizeof(_fragment4),
                                           GNRC_NETTYPE_SIXLOWPAN);
    gnrc_pktsnip_t *datagram;
    gnrc_sixlowpan_frag_rb_t *entry1, *entry2;
    msg_t msg = { .type = 0U };
    gnrc_netreg_entry_t reg = GNRC_NETREG_ENTRY_INIT_PID(
            GNRC_NETREG_DEMUX_CTX_ALL,
            thread_getpid()
        );

    gnrc_netreg_register(TEST_DATAGRAM_NETTYPE, &reg);
    /* Mixing up things. Order decided by fair dice-rolls ;-) */
    TEST_ASSERT_NOT_NULL(pkt2);
    TEST_ASSERT_NOT_NULL((entry1 = gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt2, TEST_FRAGMENT2_OFFSET, TEST_PAGE
        )));
    TEST_ASSERT_EQUAL_INT(0, gnrc_sixlowpan_frag_rb_dispatch_when_complete(
            entry1, &_test_netif_hdr.hdr
        ));
    TEST_ASSERT_NOT_NULL(pkt4);
    TEST_ASSERT_NOT_NULL((entry2 = gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt4, TEST_FRAGMENT4_OFFSET, TEST_PAGE
        )));
    TEST_ASSERT(entry1 == entry2);
    TEST_ASSERT_EQUAL_INT(0, gnrc_sixlowpan_frag_rb_dispatch_when_complete(
            entry1, &_test_netif_hdr.hdr
        ));
    TEST_ASSERT_NOT_NULL(pkt1);
    TEST_ASSERT_NOT_NULL((entry2 = gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt1, TEST_FRAGMENT1_OFFSET, TEST_PAGE
        )));
    TEST_ASSERT(entry1 == entry2);
    TEST_ASSERT_EQUAL_INT(0, gnrc_sixlowpan_frag_rb_dispatch_when_complete(
            entry1, &_test_netif_hdr.hdr
        ));
    TEST_ASSERT_NOT_NULL(pkt3);
    TEST_ASSERT_NOT_NULL((entry2 = gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt3, TEST_FRAGMENT3_OFFSET, TEST_PAGE
        )));
    TEST_ASSERT(entry1 == entry2);
    TEST_ASSERT(0 < gnrc_sixlowpan_frag_rb_dispatch_when_complete(
            entry1, &_test_netif_hdr.hdr
        ));
    TEST_ASSERT_MESSAGE(
            xtimer_msg_receive_timeout(&msg, TEST_RECEIVE_TIMEOUT) >= 0,
            "Receiving reassembled datagram timed out"
        );
    gnrc_netreg_unregister(TEST_DATAGRAM_NETTYPE, &reg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_RCV, msg.type);
    TEST_ASSERT_NOT_NULL(msg.content.ptr);
    datagram = msg.content.ptr;
    TEST_ASSERT_EQUAL_INT(TEST_DATAGRAM_SIZE, datagram->size);
    TEST_ASSERT_EQUAL_INT(TEST_DATAGRAM_NETTYPE, datagram->type);
    TEST_ASSERT_MESSAGE(memcmp(_datagram, datagram->data,
                        TEST_DATAGRAM_SIZE) == 0,
                        "Reassembled datagram does not contain expected data");
    gnrc_pktbuf_release(datagram);
    _check_pktbuf(NULL);
}

static void test_rbuf_add__full_rbuf(void)
{
    gnrc_pktsnip_t *pkt;
    const gnrc_sixlowpan_frag_rb_t *rbuf;

    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE; i++) {
        pkt = gnrc_pktbuf_add(NULL, _fragment1, sizeof(_fragment1),
                              GNRC_NETTYPE_SIXLOWPAN);
        TEST_ASSERT_NOT_NULL(pkt);
        TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt, TEST_FRAGMENT1_OFFSET, TEST_PAGE
        ));
        _set_fragment_tag(_fragment1, TEST_TAG + i + 1);
        /* pkt is released in gnrc_sixlowpan_frag_rb_add() */
    }
    pkt = gnrc_pktbuf_add(NULL, _fragment1, sizeof(_fragment1),
                          GNRC_NETTYPE_SIXLOWPAN);
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt, TEST_FRAGMENT1_OFFSET, TEST_PAGE
        ));
    rbuf = gnrc_sixlowpan_frag_rb_array();
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE; i++) {
        const gnrc_sixlowpan_frag_rb_t *entry = &rbuf[i];

        TEST_ASSERT_MESSAGE(!gnrc_sixlowpan_frag_rb_entry_empty(entry),
                            "Reassembly buffer entry unexpectedly empty");
        TEST_ASSERT((sizeof(_fragment1) - sizeof(sixlowpan_frag_t)) <
                    entry->pkt->size);
        TEST_ASSERT_MESSAGE(
                memcmp(entry->pkt->data, &_fragment1[sizeof(sixlowpan_frag_t)],
                       sizeof(_fragment1) - sizeof(sixlowpan_frag_t)) != 0,
                "Reassembly buffer contains fragment that was not supposed "
                "to fit");
        /* releasing pkt to check if packet buffer is empty in the end */
        gnrc_pktbuf_release(entry->pkt);
    }
    _check_pktbuf(NULL);
}

static void test_rbuf_add__too_big_fragment(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, _fragment1,
                                          /* something definitely bigger than
                                           * the datagram size noted in
                                           * _fragment1, can't just be + 1,
                                           * since fragment dispatch and other
                                           * dispatches are supposed to be
                                           * subtracted */
                                          2 * TEST_DATAGRAM_SIZE,
                                          GNRC_NETTYPE_SIXLOWPAN);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt, TEST_FRAGMENT1_OFFSET, TEST_PAGE
        ));
    /* packet buffer is empty*/
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    _check_pktbuf(NULL);
}

static void test_rbuf_add__overlap_lhs(void)
{
    static const size_t pkt2_offset = TEST_FRAGMENT2_OFFSET - 8U;
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, _fragment1, sizeof(_fragment1),
                                           GNRC_NETTYPE_SIXLOWPAN);
    gnrc_pktsnip_t *pkt2;
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    unsigned rbuf_entries = 0;

    _set_fragment_offset(_fragment2, pkt2_offset);
    pkt2 = gnrc_pktbuf_add(NULL, _fragment2, sizeof(_fragment2),
                           GNRC_NETTYPE_SIXLOWPAN);
    TEST_ASSERT_NOT_NULL(pkt1);
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt1, TEST_FRAGMENT1_OFFSET, TEST_PAGE
        ));
    TEST_ASSERT_NOT_NULL(pkt2);
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt2, pkt2_offset, TEST_PAGE
        ));
    rbuf = gnrc_sixlowpan_frag_rb_array();
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE; i++) {
        const gnrc_sixlowpan_frag_rb_t *entry = &rbuf[i];
        if (!gnrc_sixlowpan_frag_rb_entry_empty(entry)) {
            static const size_t pkt3_offset = TEST_FRAGMENT3_OFFSET - 8U - 1;

            rbuf_entries++;
            /* only _fragment2 should now in the reassembly buffer according to
             * https://tools.ietf.org/html/rfc4944#section-5.3 */
            _test_entry(entry,
                        /* current_size must be the offset of fragment 3, not
                         * the size of fragment 2 (fragment dispatch was
                         * removed, IPHC was applied etc.). */
                        TEST_FRAGMENT3_OFFSET - TEST_FRAGMENT2_OFFSET,
                        (unsigned)pkt2_offset, (unsigned)pkt3_offset);
            /* releasing pkt to check if packet buffer is empty in the end */
            gnrc_pktbuf_release(entry->pkt);
        }
    }
    TEST_ASSERT_EQUAL_INT(1U, rbuf_entries);
    _check_pktbuf(NULL);
}

static void test_rbuf_add__overlap_rhs(void)
{
    static const size_t pkt2_offset = TEST_FRAGMENT2_OFFSET + 8U;
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, _fragment1, sizeof(_fragment1),
                                           GNRC_NETTYPE_SIXLOWPAN);
    gnrc_pktsnip_t *pkt2;
    gnrc_pktsnip_t *pkt3 = gnrc_pktbuf_add(NULL, _fragment3, sizeof(_fragment3),
                                           GNRC_NETTYPE_SIXLOWPAN);
    const gnrc_sixlowpan_frag_rb_t *rbuf;
    unsigned rbuf_entries = 0;

    _set_fragment_offset(_fragment2, pkt2_offset);
    pkt2 = gnrc_pktbuf_add(NULL, _fragment2, sizeof(_fragment2),
                           GNRC_NETTYPE_SIXLOWPAN);
    TEST_ASSERT_NOT_NULL(pkt1);
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt1, TEST_FRAGMENT1_OFFSET, TEST_PAGE
        ));
    TEST_ASSERT_NOT_NULL(pkt3);
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt3, TEST_FRAGMENT3_OFFSET, TEST_PAGE
        ));
    TEST_ASSERT_NOT_NULL(pkt2);
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt2, pkt2_offset, TEST_PAGE
        ));
    rbuf = gnrc_sixlowpan_frag_rb_array();
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE; i++) {
        const gnrc_sixlowpan_frag_rb_t *entry = &rbuf[i];
        if (!gnrc_sixlowpan_frag_rb_entry_empty(entry)) {
            static const size_t pkt3_offset = TEST_FRAGMENT3_OFFSET + 8U - 1U;

            rbuf_entries++;
            /* only _fragment2 should now in the reassembly buffer according to
             * https://tools.ietf.org/html/rfc4944#section-5.3 */
            _test_entry(entry,
                        /* current_size must be the offset of fragment 3, not
                         * the size of fragment 2 (fragment dispatch was
                         * removed, IPHC was applied etc.). */
                        TEST_FRAGMENT3_OFFSET - TEST_FRAGMENT2_OFFSET,
                        (unsigned)pkt2_offset, (unsigned)pkt3_offset);
            /* releasing pkt to check if packet buffer is empty in the end */
            gnrc_pktbuf_release(entry->pkt);
        }
    }
    TEST_ASSERT_EQUAL_INT(1U, rbuf_entries);
    _check_pktbuf(NULL);
}

static void test_rbuf_get_by_dg(void)
{
    const gnrc_sixlowpan_frag_rb_t *entry;

    TEST_ASSERT_NULL(
        gnrc_sixlowpan_frag_rb_get_by_datagram(&_test_netif_hdr.hdr, TEST_TAG)
    );
    /* add a fragment */
    _rbuf_create_first_fragment();
    TEST_ASSERT_NOT_NULL(
        gnrc_sixlowpan_frag_rb_get_by_datagram(&_test_netif_hdr.hdr, TEST_TAG)
    );
    /* get entry to release entry->pkt it in `_check_pktbuf()` */
    entry = _first_non_empty_rbuf();
    /* entry is however not properly removed yet */
    TEST_ASSERT_NOT_NULL(entry);
    _check_pktbuf(entry);
}

static void test_rbuf_exists(void)
{
    const gnrc_sixlowpan_frag_rb_t *entry;

    TEST_ASSERT(!gnrc_sixlowpan_frag_rb_exists(&_test_netif_hdr.hdr, TEST_TAG));
    /* add a fragment */
    _rbuf_create_first_fragment();
    TEST_ASSERT(gnrc_sixlowpan_frag_rb_exists(&_test_netif_hdr.hdr, TEST_TAG));
    /* get entry to release entry->pkt it in `_check_pktbuf()` */
    entry = _first_non_empty_rbuf();
    /* entry is however not properly removed yet */
    TEST_ASSERT_NOT_NULL(entry);
    _check_pktbuf(entry);
}

static void test_rbuf_rm_by_dg(void)
{
    /* add a fragment */
    _rbuf_create_first_fragment();
    gnrc_sixlowpan_frag_rb_rm_by_datagram(&_test_netif_hdr.hdr, TEST_TAG);
    TEST_ASSERT(!gnrc_sixlowpan_frag_rb_exists(&_test_netif_hdr.hdr, TEST_TAG));
    _check_pktbuf(NULL);
}

static void test_rbuf_rm(void)
{
    const gnrc_sixlowpan_frag_rb_t *entry;

    _rbuf_create_first_fragment();
    entry = _first_non_empty_rbuf();
    /* entry is however not properly removed yet */
    TEST_ASSERT_NOT_NULL(entry);
    /* release packet as `gnrc_sixlowpan_frag_rb_remove()` does not do this */
    gnrc_pktbuf_release(entry->pkt);
    /* intentionally discarding const qualifier since we enter rbuf's internal
     * context again */
    gnrc_sixlowpan_frag_rb_remove((gnrc_sixlowpan_frag_rb_t *)entry);
    /* reassembly buffer is now empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    _check_pktbuf(NULL);
}

static void test_rbuf_gc__manually(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, _fragment1, sizeof(_fragment1),
                                          GNRC_NETTYPE_SIXLOWPAN);
    gnrc_sixlowpan_frag_rb_t *entry;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL((entry = gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt, TEST_FRAGMENT1_OFFSET, TEST_PAGE
        )));
    TEST_ASSERT_NOT_NULL(entry);
    /* set arrival CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US into the past */
    entry->super.arrival -= CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US;
    gnrc_sixlowpan_frag_rb_gc();
    /* reassembly buffer is now empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    _check_pktbuf(NULL);
}

static void test_rbuf_gc__timed(void)
{
    msg_t msg;
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, _fragment1, sizeof(_fragment1),
                                          GNRC_NETTYPE_SIXLOWPAN);
    gnrc_sixlowpan_frag_rb_t *entry;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL((entry = gnrc_sixlowpan_frag_rb_add(
            &_test_netif_hdr.hdr, pkt, TEST_FRAGMENT1_OFFSET, TEST_PAGE
        )));
    TEST_ASSERT_NOT_NULL(entry);
    TEST_ASSERT_MESSAGE(
            xtimer_msg_receive_timeout(&msg, TEST_GC_TIMEOUT) >= 0,
            "Waiting for GC timer timed out"
        );
    TEST_ASSERT_EQUAL_INT(GNRC_SIXLOWPAN_FRAG_RB_GC_MSG, msg.type);
    gnrc_sixlowpan_frag_rb_gc();
    /* reassembly buffer is now empty */
    TEST_ASSERT_NULL(_first_non_empty_rbuf());
    _check_pktbuf(NULL);
}

static void run_unittests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_rbuf_add__success_first_fragment),
        new_TestFixture(test_rbuf_add__success_subsequent_fragment),
        new_TestFixture(test_rbuf_add__success_duplicate_fragments),
        new_TestFixture(test_rbuf_add__success_complete),
        new_TestFixture(test_rbuf_add__full_rbuf),
        new_TestFixture(test_rbuf_add__too_big_fragment),
        new_TestFixture(test_rbuf_add__overlap_lhs),
        new_TestFixture(test_rbuf_add__overlap_rhs),
        new_TestFixture(test_rbuf_get_by_dg),
        new_TestFixture(test_rbuf_exists),
        new_TestFixture(test_rbuf_rm_by_dg),
        new_TestFixture(test_rbuf_rm),
        new_TestFixture(test_rbuf_gc__manually),
        new_TestFixture(test_rbuf_gc__timed),
    };

    EMB_UNIT_TESTCALLER(sixlo_frag_tests, _set_up, NULL, fixtures);
    TESTS_START();
    TESTS_RUN((Test *)&sixlo_frag_tests);
    TESTS_END();
}

int main(void)
{
    /* netreg requires queue, but queue size one should be enough for us */
    msg_init_queue(&_msg_queue, 1U);
    run_unittests();
    return 0;
}
