/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#include "net/ieee802154.h"
#include "net/ipv6.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc.h"
#include "net/gnrc/sixlowpan.h"
#include "net/gnrc/sixlowpan/config.h"
#ifdef  MODULE_GNRC_SIXLOWPAN_FRAG_STATS
#include "net/gnrc/sixlowpan/frag/stats.h"
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_STATS */
#include "net/gnrc/sixlowpan/frag/minfwd.h"
#include "net/gnrc/sixlowpan/frag/vrb.h"
#include "net/sixlowpan.h"
#include "net/sixlowpan/sfr.h"
#include "thread.h"
#include "xtimer.h"
#include "utlist.h"

#include "net/gnrc/sixlowpan/frag/rb.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* estimated fragment payload size to determinate RBUF_INT_SIZE, default to
 * MAC payload size - fragment header. */
#ifndef GNRC_SIXLOWPAN_FRAG_SIZE
/* assuming 64-bit source/destination address, source PAN ID omitted */
#define GNRC_SIXLOWPAN_FRAG_SIZE (104 - 5)
#endif

#ifndef RBUF_INT_SIZE
/* same as ((int) ceil((double) N / D)) */
#define DIV_CEIL(N, D) (((N) + (D) - 1) / (D))
#if     IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_MINFWD)
#define RBUF_INT_SIZE (DIV_CEIL(IPV6_MIN_MTU, GNRC_SIXLOWPAN_FRAG_SIZE) * \
                       (CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE + \
                        CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_SIZE))
#else   /* IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_MINFWD) */
#define RBUF_INT_SIZE (DIV_CEIL(IPV6_MIN_MTU, GNRC_SIXLOWPAN_FRAG_SIZE) * \
                       CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE)
#endif  /* IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_MINFWD) */
#endif

static gnrc_sixlowpan_frag_rb_int_t rbuf_int[RBUF_INT_SIZE];

static gnrc_sixlowpan_frag_rb_t rbuf[CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE];

static char l2addr_str[3 * IEEE802154_LONG_ADDRESS_LEN];

static xtimer_t _gc_timer;
static msg_t _gc_timer_msg = { .type = GNRC_SIXLOWPAN_FRAG_RB_GC_MSG };

/* ------------------------------------
 * internal function definitions
 * ------------------------------------*/
/* checks whether start and end overlaps, but not identical to, given interval i */
static inline bool _rbuf_int_overlap_partially(gnrc_sixlowpan_frag_rb_int_t *i,
                                               uint16_t start, uint16_t end);
/* gets a free entry from interval buffer */
static gnrc_sixlowpan_frag_rb_int_t *_rbuf_int_get_free(void);
/* update interval buffer of entry */
static bool _rbuf_update_ints(gnrc_sixlowpan_frag_rb_base_t *entry,
                              uint16_t offset, size_t frag_size);
/* gets an entry identified by its tuple */
static int _rbuf_get(const void *src, size_t src_len,
                     const void *dst, size_t dst_len,
                     size_t size, uint16_t tag,
                     unsigned page);
/* gets an entry only by link-layer information and tag */
static gnrc_sixlowpan_frag_rb_t *_rbuf_get_by_tag(const gnrc_netif_hdr_t *netif_hdr,
                                                  uint16_t tag);
/* internal add to repeat add when fragments overlapped */
static int _rbuf_add(gnrc_netif_hdr_t *netif_hdr, gnrc_pktsnip_t *pkt,
                     size_t offset, unsigned page);

/* status codes for _rbuf_add() */
enum {
    RBUF_ADD_SUCCESS = 0,
    RBUF_ADD_ERROR = -1,
    RBUF_ADD_REPEAT = -2,
    RBUF_ADD_DUPLICATE = -3,
    RBUF_ADD_FORWARDED = -4,
};

static bool _check_hdr(gnrc_pktsnip_t *hdr, unsigned page);
static void _adapt_hdr(gnrc_pktsnip_t *hdr, unsigned page);
static int _forward_frag(gnrc_pktsnip_t *pkt, size_t frag_hdr_size,
                         gnrc_sixlowpan_frag_vrb_t *vrbe, unsigned page);
static int _forward_uncomp(gnrc_pktsnip_t *pkt,
                           gnrc_sixlowpan_frag_rb_t *rbuf,
                           gnrc_sixlowpan_frag_vrb_t *vrbe,
                           unsigned page);
static int _rbuf_resize_for_reassembly(gnrc_sixlowpan_frag_rb_t *rbuf);

static int _check_fragments(gnrc_sixlowpan_frag_rb_base_t *entry,
                            size_t frag_size, size_t offset)
{
    gnrc_sixlowpan_frag_rb_int_t *ptr = entry->ints;

    /* If the fragment overlaps another fragment and differs in either the size
     * or the offset of the overlapped fragment, discards the datagram
     * https://tools.ietf.org/html/rfc4944#section-5.3 */
    while (ptr != NULL) {
        if (_rbuf_int_overlap_partially(ptr, offset, offset + frag_size - 1)) {

            /* "A fresh reassembly may be commenced with the most recently
             * received link fragment"
             * https://tools.ietf.org/html/rfc4944#section-5.3 */
            return RBUF_ADD_REPEAT;
        }
        /* End was already checked in overlap check */
        if (ptr->start == offset) {
            DEBUG("6lo rbuf: fragment already in reassembly buffer\n");
            return RBUF_ADD_DUPLICATE;
        }
        ptr = ptr->next;
    }
    return RBUF_ADD_SUCCESS;
}

gnrc_sixlowpan_frag_rb_t *gnrc_sixlowpan_frag_rb_add(gnrc_netif_hdr_t *netif_hdr,
                                                     gnrc_pktsnip_t *pkt,
                                                     size_t offset, unsigned page)
{
    int res;
    if ((res = _rbuf_add(netif_hdr, pkt, offset, page)) == RBUF_ADD_REPEAT) {
        /* there was an overlap with existing fragments detected when trying to
         * add the new fragment.
         * https://tools.ietf.org/html/rfc4944#section-5.3 states "A fresh
         * reassembly may be commenced with the most recently received link
         * fragment.", so let's do that. Since the reassembly buffer entry was
         * deleted another overlap should not be detected (so _rbuf_add() won't
         * return RBUF_ADD_REPEAT again) */
        res = _rbuf_add(netif_hdr, pkt, offset, page);
    }
    return (res < 0) ? NULL : &rbuf[res];
}

gnrc_sixlowpan_frag_rb_t *gnrc_sixlowpan_frag_rb_get_by_datagram(
    const gnrc_netif_hdr_t *netif_hdr,
    uint16_t tag)
{
    return _rbuf_get_by_tag(netif_hdr, tag);
}

bool gnrc_sixlowpan_frag_rb_exists(const gnrc_netif_hdr_t *netif_hdr,
                                   uint16_t tag)
{
    return (_rbuf_get_by_tag(netif_hdr, tag) != NULL);
}

void gnrc_sixlowpan_frag_rb_rm_by_datagram(const gnrc_netif_hdr_t *netif_hdr,
                                           uint16_t tag)
{
    gnrc_sixlowpan_frag_rb_t *e = _rbuf_get_by_tag(netif_hdr, tag);

    if (e != NULL) {
        if (e->pkt != NULL) {
            gnrc_pktbuf_release(e->pkt);
        }
        gnrc_sixlowpan_frag_rb_remove(e);
    }
}

static gnrc_sixlowpan_frag_rb_t *_rbuf_get_by_tag(const gnrc_netif_hdr_t *netif_hdr,
                                                  uint16_t tag)
{
    assert(netif_hdr != NULL);
    const uint8_t *src = gnrc_netif_hdr_get_src_addr(netif_hdr);
    const uint8_t *dst = gnrc_netif_hdr_get_dst_addr(netif_hdr);
    const uint8_t src_len = netif_hdr->src_l2addr_len;
    const uint8_t dst_len = netif_hdr->dst_l2addr_len;

    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE; i++) {
        gnrc_sixlowpan_frag_rb_t *e = &rbuf[i];

        if ((e->pkt != NULL) && (e->super.tag == tag) &&
            (e->super.src_len == src_len) &&
            (e->super.dst_len == dst_len) &&
            (memcmp(e->super.src, src, src_len) == 0) &&
            (memcmp(e->super.dst, dst, dst_len) == 0)) {
            return e;
        }
    }
    return NULL;
}

#ifndef NDEBUG
static bool _valid_offset(gnrc_pktsnip_t *pkt, size_t offset)
{
    return (
        IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG) &&
        ((sixlowpan_frag_1_is(pkt->data) && (offset == 0)) ||
         (sixlowpan_frag_n_is(pkt->data) &&
          (offset == sixlowpan_frag_offset(pkt->data))))
    ) || (
        IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) &&
        /* offset == 0 is an abort condition that should not be handed to the
         * reassembly buffer */
        sixlowpan_sfr_rfrag_is(pkt->data) &&
        (sixlowpan_sfr_rfrag_get_offset(pkt->data) != 0)
    );
}
#endif

static uint8_t *_6lo_frag_payload(gnrc_pktsnip_t *pkt)
{
    if (sixlowpan_frag_1_is(pkt->data)) {
        return ((uint8_t *)pkt->data) + sizeof(sixlowpan_frag_t);
    }
    else {
        return ((uint8_t *)pkt->data) + sizeof(sixlowpan_frag_n_t);
    }
}

static size_t _6lo_frag_size(gnrc_pktsnip_t *pkt, size_t offset, uint8_t *data)
{
    size_t frag_size;

    if (offset == 0) {
        if (pkt->size < sizeof(sixlowpan_frag_t)) {
            return 0;
        }
        frag_size = pkt->size - sizeof(sixlowpan_frag_t);
        if (data[0] == SIXLOWPAN_UNCOMP) {
            /* subtract SIXLOWPAN_UNCOMP byte from fragment size,
             * data pointer must be changed by caller (see _rbuf_add()) */
            frag_size--;
        }
    }
    else {
        if (pkt->size < sizeof(sixlowpan_frag_n_t)) {
            return 0;
        }
        frag_size = pkt->size - sizeof(sixlowpan_frag_n_t);
    }
    return frag_size;
}

static uint16_t _6lo_sfr_datagram_size(gnrc_pktsnip_t *pkt, size_t offset)
{
    /* offset doubles as datagram size in RFRAG header when sequence number is 0
     * see https://tools.ietf.org/html/rfc8931#section-5.1 */
    return (offset == 0) ? sixlowpan_sfr_rfrag_get_offset(pkt->data) : 0;
}

static uint8_t *_6lo_sfr_payload(gnrc_pktsnip_t *pkt)
{
    return ((uint8_t *)pkt->data) + sizeof(sixlowpan_sfr_rfrag_t);
}

static size_t _6lo_sfr_frag_size(gnrc_pktsnip_t *pkt)
{
    /* TODO: if necessary check MAC layer here,
     * see https://tools.ietf.org/html/rfc8931#section-5.1 */
    return sixlowpan_sfr_rfrag_get_frag_size(pkt->data);
}

static gnrc_pktsnip_t *_mark_frag_hdr(gnrc_pktsnip_t *pkt)
{
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_IPHC)) {
        if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) &&
            sixlowpan_sfr_rfrag_is(pkt->data)) {
            return gnrc_pktbuf_mark(pkt, sizeof(sixlowpan_sfr_rfrag_t),
                                    GNRC_NETTYPE_SIXLOWPAN);
        }
        else if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG) &&
                 sixlowpan_frag_is(pkt->data)) {
            return gnrc_pktbuf_mark(pkt, sizeof(sixlowpan_frag_t),
                                    GNRC_NETTYPE_SIXLOWPAN);
        }
    }
    return NULL;
}

static int _rbuf_add(gnrc_netif_hdr_t *netif_hdr, gnrc_pktsnip_t *pkt,
                     size_t offset, unsigned page)
{
    union {
        gnrc_sixlowpan_frag_rb_base_t *super;
        gnrc_sixlowpan_frag_rb_t *rbuf;
        gnrc_sixlowpan_frag_vrb_t *vrb;
    } entry;
    const uint8_t *src = gnrc_netif_hdr_get_src_addr(netif_hdr);
    const uint8_t *dst = gnrc_netif_hdr_get_dst_addr(netif_hdr);
    uint8_t *data = NULL;
    size_t frag_size = 0;   /* assign 0, otherwise cppcheck complains ;-) */
    int res;
    uint16_t datagram_size;
    uint16_t datagram_tag;

    /* check if provided offset is the same as in fragment */
    assert(_valid_offset(pkt, offset));
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG) && sixlowpan_frag_is(pkt->data)) {
        data = _6lo_frag_payload(pkt);
        frag_size = _6lo_frag_size(pkt, offset, data);
        if (frag_size == 0) {
            DEBUG("6lo rbuf: integer underflow detected.\n");
            gnrc_pktbuf_release(pkt);
            return RBUF_ADD_ERROR;
        }
        datagram_size = sixlowpan_frag_datagram_size(pkt->data);
        datagram_tag = sixlowpan_frag_datagram_tag(pkt->data);
    }
    else if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) &&
             sixlowpan_sfr_rfrag_is(pkt->data)) {
        sixlowpan_sfr_rfrag_t *rfrag = pkt->data;

        data = _6lo_sfr_payload(pkt);
        frag_size = _6lo_sfr_frag_size(pkt);
        /* offset doubles as datagram size in RFRAG header when sequence number
         * is 0 */
        datagram_size = _6lo_sfr_datagram_size(pkt, offset);
        datagram_tag = rfrag->base.tag;
    }
    else {
        /* either one of the if branches above was taken */
        assert(data != NULL);
        gnrc_pktbuf_release(pkt);
        return RBUF_ADD_ERROR;
    }

    gnrc_sixlowpan_frag_rb_gc();
    /* only check VRB for subsequent frags, first frags create and not get VRB
     * entries below */
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_MINFWD) &&
        (offset > 0) &&
        sixlowpan_frag_n_is(pkt->data) &&
        (entry.vrb = gnrc_sixlowpan_frag_vrb_get(src, netif_hdr->src_l2addr_len,
                                                 datagram_tag)) != NULL) {
        DEBUG("6lo rbuf minfwd: VRB entry found, trying to forward\n");
        switch (_check_fragments(entry.super, frag_size, offset)) {
            case RBUF_ADD_REPEAT:
                DEBUG("6lo rbuf minfwd: overlap found; dropping VRB\n");
                gnrc_sixlowpan_frag_vrb_rm(entry.vrb);
                /* we don't repeat for VRB */
                gnrc_pktbuf_release(pkt);
                return RBUF_ADD_ERROR;
            case RBUF_ADD_DUPLICATE:
                DEBUG("6lo rbuf minfwd: not forwarding duplicate\n");
                gnrc_pktbuf_release(pkt);
                return RBUF_ADD_FORWARDED;
            default:
                break;
        }
        res = RBUF_ADD_ERROR;
        if (_rbuf_update_ints(entry.super, offset, frag_size)) {
            DEBUG("6lo rbuf minfwd: trying to forward fragment\n");
            entry.super->current_size += (uint16_t)frag_size;
            if (_forward_frag(pkt, sizeof(sixlowpan_frag_n_t), entry.vrb,
                              page) < 0) {
                DEBUG("6lo rbuf minfwd: unable to forward fragment\n");
                return RBUF_ADD_ERROR;
            }
            res = RBUF_ADD_FORWARDED;
        }
        return res;
    }
    else if ((res = _rbuf_get(src, netif_hdr->src_l2addr_len,
                              dst, netif_hdr->dst_l2addr_len,
                              datagram_size, datagram_tag, page)) < 0) {
        DEBUG("6lo rbuf: reassembly buffer full.\n");
        gnrc_pktbuf_release(pkt);
        return RBUF_ADD_ERROR;
    }
    entry.rbuf = &rbuf[res];
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR)
    offset += entry.rbuf->offset_diff;
#endif  /* IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) */
    if ((offset + frag_size) > entry.super->datagram_size) {
        DEBUG("6lo rfrag: fragment too big for resulting datagram, discarding datagram\n");
        gnrc_pktbuf_release(entry.rbuf->pkt);
        gnrc_pktbuf_release(pkt);
        gnrc_sixlowpan_frag_rb_remove(entry.rbuf);
        return RBUF_ADD_ERROR;
    }

    switch (_check_fragments(entry.super, frag_size, offset)) {
        case RBUF_ADD_REPEAT:
            DEBUG("6lo rfrag: overlapping intervals, discarding datagram\n");
            gnrc_pktbuf_release(entry.rbuf->pkt);
            gnrc_sixlowpan_frag_rb_remove(entry.rbuf);
            return RBUF_ADD_REPEAT;
        case RBUF_ADD_DUPLICATE:
            gnrc_pktbuf_release(pkt);
            return res;
        default:
            break;
    }

    if (_rbuf_update_ints(entry.super, offset, frag_size)) {
        DEBUG("6lo rbuf: add fragment data\n");
        entry.super->current_size += (uint16_t)frag_size;
        if (offset == 0) {
            if (IS_USED(MODULE_GNRC_SIXLOWPAN_IPHC) &&
                sixlowpan_iphc_is(data)) {
                DEBUG("6lo rbuf: detected IPHC header.\n");
                gnrc_pktsnip_t *frag_hdr = _mark_frag_hdr(pkt);

                if (frag_hdr == NULL) {
                    DEBUG("6lo rbuf: unable to mark fragment header. "
                          "aborting reassembly.\n");
                    gnrc_pktbuf_release(entry.rbuf->pkt);
                    gnrc_pktbuf_release(pkt);
                    gnrc_sixlowpan_frag_rb_remove(entry.rbuf);
                    return RBUF_ADD_ERROR;
                }
                else {
                    DEBUG("6lo rbuf: handing over to IPHC reception.\n");
                    /* `pkt` released in IPHC */
                    gnrc_sixlowpan_iphc_recv(pkt, entry.rbuf, 0);
                    /* check if entry was deleted in IPHC (error case) */
                    if (gnrc_sixlowpan_frag_rb_entry_empty(entry.rbuf)) {
                        res = RBUF_ADD_ERROR;
                    }
                    return res;
                }
            }
            else if (data[0] == SIXLOWPAN_UNCOMP) {
                DEBUG("6lo rbuf: detected uncompressed datagram\n");
                data++;
                if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_MINFWD) &&
                    /* only try minimal forwarding when fragment is the only
                     * fragment in reassembly buffer yet */
                    sixlowpan_frag_1_is(pkt->data) &&
                    (entry.super->current_size == frag_size)) {
                    gnrc_sixlowpan_frag_vrb_t *vrbe;
                    gnrc_pktsnip_t tmp = {
                        .data = data,
                        .size = frag_size,
                        .users = 1,
                    };

                    if (_check_hdr(&tmp, page) &&
                        (vrbe = gnrc_sixlowpan_frag_vrb_from_route(
                                    entry.super,
                                    gnrc_netif_hdr_get_netif(netif_hdr),
                                    &tmp))) {
                        _adapt_hdr(&tmp, page);
                        return _forward_uncomp(pkt, rbuf, vrbe, page);
                    }
                }
                else if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) &&
                         sixlowpan_sfr_rfrag_is(pkt->data)) {
                    entry.super->datagram_size--;
                    /* Check, if fragment is still small enough to fit datagram size.
                     * `offset` is 0, as this is the first fragment so it does not have to be added
                     * here. */
                    if (frag_size > entry.super->datagram_size) {
                        DEBUG_PUTS(
                           "6lo rfrag: fragment too big for resulting datagram, "
                           "discarding datagram\n"
                        );
                        gnrc_pktbuf_release(entry.rbuf->pkt);
                        gnrc_pktbuf_release(pkt);
                        gnrc_sixlowpan_frag_rb_remove(entry.rbuf);
                        return RBUF_ADD_ERROR;
                    }
                }
            }
        }
        if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_MINFWD) ||
            IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR)) {
            /* all cases to try forwarding with minfwd or SFR above failed so
             * just do normal reassembly. For the `minfwd` case however, we need
             * to resize `entry.rbuf->pkt`, since we kept the packet allocation
             * with fragment forwarding as minimal as possible in
             * `_rbuf_get()` */
            res = _rbuf_resize_for_reassembly(entry.rbuf);
            if (res == RBUF_ADD_ERROR) {
                gnrc_pktbuf_release(pkt);
                return res;
            }
        }
        memcpy(((uint8_t *)entry.rbuf->pkt->data) + offset, data,
               frag_size);
    }
    else {
        /* no space left in rbuf interval buffer*/
        gnrc_pktbuf_release(entry.rbuf->pkt);
        gnrc_sixlowpan_frag_rb_remove(entry.rbuf);
        res = RBUF_ADD_ERROR;
    }
    /* no errors and not consumed => release packet */
    gnrc_pktbuf_release(pkt);
    return res;
}

static inline bool _rbuf_int_overlap_partially(gnrc_sixlowpan_frag_rb_int_t *i,
                                               uint16_t start, uint16_t end)
{
    /* start and ends are both inclusive, so using <= for both */
    return ((i->start <= end) && (start <= i->end)) && /* overlaps */
        ((start != i->start) || (end != i->end)); /* not identical */
}

static gnrc_sixlowpan_frag_rb_int_t *_rbuf_int_get_free(void)
{
    for (unsigned int i = 0; i < RBUF_INT_SIZE; i++) {
        if (rbuf_int[i].end == 0) { /* start must be smaller than end anyways*/
            return rbuf_int + i;
        }
    }

    return NULL;
}

#ifdef TEST_SUITES
bool gnrc_sixlowpan_frag_rb_ints_empty(void)
{
    for (unsigned int i = 0; i < RBUF_INT_SIZE; i++) {
        if (rbuf_int[i].end > 0) {
            return false;
        }
    }
    return true;
}
#endif  /* TEST_SUITES */

static bool _rbuf_update_ints(gnrc_sixlowpan_frag_rb_base_t *entry,
                              uint16_t offset, size_t frag_size)
{
    gnrc_sixlowpan_frag_rb_int_t *new;
    uint16_t end = (uint16_t)(offset + frag_size - 1);

    new = _rbuf_int_get_free();

    if (new == NULL) {
        DEBUG("6lo rfrag: no space left in rbuf interval buffer.\n");
        return false;
    }

    new->start = offset;
    new->end = end;

    DEBUG("6lo rfrag: add interval (%" PRIu16 ", %" PRIu16 ") to entry (%s, ",
          new->start, new->end, gnrc_netif_addr_to_str(entry->src,
                                                       entry->src_len,
                                                       l2addr_str));
    DEBUG("%s, %u, %u)\n", gnrc_netif_addr_to_str(entry->dst,
                                                  entry->dst_len,
                                                  l2addr_str),
          entry->datagram_size, entry->tag);

    LL_PREPEND(entry->ints, new);

    return true;
}

static void _gc_pkt(gnrc_sixlowpan_frag_rb_t *rbuf)
{
#if CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER > 0
    if (rbuf->super.current_size == 0) {
        /* packet is scheduled for deletion, but was complete, i.e. pkt is
         * already handed up to other layer, i.e. no need to release */
        return;
    }
#endif
    gnrc_pktbuf_release(rbuf->pkt);
}

void gnrc_sixlowpan_frag_rb_gc(void)
{
    uint32_t now_usec = xtimer_now_usec();
    unsigned int i;

    for (i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE; i++) {
        /* since pkt occupies pktbuf, aggressivly collect garbage */
        if (!gnrc_sixlowpan_frag_rb_entry_empty(&rbuf[i]) &&
              ((now_usec - rbuf[i].super.arrival) >
               CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US)) {
            DEBUG("6lo rfrag: entry (%s, ",
                  gnrc_netif_addr_to_str(rbuf[i].super.src,
                                         rbuf[i].super.src_len,
                                         l2addr_str));
            DEBUG("%s, %u, %u) timed out\n",
                  gnrc_netif_addr_to_str(rbuf[i].super.dst,
                                         rbuf[i].super.dst_len,
                                         l2addr_str),
                  (unsigned)rbuf[i].super.datagram_size, rbuf[i].super.tag);

            _gc_pkt(&rbuf[i]);
            gnrc_sixlowpan_frag_rb_remove(&(rbuf[i]));
        }
    }
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_VRB
    gnrc_sixlowpan_frag_vrb_gc();
#endif
}

static inline void _set_rbuf_timeout(void)
{
    xtimer_set_msg(&_gc_timer, CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US,
                   &_gc_timer_msg, thread_getpid());
}

static int _rbuf_get(const void *src, size_t src_len,
                     const void *dst, size_t dst_len,
                     size_t size, uint16_t tag,
                     unsigned page)
{
    gnrc_sixlowpan_frag_rb_t *res = NULL, *oldest = NULL;
    uint32_t now_usec = xtimer_now_usec();

    for (unsigned int i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE; i++) {
        /* check first if entry already available */
        if ((rbuf[i].pkt != NULL) && (rbuf[i].super.tag == tag) &&
            ((IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) &&
              /* not all SFR fragments carry the datagram size, so make 0 a
               * legal value to not compare datagram size */
              ((size == 0) || (rbuf[i].super.datagram_size == size))) ||
             (!IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) &&
              (rbuf[i].super.datagram_size == size))) &&
            (rbuf[i].super.src_len == src_len) &&
            (rbuf[i].super.dst_len == dst_len) &&
            (memcmp(rbuf[i].super.src, src, src_len) == 0) &&
            (memcmp(rbuf[i].super.dst, dst, dst_len) == 0)) {
            DEBUG("6lo rfrag: entry %p (%s, ", (void *)(&rbuf[i]),
                  gnrc_netif_addr_to_str(rbuf[i].super.src,
                                         rbuf[i].super.src_len,
                                         l2addr_str));
            DEBUG("%s, %u, %u) found\n",
                  gnrc_netif_addr_to_str(rbuf[i].super.dst,
                                         rbuf[i].super.dst_len,
                                         l2addr_str),
                  (unsigned)rbuf[i].super.datagram_size, rbuf[i].super.tag);
#if CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER > 0
            if (rbuf[i].super.current_size == 0) {
                /* ensure that only empty reassembly buffer entries and entries
                 * scheduled for deletion have `current_size == 0` */
                DEBUG("6lo rfrag: scheduled for deletion, don't add fragment\n");
                return -1;
            }
#endif
            rbuf[i].super.arrival = now_usec;
            _set_rbuf_timeout();
            return i;
        }

        /* if there is a free spot: remember it */
        if ((res == NULL) && gnrc_sixlowpan_frag_rb_entry_empty(&rbuf[i])) {
            res = &(rbuf[i]);
        }

        /* remember oldest slot */
        /* note that xtimer_now will overflow in ~1.2 hours */
        if ((oldest == NULL) ||
            (oldest->super.arrival - rbuf[i].super.arrival < UINT32_MAX / 2)) {
            oldest = &(rbuf[i]);
        }
    }

    /* entry not in buffer and no empty spot found */
    if (res == NULL) {
        assert(oldest != NULL);
        /* if oldest is not empty, res must not be NULL (because otherwise
         * oldest could have been picked as res) */
        assert(!gnrc_sixlowpan_frag_rb_entry_empty(oldest));
        if (!IS_ACTIVE(CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DO_NOT_OVERRIDE) ||
            ((now_usec - oldest->super.arrival) >
            CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US)) {
            DEBUG("6lo rfrag: reassembly buffer full, remove oldest entry\n");
            gnrc_pktbuf_release(oldest->pkt);
            gnrc_sixlowpan_frag_rb_remove(oldest);
            res = oldest;
#if !IS_ACTIVE(CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DO_NOT_OVERRIDE) && \
    IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_STATS)
            gnrc_sixlowpan_frag_stats_get()->rbuf_full++;
#endif
        }
        else {
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_STATS
            gnrc_sixlowpan_frag_stats_get()->rbuf_full++;
#endif
            return -1;
        }
    }

    /* now we have an empty spot */

    gnrc_nettype_t reass_type;
    switch (page) {
        /* use switch(page) to be extendable */
#ifdef MODULE_GNRC_IPV6
        case 0U:
            reass_type = GNRC_NETTYPE_IPV6;
            break;
#endif
        default:
            reass_type = GNRC_NETTYPE_UNDEF;
    }
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_VRB)) {
        if (IS_USED(MODULE_GNRC_SIXLOWPAN_IPHC)) {
            /* only allocate enough space to decompress IPv6 header
             * for forwarding information */
            res->pkt = gnrc_pktbuf_add(NULL, NULL, sizeof(ipv6_hdr_t),
                                       reass_type);
        }
        else {
            /* try fragment forwarding without IPHC. Since `res->pkt == NULL`
             * is not a valid value for a reassembly buffer entry, we need to
             * set it to at least a packet snip for now */
            res->pkt = gnrc_pktbuf_add(NULL, NULL, 0, reass_type);
        }
    }
    else {
        /* reassemble whole datagram without direct fragment forwarding */
        res->pkt = gnrc_pktbuf_add(NULL, NULL, size, reass_type);
    }
    if (res->pkt == NULL) {
        DEBUG("6lo rfrag: can not allocate reassembly buffer space.\n");
        return -1;
    }

    if (res->pkt->data) {
        /* clean first few bytes for later look-ups */
        memset(res->pkt->data, 0, sizeof(uint64_t));
    }
    res->super.datagram_size = size;
    res->super.arrival = now_usec;
    memcpy(res->super.src, src, src_len);
    memcpy(res->super.dst, dst, dst_len);
    res->super.src_len = src_len;
    res->super.dst_len = dst_len;
    res->super.tag = tag;
    res->super.current_size = 0;
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR)
    res->offset_diff = 0U;
    memset(res->received, 0U, sizeof(res->received));
#endif  /* IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) */

    DEBUG("6lo rfrag: entry %p (%s, ", (void *)res,
          gnrc_netif_addr_to_str(res->super.src, res->super.src_len,
                                 l2addr_str));
    DEBUG("%s, %u, %u) created\n",
          gnrc_netif_addr_to_str(res->super.dst, res->super.dst_len,
                                 l2addr_str), res->super.datagram_size,
          res->super.tag);

    _set_rbuf_timeout();

    return res - &(rbuf[0]);
}

#ifdef TEST_SUITES
void gnrc_sixlowpan_frag_rb_reset(void)
{
    xtimer_remove(&_gc_timer);
    memset(rbuf_int, 0, sizeof(rbuf_int));
    for (unsigned int i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_SIZE; i++) {
        if ((rbuf[i].pkt != NULL) &&
            (rbuf[i].pkt->users > 0)) {
            _gc_pkt(&rbuf[i]);
        }
    }
    memset(rbuf, 0, sizeof(rbuf));
}

const gnrc_sixlowpan_frag_rb_t *gnrc_sixlowpan_frag_rb_array(void)
{
    return &rbuf[0];
}
#endif

void gnrc_sixlowpan_frag_rb_base_rm(gnrc_sixlowpan_frag_rb_base_t *entry)
{
    while (entry->ints != NULL) {
        gnrc_sixlowpan_frag_rb_int_t *next = entry->ints->next;

        entry->ints->start = 0;
        entry->ints->end = 0;
        entry->ints->next = NULL;
        entry->ints = next;
    }
    entry->datagram_size = 0;
}

static void _tmp_rm(gnrc_sixlowpan_frag_rb_t *rbuf)
{
#if CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER > 0U
        /* use garbage-collection to leave the entry for at least
         * CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER in the reassembly buffer by
         * setting the arrival time to
         * (CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US - CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER)
         * microseconds in the past */
        rbuf->super.arrival = xtimer_now_usec() -
                              (CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US -
                               CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER);
        /* reset current size to prevent late duplicates to trigger another
         * dispatch */
        rbuf->super.current_size = 0;
#else   /* CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER == 0U */
        gnrc_sixlowpan_frag_rb_remove(rbuf);
#endif  /* CONFIG_GNRC_SIXLOWPAN_FRAG_RBUF_DEL_TIMER */
}

#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_STATS)
static inline unsigned _count_frags(gnrc_sixlowpan_frag_rb_t *rbuf)
{
    unsigned frags = 0;
    gnrc_sixlowpan_frag_rb_int_t *frag = rbuf->super.ints;

    while (frag) {
        frag = frag->next;
        frags++;
    }
    return frags;
}
#endif

int gnrc_sixlowpan_frag_rb_dispatch_when_complete(gnrc_sixlowpan_frag_rb_t *rbuf,
                                                   gnrc_netif_hdr_t *netif_hdr)
{
    assert(rbuf);
    assert(netif_hdr);
    int res = (rbuf->super.current_size == rbuf->super.datagram_size);

    if (res) {
        gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(rbuf->super.src,
                                                     rbuf->super.src_len,
                                                     rbuf->super.dst,
                                                     rbuf->super.dst_len);

        if (netif == NULL) {
            DEBUG("6lo rbuf: error allocating netif header\n");
            gnrc_pktbuf_release(rbuf->pkt);
            _tmp_rm(rbuf);
            return -1;
        }

        /* copy the transmit information of the latest fragment into the newly
         * created header to have some link_layer information. The link_layer
         * info of the previous fragments is discarded.
         */
        gnrc_netif_hdr_t *new_netif_hdr = netif->data;
        new_netif_hdr->if_pid = netif_hdr->if_pid;
        new_netif_hdr->flags = netif_hdr->flags;
        new_netif_hdr->lqi = netif_hdr->lqi;
        new_netif_hdr->rssi = netif_hdr->rssi;
        rbuf->pkt = gnrc_pkt_append(rbuf->pkt, netif);
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_STATS)
        gnrc_sixlowpan_frag_stats_get()->fragments += _count_frags(rbuf);
        gnrc_sixlowpan_frag_stats_get()->datagrams++;
#endif
        gnrc_sixlowpan_dispatch_recv(rbuf->pkt, NULL, 0);
        _tmp_rm(rbuf);
    }
    return res;
}

static bool _check_hdr(gnrc_pktsnip_t *hdr, unsigned page)
{
    switch (page) {
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6)
        case 0: {
            ipv6_hdr_t *ipv6_hdr = hdr->data;

            if (ipv6_hdr->hl <= 1) {
                DEBUG("6lo rbuf minfwd: minimal hop-limit reached\n");
                return false;
            }
            hdr->type = GNRC_NETTYPE_IPV6;
            break;
        }
#endif
        default:
            hdr->type = GNRC_NETTYPE_UNDEF;
            break;
    }
    return true;
}

static void _adapt_hdr(gnrc_pktsnip_t *hdr, unsigned page)
{
    switch (page) {
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6)
        case 0: {
            ipv6_hdr_t *ipv6_hdr = hdr->data;

            ipv6_hdr->hl--;
            break;
        }
#endif
        default:
            (void)hdr;
            break;
    }
}

static int _forward_frag(gnrc_pktsnip_t *pkt, size_t frag_hdr_size,
                         gnrc_sixlowpan_frag_vrb_t *vrbe, unsigned page)
{
    int res = -ENOTSUP;

    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_MINFWD)) {
        gnrc_pktsnip_t *frag = gnrc_pktbuf_mark(pkt, frag_hdr_size,
                                                GNRC_NETTYPE_SIXLOWPAN);
        if (frag == NULL) {
            gnrc_pktbuf_release(pkt);
            res = -ENOMEM;
        }
        else {
            pkt = gnrc_pkt_delete(pkt, frag);
            frag->next = NULL;
            /* remove netif header */
            gnrc_pktbuf_remove_snip(pkt, pkt->next);
            res = gnrc_sixlowpan_frag_minfwd_forward(pkt, frag->data, vrbe,
                                                     page);
            gnrc_pktbuf_release(frag);
        }
    }
    return res;
}

static int _forward_uncomp(gnrc_pktsnip_t *pkt,
                           gnrc_sixlowpan_frag_rb_t *rbuf,
                           gnrc_sixlowpan_frag_vrb_t *vrbe,
                           unsigned page)
{
    DEBUG("6lo rbuf minfwd: found route, trying to forward\n");
    int res = _forward_frag(pkt, sizeof(sixlowpan_frag_t),
                            vrbe, page);

    /* prevent intervals from being deleted (they are in the
     * VRB now) */
    rbuf->super.ints = NULL;
    gnrc_pktbuf_release(rbuf->pkt);
    gnrc_sixlowpan_frag_rb_remove(rbuf);
    return (res == 0) ? RBUF_ADD_SUCCESS : RBUF_ADD_ERROR;
}

static int _rbuf_resize_for_reassembly(gnrc_sixlowpan_frag_rb_t *rbuf)
{
    DEBUG("6lo rbuf: just do normal reassembly\n");
    if (gnrc_pktbuf_realloc_data(rbuf->pkt,
                                 rbuf->super.datagram_size) != 0) {
        DEBUG("6lo rbuf minfwd: can't allocate packet data\n");
        gnrc_pktbuf_release(rbuf->pkt);
        gnrc_sixlowpan_frag_rb_remove(rbuf);
        return RBUF_ADD_ERROR;
    }
    return RBUF_ADD_SUCCESS;
}

/** @} */
