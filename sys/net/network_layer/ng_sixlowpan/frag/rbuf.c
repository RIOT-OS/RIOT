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
#include "net/ng_netapi.h"
#include "net/ng_netif.h"
#include "net/ng_netif/hdr.h"
#include "net/ng_pktbuf.h"
#include "net/ng_ipv6/netif.h"
#include "net/ng_sixlowpan.h"
#include "net/ng_sixlowpan/frag.h"
#include "thread.h"
#include "timex.h"
#include "vtimer.h"
#include "utlist.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifndef RBUF_INT_SIZE
#define RBUF_INT_SIZE   (NG_IPV6_NETIF_DEFAULT_MTU * RBUF_SIZE / 127)
#endif

static rbuf_int_t rbuf_int[RBUF_INT_SIZE];

static rbuf_t rbuf[RBUF_SIZE];

#if ENABLE_DEBUG
static char l2addr_str[3 * RBUF_L2ADDR_MAX_LEN];
#endif

/* ------------------------------------
 * internal function definitions
 * ------------------------------------*/
/* checks whether start and end are in given interval i */
static inline bool _rbuf_int_in(rbuf_int_t *i, uint16_t start, uint16_t end);
/* gets a free entry from interval buffer */
static rbuf_int_t *_rbuf_int_get_free(void);
/* remove entry from reassembly buffer */
static void _rbuf_rem(rbuf_t *entry);
/* update interval buffer of entry */
static bool _rbuf_update_ints(rbuf_t *entry, uint16_t offset, size_t frag_size);
/* checks timeouts and removes entries if necessary (oldest if full) */
static void _rbuf_gc(void);
/* gets an entry identified by its tupel */
static rbuf_t *_rbuf_get(const void *src, size_t src_len,
                         const void *dst, size_t dst_len,
                         size_t size, uint16_t tag);

void rbuf_add(ng_netif_hdr_t *netif_hdr, ng_sixlowpan_frag_t *frag,
              size_t frag_size, size_t offset)
{
    rbuf_t *entry;
    rbuf_int_t *ptr;
    uint8_t *data = ((uint8_t *)frag) + sizeof(ng_sixlowpan_frag_t);
    uint16_t dg_frag_size = frag_size; /* may differ on first fragment */

    _rbuf_gc();
    entry = _rbuf_get(ng_netif_hdr_get_src_addr(netif_hdr), netif_hdr->src_l2addr_len,
                      ng_netif_hdr_get_dst_addr(netif_hdr), netif_hdr->dst_l2addr_len,
                      byteorder_ntohs(frag->disp_size) & NG_SIXLOWPAN_FRAG_SIZE_MASK,
                      byteorder_ntohs(frag->tag));

    if (entry == NULL) {
        DEBUG("6lo rbuf: reassembly buffer full.\n");
        return;
    }

    ptr = entry->ints;

    /* dispatches in the first fragment are ignored */
    if (offset != 0) {
        switch (((uint8_t *)(entry->pkt->data))[0]) {
            case NG_SIXLOWPAN_UNCOMPRESSED:
                offset++;

                break;

            default:
                break;
        }

        data++;     /* also don't take offset field */
    }
    else {
        switch (data[0]) {
            case NG_SIXLOWPAN_UNCOMPRESSED:
                dg_frag_size--;

                break;

            default:
                break;
        }

    }

    if ((offset + frag_size) > entry->pkt->size) {
        DEBUG("6lo rfrag: fragment too big for resulting datagram, discarding datagram\n");
        ng_pktbuf_release(entry->pkt);
        _rbuf_rem(entry);
        return;
    }

    while (ptr != NULL) {
        if (_rbuf_int_in(ptr, offset, offset + dg_frag_size - 1)) {
            DEBUG("6lo rfrag: overlapping or same intervals, discarding datagram\n");
            ng_pktbuf_release(entry->pkt);
            _rbuf_rem(entry);
            return;
        }

        ptr = ptr->next;
    }

    if (_rbuf_update_ints(entry, offset, dg_frag_size)) {
        if (dg_frag_size < frag_size) {
            /* some dispatches do not count to datagram size and we need
             * more space because of that */
            if (ng_pktbuf_realloc_data(entry->pkt, entry->pkt->size +
                                       (frag_size - dg_frag_size)) < 0) {
                DEBUG("6lo rbuf: could not reallocate packet data.\n");
                return;
            }

            /* move already inserted fragments (frag_size - dg_frag_size) to the right */
            if (entry->cur_size > 0) {
                for (int i = entry->pkt->size - (frag_size - dg_frag_size); i > 0; i--) {
                    uint8_t *d = ((uint8_t *)(entry->pkt->data)) + i;
                    *d = *(d - 1);
                }
            }
        }

        DEBUG("6lo rbuf: add fragment data\n");
        entry->cur_size += (uint16_t)dg_frag_size;
        memcpy(((uint8_t *)entry->pkt->data) + offset, data, frag_size);
    }

    if (entry->cur_size == entry->datagram_size) {
        kernel_pid_t iface = netif_hdr->if_pid;
        ng_pktsnip_t *netif = ng_netif_hdr_build(entry->src, entry->src_len,
                              entry->dst, entry->dst_len);

        if (netif == NULL) {
            DEBUG("6lo rbuf: error allocating netif header\n");
            ng_pktbuf_release(entry->pkt);
            return;
        }

        netif_hdr = netif->data;
        netif_hdr->if_pid = iface;
        LL_APPEND(entry->pkt, netif);

        DEBUG("6lo rbuf: datagram complete, send to self\n");
        ng_netapi_receive(thread_getpid(), entry->pkt);
        _rbuf_rem(entry);
    }
}

static inline bool _rbuf_int_in(rbuf_int_t *i, uint16_t start, uint16_t end)
{
    return (((i->start < start) && (start <= i->end)) ||
            ((start < i->start) && (i->start <= end)) ||
            ((i->start == start) && (i->end == end)));
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

static void _rbuf_rem(rbuf_t *entry)
{
    while (entry->ints != NULL) {
        rbuf_int_t *next = entry->ints->next;

        entry->ints->start = 0;
        entry->ints->end = 0;
        entry->ints->next = NULL;
        entry->ints = next;
    }

    entry->pkt = NULL;
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
          new->start, new->end, ng_netif_addr_to_str(l2addr_str,
                  sizeof(l2addr_str), entry->src, entry->src_len));
    DEBUG("%s, %u, %" PRIu16 ")\n", ng_netif_addr_to_str(l2addr_str,
          sizeof(l2addr_str), entry->dst, entry->dst_len), entry->datagram_size,
          entry->tag);

    LL_PREPEND(entry->ints, new);

    return true;
}

static void _rbuf_gc(void)
{
    rbuf_t *oldest = NULL;
    timex_t now;
    unsigned int i;

    vtimer_now(&now);

    for (i = 0; i < RBUF_SIZE; i++) {
        if (rbuf[i].pkt == NULL) { /* leave GC early if there is still room */
            return;
        }
        else if ((rbuf[i].pkt != NULL) &&
                 ((now.seconds - rbuf[i].arrival) > RBUF_TIMEOUT)) {
            DEBUG("6lo rfrag: entry (%s, ", ng_netif_addr_to_str(l2addr_str,
                  sizeof(l2addr_str), rbuf[i].src, rbuf[i].src_len));
            DEBUG("%s, %u, %" PRIu16 ") timed out\n",
                  ng_netif_addr_to_str(l2addr_str, sizeof(l2addr_str), rbuf[i].dst,
                                       rbuf[i].dst_len),
                  rbuf[i].datagram_size, rbuf[i].tag);

            ng_pktbuf_release(rbuf[i].pkt);
            _rbuf_rem(&(rbuf[i]));
        }
        else if ((oldest == NULL) || (rbuf[i].arrival < oldest->arrival)) {
            oldest = &(rbuf[i]);
        }
    }

    if ((i >= RBUF_SIZE) && (oldest != NULL) && (oldest->pkt != NULL)) {
        DEBUG("6lo rfrag: reassembly buffer full, remove oldest entry\n");
        ng_pktbuf_release(oldest->pkt);
        _rbuf_rem(oldest);
    }
}

static rbuf_t *_rbuf_get(const void *src, size_t src_len,
                         const void *dst, size_t dst_len,
                         size_t size, uint16_t tag)
{
    rbuf_t *res = NULL;
    timex_t now;

    vtimer_now(&now);

    for (unsigned int i = 0; i < RBUF_SIZE; i++) {
        /* check first if entry already available */
        if ((rbuf[i].pkt != NULL) && (rbuf[i].datagram_size == size) &&
            (rbuf[i].tag == tag) && (rbuf[i].src_len == src_len) &&
            (rbuf[i].dst_len == dst_len) &&
            (memcmp(rbuf[i].src, src, src_len) == 0) &&
            (memcmp(rbuf[i].dst, dst, dst_len) == 0)) {
            DEBUG("6lo rfrag: entry %p (%s, ", (void *)(&rbuf[i]),
                  ng_netif_addr_to_str(l2addr_str, sizeof(l2addr_str),
                                       rbuf[i].src, rbuf[i].src_len));
            DEBUG("%s, %u, %" PRIu16 ") found\n",
                  ng_netif_addr_to_str(l2addr_str, sizeof(l2addr_str),
                                       rbuf[i].dst, rbuf[i].dst_len),
                  rbuf[i].datagram_size, rbuf[i].tag);
            rbuf[i].arrival = now.seconds;
            return &(rbuf[i]);
        }

        /* if there is a free spot: remember it */
        if ((res == NULL) && (rbuf[i].cur_size == 0)) {
            res = &(rbuf[i]);
        }
    }

    if (res != NULL) { /* entry not in buffer but found empty spot */
        res->pkt = ng_pktbuf_add(NULL, NULL, size, NG_NETTYPE_SIXLOWPAN);
        *((uint64_t *)res->pkt->data) = 0;  /* clean first few bytes for later
                                             * look-ups */

        if (res->pkt == NULL) {
            DEBUG("6lo rfrag: can not allocate reassembly buffer space.\n");
            return NULL;
        }

        res->arrival = now.seconds;
        memcpy(res->src, src, src_len);
        memcpy(res->dst, dst, dst_len);
        res->src_len = src_len;
        res->dst_len = dst_len;
        res->tag = tag;
        res->datagram_size = size;
        res->cur_size = 0;

        DEBUG("6lo rfrag: entry %p (%s, ", (void *)res,
              ng_netif_addr_to_str(l2addr_str, sizeof(l2addr_str), res->src,
                                   res->src_len));
        DEBUG("%s, %u, %" PRIu16 ") created\n",
              ng_netif_addr_to_str(l2addr_str, sizeof(l2addr_str), res->dst,
                                   res->dst_len), res->datagram_size, res->tag);
    }

    return res;
}

/** @} */
