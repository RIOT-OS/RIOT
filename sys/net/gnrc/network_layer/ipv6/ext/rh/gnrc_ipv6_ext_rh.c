/*
 * Copyright (C) 2015 Cenk Gündoğan <cenk.guendogan@fu-berlin.de>
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
 * @author Cenk Gündoğan <cenk.guendogan@fu-berlin.de>
 * @author Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "net/ipv6/ext/rh.h"
#include "net/gnrc.h"

#ifdef MODULE_GNRC_RPL_SRH
#include "net/gnrc/rpl/srh.h"
#endif

#include "net/gnrc/ipv6/ext/rh.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

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

int gnrc_ipv6_ext_rh_process(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *ipv6;
    ipv6_ext_rh_t *ext = pkt->data;
    ipv6_hdr_t *hdr;
    int res = GNRC_IPV6_EXT_RH_AT_DST;

    /* check seg_left early to avoid duplicating the packet */
    if (ext->seg_left == 0) {
        return res;
    }
    ipv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    assert(ipv6 != NULL);
    hdr = ipv6->data;
    switch (ext->type) {
#ifdef MODULE_GNRC_RPL_SRH
        case IPV6_EXT_RH_TYPE_RPL_SRH:
            res = gnrc_rpl_srh_process(hdr, (gnrc_rpl_srh_t *)ext);
            break;
#endif
        default:
            res = GNRC_IPV6_EXT_RH_ERROR;
            break;
    }
    switch (res) {
        case GNRC_IPV6_EXT_RH_FORWARDED:
            _forward_pkt(pkt, hdr);
            break;
        case GNRC_IPV6_EXT_RH_AT_DST:
            break;
        default:
            gnrc_pktbuf_release(pkt);
            break;
    }
    return res;
}

/** @} */
