/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Cenk Gündoğan <cnkgndgn@gmail.com>
 * @author Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>

#include "utlist.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/icmpv6/error.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/ext/frag.h"
#include "net/gnrc/ipv6/ext/opt.h"
#include "net/gnrc/ipv6/ext/rh.h"
#if defined(MODULE_GNRC_SIXLOWPAN_IPHC_NHC) && \
    defined(MODULE_GNRC_IPV6_EXT_FRAG)
#include "net/udp.h"
#endif  /* defined(MODULE_GNRC_SIXLOWPAN_IPHC_NHC) &&
         * defined(MODULE_GNRC_IPV6_EXT_FRAG) */

#include "net/gnrc/ipv6/ext.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Checks if @p protnum is a duplicate @ref PROTNUM_IPV6_EXT_HOPOPT.
 *
 * If @p protnum is @ref PROTNUM_IPV6_EXT_HOPOPT, the function sends a parameter
 * problem message (if @ref net_gnrc_icmpv6_error is available), releases the
 * packet with error `EINVAL`, and returns `false`.
 *
 * @note    This function should only be called *after* the first hop-by-hop
 *          option header in the packet was parsed (if it exists) or if the
 *          first extension header wasn't a hop-by-hop option header.
 *
 * @param[in] pkt       A packet in receive order to be checked with the current
 *                      extension header (of type @p protnum) as its payload.
 * @param[in] protnum   The protocol number of the header in
 *                      gnrc_pktsnip_t::data of @p pkt.
 *
 * @return  true, when @p protnum == @ref PROTNUM_IPV6_EXT_HOPOPT.
 * @return  false, when @p protnum != @ref PROTNUM_IPV6_EXT_HOPOPT.
 */
static bool _duplicate_hopopt(gnrc_pktsnip_t *pkt, unsigned protnum);

/**
 * @brief   Demultiplex an extension header according to @p protnum.
 *
 * This includes dispatching it to interested parties in the
 * @ref net_gnrc_netreg.
 *
 * @param[in] pkt       A packet with the first snip pointing to the extension
 *                      header to process.
 * @param[in] protnum   Protocol number of @p pkt.
 *
 * @return  The packet for further processing.
 * @return  NULL, on error or if packet was consumed (by e.g. forwarding via
 *          a routing header). @p pkt is released in case of error.
 */
static gnrc_pktsnip_t *_demux(gnrc_pktsnip_t *pkt, unsigned protnum);

gnrc_pktsnip_t *gnrc_ipv6_ext_process_hopopt(gnrc_pktsnip_t *pkt,
                                             uint8_t *protnum)
{
    if ((*protnum == PROTNUM_IPV6_EXT_HOPOPT) && (pkt != NULL)) {
        /* if a hop-by-hop options header is within the IPv6 packet it comes
         * immediately after the IPv6 header and it must be processed before
         * the packet is forwarded
         * (see https://tools.ietf.org/html/rfc8200#section-4.1) */
        if ((pkt = _demux(pkt, *protnum)) == NULL) {
            DEBUG("ipv6 ext: packet was consumed in extension header "
                  "handling\n");
            return NULL;
        }
        /* packet can only be unmarked (since we are the first one to
         * process this packet and 6LoWPAN only provides packets unmarked)
         *
         * pkt -----------> pkt->next --------> pkt->next->next
         *  v                     v                         v
         * next header          hop-by-hop option       IPv6 header */
        assert((pkt->next != NULL) && (pkt->next->next != NULL) &&
               (pkt->next->next->type == GNRC_NETTYPE_IPV6));
        *protnum = ((ipv6_ext_t *)pkt->next->data)->nh;
        if (_duplicate_hopopt(pkt, *protnum)) {
            return NULL;
        }
    }
    return pkt;
}

gnrc_pktsnip_t *gnrc_ipv6_ext_process_all(gnrc_pktsnip_t *pkt,
                                          uint8_t *protnum)
{
    bool is_ext = true;
#if defined(MODULE_GNRC_SIXLOWPAN_IPHC_NHC) && \
    defined(MODULE_GNRC_IPV6_EXT_FRAG)
    bool is_frag = false;
#endif  /* defined(MODULE_GNRC_SIXLOWPAN_IPHC_NHC) &&
         * defined(MODULE_GNRC_IPV6_EXT_FRAG) */

    while (is_ext) {
#if defined(MODULE_GNRC_SIXLOWPAN_IPHC_NHC) && \
    defined(MODULE_GNRC_IPV6_EXT_FRAG)
        if (*protnum == PROTNUM_IPV6_EXT_FRAG) {
            /* just assigning the if expression might override it if
             * fragment header is not the last extension header ;-) */
            is_frag = true;
        }
#endif  /* defined(MODULE_GNRC_SIXLOWPAN_IPHC_NHC) &&
         * defined(MODULE_GNRC_IPV6_EXT_FRAG) */
        switch (*protnum) {
            case PROTNUM_IPV6_EXT_DST:
            case PROTNUM_IPV6_EXT_RH:
            case PROTNUM_IPV6_EXT_FRAG:
            case PROTNUM_IPV6_EXT_AH:
            case PROTNUM_IPV6_EXT_ESP:
            case PROTNUM_IPV6_EXT_MOB: {
                ipv6_ext_t *ext_hdr;
                uint8_t nh;

                DEBUG("ipv6: handle extension header (protnum = %u)\n",
                      *protnum);
                ext_hdr = pkt->data;
                nh = ext_hdr->nh;
                if ((pkt = _demux(pkt, *protnum)) == NULL) {
                    DEBUG("ipv6: packet was consumed by extension header "
                          "handling\n");
                    return NULL;
                }
                *protnum = nh;
                if (_duplicate_hopopt(pkt, nh)) {
                    return NULL;
                }
                break;
            }
#if defined(MODULE_GNRC_SIXLOWPAN_IPHC_NHC) && \
    defined(MODULE_GNRC_IPV6_EXT_FRAG)
            case PROTNUM_UDP:
                if (is_frag) {
                    DEBUG("gnrc_ipv6_ext: adapting compressed header length\n");
                    /* we can only really now determine the length of the UDP
                     * packet, after the IPv6 datagram is fully reassembled */
                    udp_hdr_t *udp_hdr = pkt->data;

                    udp_hdr->length = byteorder_htons((uint16_t)pkt->size);
                }
                is_ext = false;
                break;
            case PROTNUM_IPV6:
                if (is_frag) {
                    DEBUG("gnrc_ipv6_ext: adapting compressed header length\n");
                    /* we can only really now determine the length of the
                     * encapsulated IPv6 header, after the IPv6 datagram is
                     * fully reassembled */
                    ipv6_hdr_t *ipv6_hdr = pkt->data;

                    ipv6_hdr->len = byteorder_htons((uint16_t)pkt->size -
                                                    sizeof(ipv6_hdr_t));
                }
                is_ext = false;
                break;
#endif  /* defined(MODULE_GNRC_SIXLOWPAN_IPHC_NHC) &&
         * defined(MODULE_GNRC_IPV6_EXT_FRAG) */
            default:
                is_ext = false;
                break;
        }
    }
    return pkt;
}

static bool _duplicate_hopopt(gnrc_pktsnip_t *pkt, unsigned protnum)
{
    if (protnum == PROTNUM_IPV6_EXT_HOPOPT) {
        DEBUG("ipv6: duplicate Hop-by-Hop header\n");
#ifdef MODULE_GNRC_ICMPV6_ERROR
        gnrc_icmpv6_error_param_prob_send(
                ICMPV6_ERROR_PARAM_PROB_NH,
                &((ipv6_ext_t *)pkt->next->data)->nh, pkt
            );
#endif
        gnrc_pktbuf_release_error(pkt, EINVAL);
        return true;
    }
    return false;
}

/**
 * @brief       Marks an IPv6 extension header according to the length field
 *              provided by the extension header itself.
 *
 * @param[in] pkt   The packet, with the extension header that is to be marked
 *                  as first snip
 *
 * @return  The marked snip
 * @return  NULL when no space in packet buffer left or the length field of
 *          the extension header results in a number of bytes greater than
 *          gnrc_pktsnip_t::size of @p pkt. In both error cases, @p pkt is
 *          released.
 */
static gnrc_pktsnip_t *_mark_extension_header(gnrc_pktsnip_t *pkt)
{
    ipv6_ext_t *ext = (ipv6_ext_t *)pkt->data;
    size_t hdr_size = ((ext->len * IPV6_EXT_LEN_UNIT) + IPV6_EXT_LEN_UNIT);
    gnrc_pktsnip_t *ext_snip = gnrc_pktbuf_mark(pkt, hdr_size,
                                                GNRC_NETTYPE_IPV6_EXT);

    if (ext_snip == NULL) {
        gnrc_pktbuf_release(pkt);
    }
    return ext_snip;
}

static inline bool _has_valid_size(gnrc_pktsnip_t *pkt, uint8_t protnum)
{
    ipv6_ext_t *ext;

    if (pkt->size < sizeof(ipv6_ext_t)) {
        return false;
    }

    ext = pkt->data;

    switch (protnum) {
        case PROTNUM_IPV6_EXT_RH:
        case PROTNUM_IPV6_EXT_HOPOPT:
        case PROTNUM_IPV6_EXT_DST:
        case PROTNUM_IPV6_EXT_FRAG:
        case PROTNUM_IPV6_EXT_AH:
        case PROTNUM_IPV6_EXT_ESP:
        case PROTNUM_IPV6_EXT_MOB:
            return ((ext->len * IPV6_EXT_LEN_UNIT) + IPV6_EXT_LEN_UNIT) <= pkt->size;

        default:
            return true;
    }
}

static gnrc_pktsnip_t *_demux(gnrc_pktsnip_t *pkt, unsigned protnum)
{
    DEBUG("ipv6_ext: next header = %u\n", protnum);
    if (!_has_valid_size(pkt, protnum)) {
        DEBUG("ipv6_ext: invalid size\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }
    if (gnrc_netreg_num(GNRC_NETTYPE_IPV6, protnum) > 0) {
        gnrc_pktsnip_t *sub_pkt;

        /* enforce duplication so we can dispatch packet to subscriber(s) and
         * handle the packet */
        gnrc_pktbuf_hold(pkt, 1);
        sub_pkt = gnrc_pktbuf_start_write(pkt);
        if (sub_pkt != NULL) {
            /* check in case subscriber unregistered in the meantime */
            if (gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6, protnum,
                                             sub_pkt) == 0) {
                gnrc_pktbuf_release(sub_pkt);
            };
        }
        else {
            gnrc_pktbuf_release(pkt);
            /* try to keep handling, the sockets aren't *that* important anyway :-P */
        }
    }
    switch (protnum) {
        case PROTNUM_IPV6_EXT_RH:
#ifdef MODULE_GNRC_IPV6_EXT_RH
            switch (gnrc_ipv6_ext_rh_process(pkt)) {
                case GNRC_IPV6_EXT_RH_AT_DST:
                    /* We are the final destination of the route laid out in
                     * the routing header. So proceeds like normal packet. */
                    if (_mark_extension_header(pkt) == NULL) {
                        /* we couldn't mark the routing header though, return
                         * an error */
                        return NULL;
                    }
                    break;

                default:
                    /* unexpected return value => error */
                    gnrc_pktbuf_release(pkt);
                    /* Intentionally falls through */
                case GNRC_IPV6_EXT_RH_ERROR:
                    /* already released by gnrc_ipv6_ext_rh_process, so no
                     * release here */
                case GNRC_IPV6_EXT_RH_FORWARDED:
                    /* the packet is forwarded and released. finish processing */
                    return NULL;
            }

            break;
#endif  /* MODULE_GNRC_IPV6_EXT_RH */
        case PROTNUM_IPV6_EXT_FRAG:
#ifdef MODULE_GNRC_IPV6_EXT_FRAG
            return gnrc_ipv6_ext_frag_reass(pkt);
#endif  /* MODULE_GNRC_IPV6_EXT_FRAG */
        case PROTNUM_IPV6_EXT_HOPOPT:
        case PROTNUM_IPV6_EXT_DST:
            if (IS_USED(MODULE_GNRC_IPV6_EXT_OPT)) {
                return gnrc_ipv6_ext_opt_process(pkt, protnum);
            }
            /* Intentionally falls through */
        case PROTNUM_IPV6_EXT_AH:
        case PROTNUM_IPV6_EXT_ESP:
        case PROTNUM_IPV6_EXT_MOB:
            DEBUG("ipv6_ext: skipping over unsupported extension header\n");
            if (_mark_extension_header(pkt) == NULL) {
                /* unable mark it though to get it out of the way of the
                 * payload, so return an error */
                return NULL;
            }
            break;

        default:
            break;
    }
    return pkt;
}

gnrc_pktsnip_t *gnrc_ipv6_ext_build(gnrc_pktsnip_t *ipv6, gnrc_pktsnip_t *next,
                                    uint8_t nh, size_t size)
{
    gnrc_pktsnip_t *prev = NULL, *snip;
    ipv6_ext_t *ext;

    if (size < IPV6_EXT_LEN_UNIT) {
        return NULL;
    }

    if (ipv6 != NULL) {
        prev = gnrc_pkt_prev_snip(ipv6, next);

        if (prev == NULL) {
            return NULL;
        }
    }

    snip = gnrc_pktbuf_add(next, NULL, size, GNRC_NETTYPE_IPV6_EXT);

    if (snip == NULL) {
        return NULL;
    }

    ext = snip->data;

    ext->nh = nh;

    if (size & 0x7) { /* not divisible by eight */
        ext->len = (size / IPV6_EXT_LEN_UNIT);
    }
    else {
        ext->len = (size / IPV6_EXT_LEN_UNIT) - 1;
    }

    if (prev != NULL) {
        prev->next = snip;
    }

    return snip;
}

/** @} */
