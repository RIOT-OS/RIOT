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

#include <errno.h>

#include "utlist.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/ext/rh.h"

#include "net/gnrc/ipv6/ext.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

gnrc_pktsnip_t *gnrc_ipv6_ext_process_all(gnrc_pktsnip_t *pkt,
                                          uint8_t *nh)
{
    bool is_ext = true;
    while (is_ext) {
        switch (*nh) {
            case PROTNUM_IPV6_EXT_DST:
            case PROTNUM_IPV6_EXT_RH:
            case PROTNUM_IPV6_EXT_FRAG:
            case PROTNUM_IPV6_EXT_AH:
            case PROTNUM_IPV6_EXT_ESP:
            case PROTNUM_IPV6_EXT_MOB: {
                ipv6_ext_t *ext_hdr;

                DEBUG("ipv6: handle extension header (nh = %u)\n", *nh);
                ext_hdr = pkt->data;
                if ((pkt = gnrc_ipv6_ext_demux(pkt, *nh)) == NULL) {
                    DEBUG("ipv6: packet was consumed by extension header "
                          "handling\n");
                    return NULL;
                }
                *nh = ext_hdr->nh;
                break;
            }
            default:
                is_ext = false;
                break;
        }
    }
    return pkt;
}

#ifdef MODULE_GNRC_IPV6_EXT_RH
/* unchecked precondition: hdr is gnrc_pktsnip_t::data of the
 * GNRC_NETTYPE_IPV6 snip within pkt */
static void _forward_pkt(gnrc_pktsnip_t *pkt, ipv6_hdr_t *hdr)
{
    gnrc_pktsnip_t *netif_snip;

    if (--(hdr->hl) == 0) {
        DEBUG("ipv6_ext_rh: hop limit reached 0: drop packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    /* remove L2 headers around IPV6 */
    netif_snip = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);
    if (netif_snip != NULL) {
        pkt = gnrc_pktbuf_remove_snip(pkt, netif_snip);
    }
    /* reverse packet into send order */
    pkt = gnrc_pktbuf_reverse_snips(pkt);
    if (pkt == NULL) {
        DEBUG("ipv6_ext_rh: can't reverse snip order in packet");
        /* gnrc_pktbuf_reverse_snips() releases pkt on error */
        return;
    }
    /* forward packet */
    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6,
                                   GNRC_NETREG_DEMUX_CTX_ALL,
                                   pkt)) {
        DEBUG("ipv6_ext_rh: could not dispatch packet to the IPv6 "
              "thread\n");
        gnrc_pktbuf_release(pkt);
    }
}

/* handle routing header at pkt->data */
static int _handle_rh(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *ipv6;
    ipv6_ext_t *ext = (ipv6_ext_t *)pkt->data;
    ipv6_hdr_t *hdr;
    int res;

    /* check seg_left early to to exit quickly */
    if (((ipv6_ext_rh_t *)ext)->seg_left == 0) {
        return GNRC_IPV6_EXT_RH_AT_DST;
    }
    ipv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    assert(ipv6 != NULL);
    hdr = ipv6->data;
    switch ((res = gnrc_ipv6_ext_rh_process(hdr, (ipv6_ext_rh_t *)ext))) {
        case GNRC_IPV6_EXT_RH_ERROR:
            /* TODO: send ICMPv6 error codes */
            gnrc_pktbuf_release(pkt);
            break;

        case GNRC_IPV6_EXT_RH_FORWARDED:
            _forward_pkt(pkt, hdr);
            break;

        case GNRC_IPV6_EXT_RH_AT_DST:
            /* this should not happen since we checked seg_left early */
            gnrc_pktbuf_release(pkt);
            break;
    }

    return res;
}
#endif  /* MODULE_GNRC_IPV6_EXT_RH */

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

static inline bool _has_valid_size(gnrc_pktsnip_t *pkt, uint8_t nh)
{
    ipv6_ext_t *ext;

    if (pkt->size < sizeof(ipv6_ext_t)) {
        return false;
    }

    ext = pkt->data;

    switch (nh) {
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

gnrc_pktsnip_t *gnrc_ipv6_ext_demux(gnrc_pktsnip_t *pkt, unsigned nh)
{
    DEBUG("ipv6_ext: next header = %u\n", nh);
    if (!_has_valid_size(pkt, nh)) {
        DEBUG("ipv6_ext: invalid size\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }
    switch (nh) {
        case PROTNUM_IPV6_EXT_RH:
#ifdef MODULE_GNRC_RPL_SRH
            switch (_handle_rh(pkt)) {
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
                    /* already released by _handle_rh, so no release here */
                case GNRC_IPV6_EXT_RH_FORWARDED:
                    /* the packet is forwarded and released. finish processing */
                    return NULL;
            }

            break;
#endif

        case PROTNUM_IPV6_EXT_HOPOPT:
        case PROTNUM_IPV6_EXT_DST:
        case PROTNUM_IPV6_EXT_FRAG:
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
        LL_SEARCH_SCALAR(ipv6, prev, next, next);

        if (prev == NULL) {
            return NULL;
        }
    }

    snip = gnrc_pktbuf_add(next, NULL, size, GNRC_NETTYPE_IPV6);

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
