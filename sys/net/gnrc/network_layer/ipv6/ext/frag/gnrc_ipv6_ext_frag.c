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

#include "byteorder.h"
#include "net/ipv6/ext/frag.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/ext.h"
#include "net/gnrc/pktbuf.h"
#include "sched.h"
#include "xtimer.h"

#include "net/gnrc/ipv6/ext/frag.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static gnrc_ipv6_ext_frag_rbuf_t _rbuf[GNRC_IPV6_EXT_FRAG_RBUF_SIZE];
static gnrc_ipv6_ext_frag_limits_t _limits_pool[GNRC_IPV6_EXT_FRAG_LIMITS_POOL_SIZE];
static clist_node_t _free_limits;
static xtimer_t _gc_xtimer;
static msg_t _gc_msg = { .type = GNRC_IPV6_EXT_FRAG_RBUF_GC };

typedef enum {
    FRAG_LIMITS_NEW = 0,        /**< limits are not present and do not overlap */
    FRAG_LIMITS_DUPLICATE,      /**< fragment limits are already present */
    FRAG_LIMITS_OVERLAP,        /**< limits overlap */
    FRAG_LIMITS_FULL,           /**< no free gnrc_ipv6_ext_frag_limits_t object */
} _limits_res_t;

void gnrc_ipv6_ext_frag_init(void)
{
#ifdef TEST_SUITES
    memset(_rbuf, 0, sizeof(_rbuf));
#endif
    for (unsigned i = 0; i < GNRC_IPV6_EXT_FRAG_LIMITS_POOL_SIZE; i++) {
        clist_rpush(&_free_limits, (clist_node_t *)&_limits_pool[i]);
    }
}

/*
 * ===============
 * IPv6 reassembly
 * ===============
 */

/**
 * @brief   Initializes a reassembly buffer entry
 *
 * @param[in] rbuf  A reassembly buffer entry.
 * @param[in] ipv6  The IPv6 header for the reassembly buffer entry.
 * @param[in] id    The identification from the fragment header.
 */
static inline void _init_rbuf(gnrc_ipv6_ext_frag_rbuf_t *rbuf, ipv6_hdr_t *ipv6,
                              uint32_t id);

/**
 * @brief   Checks if given fragment limits overlap with fragment limits already
 *          in a given reassembly buffer entry
 *
 * If no overlap exists the new limits are added to @p rbuf.
 *
 * @param[in, out] rbuf A reassembly buffer entry.
 * @param[in] offset    A fragment offset.
 * @param[in] pkt_len   The length of the packet.
 *
 * @return  see _limits_res_t.
 */
static _limits_res_t _overlaps(gnrc_ipv6_ext_frag_rbuf_t *rbuf,
                               unsigned offset, unsigned pkt_len);

/**
 * @brief   Sets the next header field of a header.
 *
 * @pre `hdr_snip->type` $\in$ {GNRC_NETTYPE_IPV6, GNRC_NETTYPE_IPV6_EXT}
 *
 * @param[in] hdr_snip  A header
 * @param[in] nh        A protocol number
 */
static inline void _set_nh(gnrc_pktsnip_t *hdr_snip, uint8_t nh);

/**
 * @brief   Checks if a fragmented packet is completely reassembled.
 *
 * @param[in] rbuf      A reassembly buffer entry.
 *
 * @return  The reassembled packet on if it is completed.
 * @return  NULL, if the packet is not completely reassembled yet
 */
static gnrc_pktsnip_t *_completed(gnrc_ipv6_ext_frag_rbuf_t *rbuf);

gnrc_pktsnip_t *gnrc_ipv6_ext_frag_reass(gnrc_pktsnip_t *pkt)
{
    gnrc_ipv6_ext_frag_rbuf_t *rbuf;
    gnrc_pktsnip_t *fh_snip, *ipv6_snip;
    ipv6_hdr_t *ipv6;
    ipv6_ext_frag_t *fh;
    unsigned offset;
    uint8_t nh;

    fh_snip = gnrc_pktbuf_mark(pkt, sizeof(ipv6_ext_frag_t),
                               GNRC_NETTYPE_IPV6_EXT);
    if (fh_snip == NULL) {
        DEBUG("ipv6_ext_frag: unable to mark fragmentation header\n");
        goto error_release;
    }
    fh = fh_snip->data;
    /* search IPv6 header */
    ipv6_snip = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    assert(ipv6_snip != NULL);
    ipv6 = ipv6_snip->data;
    rbuf = gnrc_ipv6_ext_frag_rbuf_get(ipv6, byteorder_ntohl(fh->id));
    if (rbuf == NULL) {
        DEBUG("ipv6_ext_frag: reassembly buffer full\n");
        goto error_release;
    }
    rbuf->arrival = xtimer_now_usec();
    xtimer_set_msg(&_gc_xtimer, GNRC_IPV6_EXT_FRAG_RBUF_TIMEOUT_US, &_gc_msg,
                   sched_active_pid);
    nh = fh->nh;
    offset = ipv6_ext_frag_get_offset(fh);
    switch (_overlaps(rbuf, offset, pkt->size)) {
        case FRAG_LIMITS_NEW:
            break;
        case FRAG_LIMITS_DUPLICATE:
            gnrc_pktbuf_release(pkt);
            return NULL;
        case FRAG_LIMITS_OVERLAP:
            DEBUG("ipv6_ext_frag: fragment overlaps with existing fragments\n");
            /* intentionally falls through */
        case FRAG_LIMITS_FULL:
        default:
            DEBUG("ipv6_ext_frag: can't store fragment limits\n");
            goto error_exit;
    }
    if (offset > 0) {
        size_t size_until = offset + pkt->size;

        /* use IPv6 header in reassembly buffer from here on */
        ipv6 = rbuf->ipv6;
        /* subsequent fragment */
        if (!ipv6_ext_frag_more(fh)) {
            /* last fragment; add to rbuf->pkt_len */
            rbuf->last++;
            rbuf->pkt_len += size_until;
        }
        /* not divisible by 8 */
        else if ((pkt->size & 0x7)) {
            DEBUG("ipv6_ext_frag: fragment length not divisible by 8");
            goto error_exit;
        }
        if (rbuf->pkt == NULL) {
            rbuf->pkt = gnrc_pktbuf_add(fh_snip->next, NULL, size_until,
                                        GNRC_NETTYPE_UNDEF);
            if (rbuf->pkt == NULL) {
                DEBUG("ipv6_ext_frag: unable to create space for reassembled "
                      "packet\n");
                goto error_exit;
            }
        }
        else if (rbuf->pkt->size < size_until) {
            if (gnrc_pktbuf_realloc_data(rbuf->pkt, size_until) != 0) {
                DEBUG("ipv6_ext_frag: unable to allocate space for reassembled "
                      "packet\n");
                goto error_exit;
            }
        }
        memcpy(((uint8_t *)rbuf->pkt->data) + offset, pkt->data, pkt->size);
        /* we don't need the rest anymore */
        gnrc_pktbuf_release(pkt);
        return _completed(rbuf);
    }
    else if (!ipv6_ext_frag_more(fh)) {
        /* first fragment but actually not fragmented */
        _set_nh(fh_snip->next, nh);
        gnrc_pktbuf_remove_snip(pkt, fh_snip);
        gnrc_ipv6_ext_frag_rbuf_del(rbuf);
        ipv6->len = byteorder_htons(byteorder_ntohs(ipv6->len) -
                                    sizeof(ipv6_ext_frag_t));
        return pkt;
    }
    else {
        /* first fragment */
        uint16_t ipv6_len = byteorder_ntohs(ipv6->len);

        /* not divisible by 8*/
        if ((pkt->size & 0x7)) {
            DEBUG("ipv6_ext_frag: fragment length not divisible by 8");
            goto error_exit;
        }
        _set_nh(fh_snip->next, nh);
        gnrc_pktbuf_remove_snip(pkt, fh_snip);
        /* TODO: RFC 8200 says "- 8"; determine if `sizeof(ipv6_ext_frag_t)` is
         * really needed*/
        rbuf->pkt_len += ipv6_len - pkt->size - sizeof(ipv6_ext_frag_t);
        if (rbuf->pkt != NULL) {
            /* first fragment but not first arriving */
            memcpy(rbuf->pkt->data, pkt->data, pkt->size);
            rbuf->pkt->next = pkt->next;
            rbuf->pkt->type = pkt->type;
            /* payload was copied to reassembly buffer so remove it */
            gnrc_pktbuf_remove_snip(pkt, pkt);
            rbuf->ipv6 = ipv6;
            return _completed(rbuf);
        }
        else {
            /* first fragment but first arriving */
            rbuf->pkt = pkt;
        }
    }
    return NULL;
error_exit:
    gnrc_ipv6_ext_frag_rbuf_del(rbuf);
error_release:
    gnrc_pktbuf_release(pkt);
    return NULL;
}

gnrc_ipv6_ext_frag_rbuf_t *gnrc_ipv6_ext_frag_rbuf_get(ipv6_hdr_t *ipv6,
                                                       uint32_t id)
{
    gnrc_ipv6_ext_frag_rbuf_t *res = NULL, *oldest = NULL;
    for (unsigned i = 0; i < GNRC_IPV6_EXT_FRAG_RBUF_SIZE; i++) {
        gnrc_ipv6_ext_frag_rbuf_t *tmp = &_rbuf[i];
        if (tmp->ipv6 != NULL) {
            if ((tmp->id == id) &&
                ipv6_addr_equal(&tmp->ipv6->src, &ipv6->src) &&
                ipv6_addr_equal(&tmp->ipv6->dst, &ipv6->dst)) {
                return tmp;
            }
        }
        else if (res == NULL) {
            res = tmp;
            _init_rbuf(res, ipv6, id);
        }
        if ((oldest == NULL) ||
            /* xtimer_now_usec() overflows every ~1.2 hours */
            ((tmp->arrival - oldest->arrival) < (UINT32_MAX / 2))) {
            oldest = tmp;
        }
    }
    if (res == NULL) {
        assert(oldest != NULL); /* reassembly buffer is full, so there needs
                                 * to be an oldest entry */
        DEBUG("ipv6_ext_frag: dropping oldest entry\n");
        gnrc_ipv6_ext_frag_rbuf_del(oldest);
        res = oldest;
        _init_rbuf(res, ipv6, id);
    }
    return res;
}

void gnrc_ipv6_ext_frag_rbuf_free(gnrc_ipv6_ext_frag_rbuf_t *rbuf)
{
    rbuf->ipv6 = NULL;
    while (rbuf->limits.next != NULL) {
        clist_node_t *tmp = clist_lpop(&rbuf->limits);
        clist_rpush(&_free_limits, tmp);
    }
}

void gnrc_ipv6_ext_frag_rbuf_gc(void)
{
    uint32_t now = xtimer_now_usec();
    for (unsigned i = 0; i < GNRC_IPV6_EXT_FRAG_RBUF_SIZE; i++) {
        gnrc_ipv6_ext_frag_rbuf_t *rbuf = &_rbuf[i];
        if ((now - rbuf->arrival) > GNRC_IPV6_EXT_FRAG_RBUF_TIMEOUT_US) {
            gnrc_ipv6_ext_frag_rbuf_del(rbuf);
        }
    }
}

typedef struct {
    uint16_t start;
    uint16_t end;
} _check_limits_t;

static inline void _init_rbuf(gnrc_ipv6_ext_frag_rbuf_t *rbuf, ipv6_hdr_t *ipv6,
                              uint32_t id)
{
    rbuf->ipv6 = ipv6;
    rbuf->id = id;
    rbuf->pkt_len = 0;
    rbuf->last = 0;
}

static int _check_overlap(clist_node_t *node, void *arg)
{
    _check_limits_t *limits = arg;
    gnrc_ipv6_ext_frag_limits_t *cur = (gnrc_ipv6_ext_frag_limits_t *)node;

    return ((cur->start < limits->end) && (limits->start < cur->end));
}

static int _limits_cmp(clist_node_t *a, clist_node_t *b)
{
    gnrc_ipv6_ext_frag_limits_t *al = (gnrc_ipv6_ext_frag_limits_t *)a;
    gnrc_ipv6_ext_frag_limits_t *bl = (gnrc_ipv6_ext_frag_limits_t *)b;

    return (int)al->start - (int)bl->start;
}

static _limits_res_t _overlaps(gnrc_ipv6_ext_frag_rbuf_t *rbuf,
                               unsigned offset, unsigned pkt_len)
{
    _check_limits_t limits = { .start = offset >> 3U,
                               .end = (offset + pkt_len) >> 3U };
    gnrc_ipv6_ext_frag_limits_t *res;

    if (limits.start == limits.end) {
        /* might happen with last fragment */
        limits.end++;
    }
    res = (gnrc_ipv6_ext_frag_limits_t *)clist_foreach(&rbuf->limits,
                                                       _check_overlap,
                                                       &limits);
    if (res == NULL) {
        res = (gnrc_ipv6_ext_frag_limits_t *)clist_lpop(&_free_limits);
        if (res != NULL) {
            res->start = limits.start;
            res->end = limits.end;
            clist_rpush(&rbuf->limits, (clist_node_t *)res);
            clist_sort(&rbuf->limits, _limits_cmp);
            return FRAG_LIMITS_NEW;
        }
        else {
            return FRAG_LIMITS_FULL;
        }
    }
    else if ((res->start == limits.start) && (res->end == limits.end)) {
        return FRAG_LIMITS_DUPLICATE;
    }
    else {
        return FRAG_LIMITS_NEW;
    }
}

static inline void _set_nh(gnrc_pktsnip_t *hdr_snip, uint8_t nh)
{
    switch (hdr_snip->type) {
        case GNRC_NETTYPE_IPV6: {
            ipv6_hdr_t *hdr = hdr_snip->data;
            hdr->nh = nh;
            break;
        }
        case GNRC_NETTYPE_IPV6_EXT: {
            ipv6_ext_t *hdr = hdr_snip->data;
            hdr->nh = nh;
            break;
        }
        default:
            /* should not happen */
            assert(false);
            break;
    }
}

static gnrc_pktsnip_t *_completed(gnrc_ipv6_ext_frag_rbuf_t *rbuf)
{
    assert(rbuf->limits.next != NULL);    /* this function is only called when
                                           * at least one fragment was already
                                           * added */
    /* clist: first element is second element ;-) (from next of head) */
    gnrc_ipv6_ext_frag_limits_t *ptr =
            (gnrc_ipv6_ext_frag_limits_t *)rbuf->limits.next->next;
    if (rbuf->last && (ptr->start == 0)) {
        gnrc_pktsnip_t *res = NULL;

        /* last and first fragment were received, so check if everything
         * in-between is there */
        do {
            gnrc_ipv6_ext_frag_limits_t *next = ptr->next;
            if (ptr->end < next->start) {
                return NULL;
            }
            ptr = next;
        } while (((clist_node_t *)ptr) != rbuf->limits.next);
        res = rbuf->pkt;
        /* rewrite length */
        rbuf->ipv6->len = byteorder_htons(rbuf->pkt_len);
        rbuf->pkt = NULL;
        gnrc_ipv6_ext_frag_rbuf_free(rbuf);
        return res;
    }
    return NULL;
}

/** @} */
