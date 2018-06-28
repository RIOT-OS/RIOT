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

#include <inttypes.h>
#include <stdbool.h>

#include "rbuf.h"
#include "net/ipv6.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc.h"
#include "net/gnrc/sixlowpan.h"
#include "net/gnrc/sixlowpan/frag.h"
#include "net/sixlowpan.h"
#include "thread.h"
#include "xtimer.h"
#include "utlist.h"

#define ENABLE_DEBUG    (0)
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
#define RBUF_INT_SIZE (DIV_CEIL(IPV6_MIN_MTU, GNRC_SIXLOWPAN_FRAG_SIZE) * RBUF_SIZE)
#endif

static rbuf_int_t rbuf_int[RBUF_INT_SIZE];

static rbuf_t rbuf[RBUF_SIZE];

static char l2addr_str[3 * IEEE802154_LONG_ADDRESS_LEN];

static xtimer_t _gc_timer;
static msg_t _gc_timer_msg = { .type = GNRC_SIXLOWPAN_MSG_FRAG_GC_RBUF };

/* ------------------------------------
 * internal function definitions
 * ------------------------------------*/
/* checks whether start and end overlaps, but not identical to, given interval i */
static inline bool _rbuf_int_overlap_partially(rbuf_int_t *i, uint16_t start, uint16_t end);
/* gets a free entry from interval buffer */
static rbuf_int_t *_rbuf_int_get_free(void);
/* update interval buffer of entry */
static bool _rbuf_update_ints(rbuf_t *entry, uint16_t offset, size_t frag_size);
/* gets an entry identified by its tupel */
static rbuf_t *_rbuf_get(const void *src, size_t src_len,
                         const void *dst, size_t dst_len,
                         size_t size, uint16_t tag, unsigned page);

void rbuf_add(gnrc_netif_hdr_t *netif_hdr, gnrc_pktsnip_t *pkt,
              size_t offset, unsigned page)
{
    rbuf_t *entry;
    sixlowpan_frag_t *frag = pkt->data;
    rbuf_int_t *ptr;
    uint8_t *data = ((uint8_t *)pkt->data) + sizeof(sixlowpan_frag_t);
    size_t frag_size;

    rbuf_gc();
    entry = _rbuf_get(gnrc_netif_hdr_get_src_addr(netif_hdr), netif_hdr->src_l2addr_len,
                      gnrc_netif_hdr_get_dst_addr(netif_hdr), netif_hdr->dst_l2addr_len,
                      byteorder_ntohs(frag->disp_size) & SIXLOWPAN_FRAG_SIZE_MASK,
                      byteorder_ntohs(frag->tag), page);

    if (entry == NULL) {
        DEBUG("6lo rbuf: reassembly buffer full.\n");
        return;
    }

    ptr = entry->ints;

    /* dispatches in the first fragment are ignored */
    if (offset == 0) {
        frag_size = pkt->size - sizeof(sixlowpan_frag_t);
        if (data[0] == SIXLOWPAN_UNCOMP) {
            frag_size--;
        }
    }
    else {
        frag_size = pkt->size - sizeof(sixlowpan_frag_n_t);
        data++; /* FRAGN header is one byte longer (offset) */
    }

    if ((offset + frag_size) > entry->super.pkt->size) {
        DEBUG("6lo rfrag: fragment too big for resulting datagram, discarding datagram\n");
        gnrc_pktbuf_release(entry->super.pkt);
        rbuf_rm(entry);
        return;
    }

    /* If the fragment overlaps another fragment and differs in either the size
     * or the offset of the overlapped fragment, discards the datagram
     * https://tools.ietf.org/html/rfc4944#section-5.3 */
    while (ptr != NULL) {
        if (_rbuf_int_overlap_partially(ptr, offset, offset + frag_size - 1)) {
            DEBUG("6lo rfrag: overlapping intervals, discarding datagram\n");
            gnrc_pktbuf_release(entry->super.pkt);
            rbuf_rm(entry);

            /* "A fresh reassembly may be commenced with the most recently
             * received link fragment"
             * https://tools.ietf.org/html/rfc4944#section-5.3 */
            rbuf_add(netif_hdr, pkt, offset, page);

            return;
        }

        ptr = ptr->next;
    }

    if (_rbuf_update_ints(entry, offset, frag_size)) {
        DEBUG("6lo rbuf: add fragment data\n");
        entry->super.current_size += (uint16_t)frag_size;
        if (offset == 0) {
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
            if (sixlowpan_iphc_is(data)) {
                gnrc_pktsnip_t *frag_hdr = gnrc_pktbuf_mark(pkt,
                        sizeof(sixlowpan_frag_t), GNRC_NETTYPE_SIXLOWPAN);
                if (frag_hdr == NULL) {
                    gnrc_pktbuf_release(entry->super.pkt);
                    rbuf_rm(entry);
                    return;
                }
                gnrc_sixlowpan_iphc_recv(pkt, &entry->super, 0);
                return;
            }
            else
#endif
            if (data[0] == SIXLOWPAN_UNCOMP) {
                data++;
            }
        }
        memcpy(((uint8_t *)entry->super.pkt->data) + offset, data,
               frag_size);
    }
    gnrc_sixlowpan_frag_rbuf_dispatch_when_complete(&entry->super, netif_hdr);
    gnrc_pktbuf_release(pkt);
}

static inline bool _rbuf_int_overlap_partially(rbuf_int_t *i, uint16_t start, uint16_t end)
{
    /* start and ends are both inclusive, so using <= for both */
    return ((i->start <= end) && (start <= i->end)) && /* overlaps */
        ((start != i->start) || (end != i->end)); /* not identical */
}

static rbuf_int_t *_rbuf_int_get_free(void)
{
    for (unsigned int i = 0; i < RBUF_INT_SIZE; i++) {
        if (rbuf_int[i].end == 0) { /* start must be smaller than end anyways*/
            return rbuf_int + i;
        }
    }

    return NULL;
}

void rbuf_rm(rbuf_t *entry)
{
    while (entry->ints != NULL) {
        rbuf_int_t *next = entry->ints->next;

        entry->ints->start = 0;
        entry->ints->end = 0;
        entry->ints->next = NULL;
        entry->ints = next;
    }

    entry->super.pkt = NULL;
}

static bool _rbuf_update_ints(rbuf_t *entry, uint16_t offset, size_t frag_size)
{
    rbuf_int_t *new;
    uint16_t end = (uint16_t)(offset + frag_size - 1);

    new = _rbuf_int_get_free();

    if (new == NULL) {
        DEBUG("6lo rfrag: no space left in rbuf interval buffer.\n");
        return false;
    }

    new->start = offset;
    new->end = end;

    DEBUG("6lo rfrag: add interval (%" PRIu16 ", %" PRIu16 ") to entry (%s, ",
          new->start, new->end, gnrc_netif_addr_to_str(entry->super.src,
                                                       entry->super.src_len,
                                                       l2addr_str));
    DEBUG("%s, %u, %u)\n", gnrc_netif_addr_to_str(entry->super.dst,
                                                  entry->super.dst_len,
                                                  l2addr_str),
          (unsigned)entry->super.pkt->size, entry->super.tag);

    LL_PREPEND(entry->ints, new);

    return true;
}

void rbuf_gc(void)
{
    uint32_t now_usec = xtimer_now_usec();
    unsigned int i;

    for (i = 0; i < RBUF_SIZE; i++) {
        /* since pkt occupies pktbuf, aggressivly collect garbage */
        if ((rbuf[i].super.pkt != NULL) &&
              ((now_usec - rbuf[i].arrival) > RBUF_TIMEOUT)) {
            DEBUG("6lo rfrag: entry (%s, ",
                  gnrc_netif_addr_to_str(rbuf[i].super.src,
                                         rbuf[i].super.src_len,
                                         l2addr_str));
            DEBUG("%s, %u, %u) timed out\n",
                  gnrc_netif_addr_to_str(rbuf[i].super.dst,
                                         rbuf[i].super.dst_len,
                                         l2addr_str),
                  (unsigned)rbuf[i].super.pkt->size, rbuf[i].super.tag);

            gnrc_pktbuf_release(rbuf[i].super.pkt);
            rbuf_rm(&(rbuf[i]));
        }
    }
}

static inline void _set_rbuf_timeout(void)
{
    xtimer_set_msg(&_gc_timer, RBUF_TIMEOUT, &_gc_timer_msg, sched_active_pid);
}

static rbuf_t *_rbuf_get(const void *src, size_t src_len,
                         const void *dst, size_t dst_len,
                         size_t size, uint16_t tag, unsigned page)
{
    rbuf_t *res = NULL, *oldest = NULL;
    uint32_t now_usec = xtimer_now_usec();

    for (unsigned int i = 0; i < RBUF_SIZE; i++) {
        /* check first if entry already available */
        if ((rbuf[i].super.pkt != NULL) && (rbuf[i].super.pkt->size == size) &&
            (rbuf[i].super.tag == tag) && (rbuf[i].super.src_len == src_len) &&
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
                  (unsigned)rbuf[i].super.pkt->size, rbuf[i].super.tag);
            rbuf[i].arrival = now_usec;
            _set_rbuf_timeout();
            return &(rbuf[i]);
        }

        /* if there is a free spot: remember it */
        if ((res == NULL) && (rbuf[i].super.pkt == NULL)) {
            res = &(rbuf[i]);
        }

        /* remember oldest slot */
        /* note that xtimer_now will overflow in ~1.2 hours */
        if ((oldest == NULL) || (oldest->arrival - rbuf[i].arrival < UINT32_MAX / 2)) {
            oldest = &(rbuf[i]);
        }
    }

    /* entry not in buffer and no empty spot found */
    if (res == NULL) {
        assert(oldest != NULL);
        /* if oldest->pkt == NULL, res must not be NULL */
        assert(oldest->super.pkt != NULL);
        DEBUG("6lo rfrag: reassembly buffer full, remove oldest entry\n");
        gnrc_pktbuf_release(oldest->super.pkt);
        rbuf_rm(oldest);
        res = oldest;
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
    res->super.pkt = gnrc_pktbuf_add(NULL, NULL, size, reass_type);
    if (res->super.pkt == NULL) {
        DEBUG("6lo rfrag: can not allocate reassembly buffer space.\n");
        return NULL;
    }

    *((uint64_t *)res->super.pkt->data) = 0;  /* clean first few bytes for later
                                               * look-ups */
    res->arrival = now_usec;
    memcpy(res->super.src, src, src_len);
    memcpy(res->super.dst, dst, dst_len);
    res->super.src_len = src_len;
    res->super.dst_len = dst_len;
    res->super.tag = tag;
    res->super.current_size = 0;

    DEBUG("6lo rfrag: entry %p (%s, ", (void *)res,
          gnrc_netif_addr_to_str(res->super.src, res->super.src_len,
                                 l2addr_str));
    DEBUG("%s, %u, %u) created\n",
          gnrc_netif_addr_to_str(res->super.dst, res->super.dst_len,
                                 l2addr_str), (unsigned)res->super.pkt->size,
          res->super.tag);

    _set_rbuf_timeout();

    return res;
}

/** @} */
