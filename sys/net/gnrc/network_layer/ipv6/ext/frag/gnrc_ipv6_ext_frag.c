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
#include <errno.h>
#include <stdbool.h>

#include "byteorder.h"
#include "net/ipv6/ext/frag.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/ext.h"
#include "net/gnrc/ipv6/ext/frag.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pktbuf.h"
#include "random.h"
#include "sched.h"
#include "xtimer.h"

#include "net/gnrc/ipv6/ext/frag.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static gnrc_ipv6_ext_frag_send_t _snd_bufs[CONFIG_GNRC_IPV6_EXT_FRAG_SEND_SIZE];
static gnrc_ipv6_ext_frag_rbuf_t _rbuf[CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_SIZE];
static gnrc_ipv6_ext_frag_limits_t _limits_pool[CONFIG_GNRC_IPV6_EXT_FRAG_LIMITS_POOL_SIZE];
static clist_node_t _free_limits;
static xtimer_t _gc_xtimer;
static msg_t _gc_msg = { .type = GNRC_IPV6_EXT_FRAG_RBUF_GC };
static gnrc_ipv6_ext_frag_stats_t _stats;

/**
 * @todo    Implement better mechanism as described in
 *          https://tools.ietf.org/html/rfc7739 (for minimal approach
 *          destination cache is required)
 */
static uint32_t _last_id;

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
    _last_id = random_uint32();
    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_EXT_FRAG_LIMITS_POOL_SIZE; i++) {
        clist_rpush(&_free_limits, (clist_node_t *)&_limits_pool[i]);
    }
}

/*
 * ==================
 * IPv6 fragmentation
 * ==================
 */

/**
 * @brief   Allocates a fragmentation send buffer entry from pool
 *
 * @return  A free fragmentation send buffer entry.
 */
static gnrc_ipv6_ext_frag_send_t *_snd_buf_alloc(void);

/**
 * @brief   Removes a fragmentation send buffer and releases the stored
 *          datagrams and fragments.
 *
 * @param[in] snd_buf   A fragmentation send buffer entry
 * @param[in] error     Error number for the error causing the release
 */
static void _snd_buf_free(gnrc_ipv6_ext_frag_send_t *snd_buf, int error);

/**
 * @brief   Removes a fragmentation send buffer without releasing the stored
 *          datagrams and fragments.
 *
 * @param[in] snd_buf   A fragmentation send buffer entry
 */
static void _snd_buf_del(gnrc_ipv6_ext_frag_send_t *snd_buf);

/**
 * @brief   Determines the last Per-Fragment extension header of a datagram.
 *
 * @see [RFC 8200, section 4.5](https://tools.ietf.org/html/rfc8200#section-4.5)
 *      for definition of _Per-Fragment extension header_
 *
 * @param[in] pkt   An IPv6 datagram
 *
 * @return  The last Per-Fragment extension header in @p pkt.
 * @return  NULL, unexpected error. Should never be reached.
 */
static gnrc_pktsnip_t *_determine_last_per_frag(gnrc_pktsnip_t *pkt);

void gnrc_ipv6_ext_frag_send_pkt(gnrc_pktsnip_t *pkt, unsigned path_mtu)
{
    gnrc_ipv6_ext_frag_send_t *snd_buf = _snd_buf_alloc();
    gnrc_pktsnip_t *last_per_frag;

    assert(pkt->type == GNRC_NETTYPE_NETIF);
    if (snd_buf == NULL) {
        DEBUG("ipv6_ext_frag: can not allocate fragmentation send buffer\n");
        gnrc_pktbuf_release_error(pkt, ENOMEM);
        return;
    }
    last_per_frag = _determine_last_per_frag(pkt);
    snd_buf->per_frag = pkt;
    snd_buf->pkt = last_per_frag->next;
    /* separate per-fragment headers from rest */
    last_per_frag->next = NULL;
    snd_buf->id = _last_id;
    _last_id += random_uint32_range(1, 64);
    snd_buf->path_mtu = path_mtu;
    snd_buf->offset = 0;
    gnrc_ipv6_ext_frag_send(snd_buf);
}

void gnrc_ipv6_ext_frag_send(gnrc_ipv6_ext_frag_send_t *snd_buf)
{
    assert(snd_buf != NULL);
    gnrc_pktsnip_t *last = NULL, *ptr, *to_send = NULL;
    ipv6_ext_frag_t *frag_hdr;
    uint8_t *nh = NULL;
    network_uint16_t *len = NULL;
    msg_t msg;
    /* see if fragment to send fits into the path MTU */
    bool last_fragment = (snd_buf->path_mtu >
                          (gnrc_pkt_len(snd_buf->per_frag->next) +
                           sizeof(ipv6_ext_frag_t) +
                           gnrc_pkt_len(snd_buf->pkt)));
    uint16_t remaining = snd_buf->path_mtu & 0xfff8; /* lower multiple of 8 */

    /* prepare fragment for sending */
    ptr = snd_buf->per_frag;
    if (!last_fragment) {
        /* this won't be the last fragment
         * => we need to duplicate the per-fragment headers */
        gnrc_pktbuf_hold(ptr, 1);
    }
    else {
        /* prevent duplicate release of per_frag */
        snd_buf->per_frag = NULL;
    }
    /* first add per-fragment headers */
    while (ptr) {
        gnrc_pktsnip_t *tmp = gnrc_pktbuf_start_write(ptr);
        if (tmp == NULL) {
            DEBUG("ipv6_ext_frag: packet buffer full, canceling fragmentation\n");
            if (ptr->users > 1) {
                /* we are not the last fragment, so we need to also release
                 * our hold on the snips we did not duplicate so far
                 * and all also release all the snips we did duplicated so far
                 */
                if (to_send != NULL) {
                    gnrc_pktbuf_release(to_send);
                }
                else {
                    gnrc_pktbuf_release(ptr);
                }
            }
            _snd_buf_free(snd_buf, ENOSPC);
            return;
        }
        ptr = tmp;
        if (to_send == NULL) {
            to_send = ptr;
        }
        switch (ptr->type) {
            case GNRC_NETTYPE_IPV6: {
                ipv6_hdr_t *hdr = ptr->data;
                nh = &hdr->nh;
                len = &hdr->len;
                break;
            }
            case GNRC_NETTYPE_IPV6_EXT: {
                ipv6_ext_t *hdr = ptr->data;
                nh = &hdr->nh;
                break;
            }
            default:
                break;
        }
        if (ptr->type != GNRC_NETTYPE_NETIF) {
            remaining -= ptr->size;
        }
        if (last) {
            last->next = ptr;
        }
        last = ptr;
        ptr = ptr->next;
    }
    assert(nh != NULL);
    /* then the fragment header */
    ptr = gnrc_ipv6_ext_build(last, last->next, *nh, sizeof(ipv6_ext_frag_t));
    if (ptr == NULL) {
        DEBUG("ipv6_ext_frag: unable to create fragmentation header\n");
        gnrc_pktbuf_release(to_send);
        _snd_buf_free(snd_buf, ENOSPC);
        return;
    }
    remaining -= sizeof(ipv6_ext_frag_t);
    frag_hdr = ptr->data;
    ipv6_ext_frag_set_offset(frag_hdr, snd_buf->offset);
    if (!last_fragment) {
        ipv6_ext_frag_set_more(frag_hdr);
    }
    frag_hdr->id = byteorder_htonl(snd_buf->id);
    *nh = PROTNUM_IPV6_EXT_FRAG;
    last = ptr;
    /* then the rest */
    while (remaining && snd_buf->pkt) {
        if (last_fragment ||
            (snd_buf->pkt->size <= remaining)) {
            ptr = snd_buf->pkt;
            snd_buf->pkt = ptr->next;
        }
        else {
            ptr = gnrc_pktbuf_mark(snd_buf->pkt, remaining,
                                   GNRC_NETTYPE_UNDEF);
            if (ptr == NULL) {
                DEBUG("ipv6_ext_frag: packet buffer full, canceling fragmentation\n");
                gnrc_pktbuf_release(to_send);
                _snd_buf_free(snd_buf, ENOSPC);
                return;
            }
            assert(snd_buf->pkt->next == ptr);  /* we just created it with mark */
            snd_buf->pkt->next = snd_buf->pkt->next->next;
        }
        ptr->next = NULL;
        last->next = ptr;
        last = ptr;
        remaining -= ptr->size;
        snd_buf->offset += ptr->size;
    }
    assert(len != NULL);
    /* adapt IPv6 header length field */
    *len = byteorder_htons(gnrc_pkt_len(to_send->next->next));
    /* tell gnrc_ipv6 to send the above prepared fragment */
    msg.type = GNRC_IPV6_EXT_FRAG_SEND;
    msg.content.ptr = to_send;
    if (msg_try_send(&msg, gnrc_ipv6_pid) <= 0) {
        DEBUG("ipv6_ext_frag: Unable to send fragment, canceling fragmentation\n");
        gnrc_pktbuf_release(to_send);
        _snd_buf_free(snd_buf, ENOMEM);
        return;
    }
    if (last_fragment) {
        /* last fragment => we don't need the send buffer anymore.
         * But as we just sent it to gnrc_ipv6 we still need the packet
         * allocated, so not _snd_buf_free()! */
        _snd_buf_del(snd_buf);
    }
    else {
        /* tell gnrc_ipv6 to continue fragmenting the datagram in snd_buf
         * later */
        msg.type = GNRC_IPV6_EXT_FRAG_CONTINUE;
        msg.content.ptr = snd_buf;
        if (msg_try_send(&msg, gnrc_ipv6_pid) <= 0) {
            DEBUG("ipv6_ext_frag: Unable to continue fragmentation, canceling\n");
            _snd_buf_free(snd_buf, ENOMEM);
        }
    }
}

static gnrc_ipv6_ext_frag_send_t *_snd_buf_alloc(void)
{
    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_EXT_FRAG_SEND_SIZE; i++) {
        gnrc_ipv6_ext_frag_send_t *snd_buf = &_snd_bufs[i];
        if (snd_buf->pkt == NULL) {
            return snd_buf;
        }
    }
    if (IS_USED(MODULE_GNRC_IPV6_EXT_FRAG_STATS)) {
        _stats.frag_full++;
    }
    return NULL;
}

static void _snd_buf_del(gnrc_ipv6_ext_frag_send_t *snd_buf)
{
    snd_buf->per_frag = NULL;
    snd_buf->pkt = NULL;
}

static void _snd_buf_free(gnrc_ipv6_ext_frag_send_t *snd_buf, int error)
{
    if (snd_buf->per_frag) {
        gnrc_pktbuf_release(snd_buf->per_frag);
    }
    if (snd_buf->pkt) {
        gnrc_pktbuf_release_error(snd_buf->pkt, error);
    }
    _snd_buf_del(snd_buf);
}

static gnrc_pktsnip_t *_determine_last_per_frag(gnrc_pktsnip_t *ptr)
{
    gnrc_pktsnip_t *last_per_frag = NULL;
    unsigned nh = PROTNUM_RESERVED;

    /* ignore NETIF header */
    ptr = ptr->next;
    while (ptr) {
        switch (ptr->type) {
            case GNRC_NETTYPE_IPV6: {
                ipv6_hdr_t *hdr = ptr->data;
                last_per_frag = ptr;
                nh = hdr->nh;
                break;
            }
            case GNRC_NETTYPE_IPV6_EXT: {
                ipv6_ext_t *hdr = ptr->data;
                switch (nh) {
                    /* "[...] that is, all headers up to and including the
                     * Routing header if present, else the Hop-by-Hop Options
                     * header if present, [...]"
                     * (IPv6 header comes before Hop-by-Hop Options comes before
                     * Routing header, so an override to keep the quoted
                     * priorities is ensured) */
                    case PROTNUM_IPV6_EXT_HOPOPT:
                    case PROTNUM_IPV6_EXT_RH:
                        last_per_frag = ptr;
                        break;
                    default:
                        break;
                }
                nh = hdr->nh;
                break;
            }
            default:
                assert(last_per_frag != NULL);
                return last_per_frag;
        }
        ptr = ptr->next;
    }
    /* should not be reached */
    assert(false);
    return NULL;
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
    else if (pkt->size == 0) {
        DEBUG("ipv6_ext_frag: fragment empty after removing header\n");
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
    xtimer_set_msg(&_gc_xtimer, CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_TIMEOUT_US, &_gc_msg,
                   thread_getpid());
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
            /* entry did not exist yet */
            rbuf->pkt = gnrc_pktbuf_add(fh_snip->next, NULL, size_until,
                                        GNRC_NETTYPE_UNDEF);
            if (rbuf->pkt == NULL) {
                DEBUG("ipv6_ext_frag: unable to create space for reassembled "
                      "packet\n");
                goto error_exit;
            }
        }
        else if (rbuf->pkt->size < size_until) {
            /* entry exists already but doesn't fit full datagram yet */
            if (gnrc_pktbuf_realloc_data(rbuf->pkt, size_until) != 0) {
                DEBUG("ipv6_ext_frag: unable to allocate space for reassembled "
                      "packet\n");
                goto error_exit;
            }
        }
        /* copy payload of fragment into reassembled datagram */
        memcpy(((uint8_t *)rbuf->pkt->data) + offset, pkt->data, pkt->size);
        /* if entry was newly created above */
        if (rbuf->pkt->next == fh_snip->next) {
            /* we don't need the payload and fragment header anymore, the
             * remaining headers are still kept to be reused when assembled, so
             * just remove the payload. */
            gnrc_pktbuf_remove_snip(pkt, fh_snip);
            gnrc_pktbuf_remove_snip(pkt, pkt);
        }
        else {
            /* we don't need the rest anymore */
            gnrc_pktbuf_release(pkt);
        }
        return _completed(rbuf);
    }
    else if (!ipv6_ext_frag_more(fh)) {
        /* first fragment but actually not fragmented */
        _set_nh(fh_snip->next, nh);
        gnrc_pktbuf_remove_snip(pkt, fh_snip);
        gnrc_ipv6_ext_frag_rbuf_del(rbuf);
        ipv6->len = byteorder_htons(byteorder_ntohs(ipv6->len) -
                                    sizeof(ipv6_ext_frag_t));
        if (IS_USED(MODULE_GNRC_IPV6_EXT_FRAG_STATS)) {
            _stats.fragments++;
            _stats.datagrams++;
        }
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
        else if (rbuf->pkt != NULL && rbuf->pkt->size < pkt->size) {
            DEBUG("ipv6_ext_frag: reassembly buffer too small to fit first "
                  "fragment\n");
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
    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_SIZE; i++) {
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
    if ((res == NULL) &&
        !IS_ACTIVE(CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_DO_NOT_OVERRIDE)) {
        assert(oldest != NULL); /* reassembly buffer is full, so there needs
                                 * to be an oldest entry */
        DEBUG("ipv6_ext_frag: dropping oldest entry\n");
        if (IS_USED(MODULE_GNRC_IPV6_EXT_FRAG_STATS)) {
            _stats.rbuf_full++;
        }
        gnrc_ipv6_ext_frag_rbuf_del(oldest);
        res = oldest;
        _init_rbuf(res, ipv6, id);
    }
    else if (IS_USED(MODULE_GNRC_IPV6_EXT_FRAG_STATS) && (res == NULL)) {
        _stats.rbuf_full++;
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
    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_SIZE; i++) {
        gnrc_ipv6_ext_frag_rbuf_t *rbuf = &_rbuf[i];
        if ((now - rbuf->arrival) > CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_TIMEOUT_US) {
            gnrc_ipv6_ext_frag_rbuf_del(rbuf);
        }
    }
}

gnrc_ipv6_ext_frag_stats_t *gnrc_ipv6_ext_frag_stats(void)
{
    return (IS_USED(MODULE_GNRC_IPV6_EXT_FRAG_STATS)) ? &_stats : NULL;
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
    /* ptr should not be NULL at this point so it is safe to dereference with ptr->start here */
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
        if (IS_USED(MODULE_GNRC_IPV6_EXT_FRAG_STATS)) {
            _stats.fragments += clist_count(&rbuf->limits);
            _stats.datagrams++;
        }
        gnrc_ipv6_ext_frag_rbuf_free(rbuf);
        return res;
    }
    return NULL;
}

/** @} */
