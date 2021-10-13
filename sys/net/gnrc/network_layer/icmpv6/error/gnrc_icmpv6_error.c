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

#include <assert.h>

#include "net/ipv6.h"
#include "net/gnrc/icmpv6.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/pktbuf.h"

#include "net/gnrc/icmpv6/error.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* all error messages are basically the same size and format */
#define ICMPV6_ERROR_SZ (sizeof(icmpv6_error_dst_unr_t))
#define ICMPV6_ERROR_SET_VALUE(data, value) \
    ((icmpv6_error_pkt_too_big_t *)(data))->mtu = byteorder_htonl(value)

#undef MIN
#define MIN(a, b)   ((a) < (b)) ? (a) : (b)

/**
 * @brief   Get packet fit.
 *
 * Gets the minimum size for an ICMPv6 error message packet, based on the
 * invoking packet's size and the interface the invoking packet came over.
 *
 * @param[in] orig_pkt  The invoking packet
 *
 * @return  The supposed size of the ICMPv6 error message.
 */
static size_t _fit(const gnrc_pktsnip_t *orig_pkt)
{
    /* discarding const qualifier is safe here */
    gnrc_pktsnip_t *netif_hdr = gnrc_pktsnip_search_type(
            (gnrc_pktsnip_t *)orig_pkt, GNRC_NETTYPE_NETIF
        );
    size_t pkt_len = gnrc_pkt_len(orig_pkt) + ICMPV6_ERROR_SZ +
                     sizeof(ipv6_hdr_t);

    if (netif_hdr) {
        gnrc_netif_t *netif = gnrc_netif_hdr_get_netif(netif_hdr->data);

        pkt_len -= netif_hdr->size;
        DEBUG("gnrc_icmpv6_error: fitting to MTU of iface %u (%u)\n",
              netif->pid, netif->ipv6.mtu);
        return MIN(pkt_len, netif->ipv6.mtu - sizeof(ipv6_hdr_t));
    }
    else {
        /* packet does not have a netif header (most likely because it did not
         * came from remote) => just assume pkt_len as ideal */
        DEBUG("gnrc_icmpv6_error: copying whole packet\n");
        return pkt_len;
    }
}

static inline bool _in_orig_pkt(const gnrc_pktsnip_t *orig_pkt)
{
    return (orig_pkt != NULL) && (orig_pkt->type != GNRC_NETTYPE_NETIF);
}

static size_t _copy_rcv_snip(gnrc_pktsnip_t *pkt,
                             const gnrc_pktsnip_t *orig_snip)
{
    /* always skip ICMPv6 error header */
    size_t offset = ICMPV6_ERROR_SZ;
    const gnrc_pktsnip_t *ptr = orig_snip;

    while (_in_orig_pkt(ptr->next)) {
        offset += ptr->next->size;
        ptr = ptr->next;
    }

    if (offset < pkt->size) {
        uint8_t *data = pkt->data;

        memcpy(data + offset, orig_snip->data,
               MIN(pkt->size - offset, orig_snip->size));
    }
    return offset;
}

static inline bool _check_send_order(const gnrc_pktsnip_t *pkt)
{
    /* sent packets in IPv6 start either with netif header or
     * with IPv6 header (but then the NETIF header doesn't follow) */
    return (pkt->type == GNRC_NETTYPE_NETIF) ||
           ((pkt->type == GNRC_NETTYPE_IPV6) &&
            ((pkt->next == NULL) || (pkt->next->type != GNRC_NETTYPE_NETIF)));
}

/* Build a generic error message */
static gnrc_pktsnip_t *_icmpv6_error_build(uint8_t type, uint8_t code,
                                           const gnrc_pktsnip_t *orig_pkt,
                                           uint32_t value)
{
    gnrc_pktsnip_t *pkt = gnrc_icmpv6_build(NULL, type, code, _fit(orig_pkt));

    /* copy as much of the originating packet into error message as fits the
     * message's size */
    if (pkt != NULL) {
        ICMPV6_ERROR_SET_VALUE(pkt->data, value);
        if (_check_send_order(orig_pkt)) {
            const gnrc_pktsnip_t *ptr = (orig_pkt->type == GNRC_NETTYPE_NETIF)
                                      ? orig_pkt->next
                                      : orig_pkt;
            size_t offset = ICMPV6_ERROR_SZ;

            while ((ptr != NULL) && (offset < pkt->size)) {
                uint8_t *data = pkt->data;

                memcpy(data + offset, ptr->data, MIN(pkt->size - offset,
                                                     ptr->size));
                offset += ptr->size;
                ptr = ptr->next;
            }
        }
        else {
            while (_in_orig_pkt(orig_pkt)) {
                _copy_rcv_snip(pkt, orig_pkt);
                orig_pkt = orig_pkt->next;
            }
        }
    }

    return pkt;
}

static inline gnrc_pktsnip_t *_dst_unr_build(uint8_t code,
                                             const gnrc_pktsnip_t *orig_pkt)
{
    return _icmpv6_error_build(ICMPV6_DST_UNR, code, orig_pkt, 0);
}

static inline gnrc_pktsnip_t *_pkt_too_big_build(uint32_t mtu,
                                                 const gnrc_pktsnip_t *orig_pkt)
{
    return _icmpv6_error_build(ICMPV6_PKT_TOO_BIG, 0, orig_pkt, mtu);
}

static inline gnrc_pktsnip_t *_time_exc_build(uint8_t code,
                                              const gnrc_pktsnip_t *orig_pkt)
{
    return _icmpv6_error_build(ICMPV6_TIME_EXC, code, orig_pkt, 0);
}

static inline bool _in_range(uint8_t *ptr, uint8_t *start, size_t sz)
{
    return (ptr >= start) && (ptr < (start + sz));
}

static gnrc_pktsnip_t *_param_prob_build(uint8_t code, void *ptr,
                                         const gnrc_pktsnip_t *orig_pkt)
{
    gnrc_pktsnip_t *pkt = gnrc_icmpv6_build(NULL, ICMPV6_PARAM_PROB, code,
                                            _fit(orig_pkt));

    /* copy as much of the originating packet into error message and
     * determine relative *ptr* offset */
    if (pkt != NULL) {
        icmpv6_error_param_prob_t *hdr = pkt->data;
        uint32_t ptr_offset = 0U;

        while (_in_orig_pkt(orig_pkt)) {
            /* copy as long as it fits into packet; parameter problem can only
             * come from received packets */
            size_t offset = _copy_rcv_snip(pkt, orig_pkt);

            if (_in_range(ptr, orig_pkt->data, orig_pkt->size)) {
                ptr_offset = (uint32_t)(((uint8_t *)ptr) -
                                        ((uint8_t *)orig_pkt->data) +
                                        (offset - ICMPV6_ERROR_SZ));
            }
            orig_pkt = orig_pkt->next;
        }

        /* set "pointer" field to relative pointer offset */
        hdr->ptr = byteorder_htonl(ptr_offset);
    }

    return pkt;
}

static void _send(gnrc_pktsnip_t *pkt, const gnrc_pktsnip_t *orig_pkt,
                  gnrc_pktsnip_t *ipv6)
{
    if (pkt != NULL) {
        /* discarding const qualifier is safe here */
        gnrc_pktsnip_t *netif = gnrc_pktsnip_search_type((gnrc_pktsnip_t *)orig_pkt,
                                                         GNRC_NETTYPE_NETIF);
        assert(ipv6 != NULL);
        ipv6_hdr_t *ipv6_hdr = ipv6->data;
        /* overwrite ipv6 parameter pointer ... we don't need it after this */
        ipv6 = gnrc_ipv6_hdr_build(pkt, NULL, &ipv6_hdr->src);
        if (ipv6 == NULL) {
            DEBUG("gnrc_icmpv6_error: No space in packet buffer left\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        pkt = ipv6;
        if (netif) {
            /* copy interface from original netif header to assure packet
             * goes out where it came from */
            gnrc_netif_t *iface = gnrc_netif_hdr_get_netif(netif->data);

            netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
            if (netif == NULL) {
                DEBUG("gnrc_icmpv6_error: No space in packet buffer left\n");
                gnrc_pktbuf_release(pkt);
                return;
            }
            gnrc_netif_hdr_set_netif(netif->data, iface);
            pkt = gnrc_pkt_prepend(pkt, netif);
        }
        if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6,
                                       GNRC_NETREG_DEMUX_CTX_ALL,
                                       pkt)) {
            DEBUG("gnrc_icmpv6_error: No send handler found.\n");
            gnrc_pktbuf_release(pkt);
        }
    }
    else {
        DEBUG("gnrc_icmpv6_error: No space in packet buffer left\n");
    }
}

static gnrc_pktsnip_t *_check_ipv6_hdr(const gnrc_pktsnip_t *orig_pkt,
                                       uint8_t type, uint8_t code)
{
    /* discarding const qualifier is safe here */
    gnrc_pktsnip_t *ipv6 = gnrc_pktsnip_search_type((gnrc_pktsnip_t *)orig_pkt,
                                                    GNRC_NETTYPE_IPV6);
    assert(ipv6 != NULL);
    const ipv6_hdr_t *ipv6_hdr = ipv6->data;

    /* see https://tools.ietf.org/html/rfc4443#section-2.4 (e.6) */
    if (ipv6_addr_is_unspecified(&ipv6_hdr->src) ||
        ipv6_addr_is_multicast(&ipv6_hdr->src)) {
        ipv6 = NULL;
    }
    /* ipv6_hdr->dst may only be multicast for Packet Too Big Messages
     * and Parameter Problem Messages with code 2, see
     * https://tools.ietf.org/html/rfc4443#section-2.4 (e.3) */
    else if (ipv6_addr_is_multicast(&ipv6_hdr->dst)) {
        if ((type != ICMPV6_PKT_TOO_BIG) &&
            ((type != ICMPV6_PARAM_PROB) ||
             (code != ICMPV6_ERROR_PARAM_PROB_OPT))) {
            ipv6 = NULL;
        }
    }
    return ipv6;
}

void gnrc_icmpv6_error_dst_unr_send(uint8_t code, const gnrc_pktsnip_t *orig_pkt)
{
    gnrc_pktsnip_t *ipv6 = _check_ipv6_hdr(orig_pkt, ICMPV6_DST_UNR, code);

    if (ipv6 != NULL) {
        gnrc_pktsnip_t *pkt = _dst_unr_build(code, orig_pkt);

        DEBUG("gnrc_icmpv6_error: trying to send destination unreachable error\n");
        _send(pkt, orig_pkt, ipv6);
    }
}

void gnrc_icmpv6_error_pkt_too_big_send(uint32_t mtu,
                                        const gnrc_pktsnip_t *orig_pkt)
{
    gnrc_pktsnip_t *ipv6 = _check_ipv6_hdr(orig_pkt, ICMPV6_PKT_TOO_BIG, 0);

    if (ipv6 != NULL) {
        gnrc_pktsnip_t *pkt = _pkt_too_big_build(mtu, orig_pkt);

        DEBUG("gnrc_icmpv6_error: trying to send packet too big error\n");
        _send(pkt, orig_pkt, ipv6);
    }
}

void gnrc_icmpv6_error_time_exc_send(uint8_t code,
                                     const gnrc_pktsnip_t *orig_pkt)
{
    gnrc_pktsnip_t *ipv6 = _check_ipv6_hdr(orig_pkt, ICMPV6_TIME_EXC, code);

    if (ipv6 != NULL) {
        gnrc_pktsnip_t *pkt = _time_exc_build(code, orig_pkt);

        DEBUG("gnrc_icmpv6_error: trying to send time exceeded error\n");
        _send(pkt, orig_pkt, ipv6);
    }
}

void gnrc_icmpv6_error_param_prob_send(uint8_t code, void *ptr,
                                       const gnrc_pktsnip_t *orig_pkt)
{
    gnrc_pktsnip_t *ipv6 = _check_ipv6_hdr(orig_pkt, ICMPV6_PARAM_PROB, code);

    if (ipv6 != NULL) {
        gnrc_pktsnip_t *pkt = _param_prob_build(code, ptr, orig_pkt);

        DEBUG("gnrc_icmpv6_error: trying to send parameter problem error\n");
        _send(pkt, orig_pkt, ipv6);
    }
}

/** @} */
