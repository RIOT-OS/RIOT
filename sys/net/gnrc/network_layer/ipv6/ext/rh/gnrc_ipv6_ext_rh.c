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

#include <assert.h>

#include "net/ipv6/ext/rh.h"
#include "net/gnrc.h"

#include "net/gnrc/icmpv6/error.h"

#ifdef MODULE_GNRC_RPL_SR
#include "net/gnrc/rpl/srh.h"
#endif

#include "net/gnrc/ipv6/ext/rh.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* unchecked precondition: hdr is gnrc_pktsnip_t::data of the
 * GNRC_NETTYPE_IPV6 snip within pkt */
static void _forward_pkt(gnrc_pktsnip_t *pkt, ipv6_hdr_t *hdr)
{
    gnrc_pktsnip_t *netif_snip;

    if (--(hdr->hl) == 0) {
        DEBUG("ipv6_ext_rh: hop limit reached 0: drop packet\n");
        gnrc_icmpv6_error_time_exc_send(ICMPV6_ERROR_TIME_EXC_HL, pkt);
        gnrc_pktbuf_release_error(pkt, ETIMEDOUT);
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
    void *err_ptr = NULL;

    ipv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    assert(ipv6 != NULL);
    hdr = ipv6->data;

    /* check seg_left early to avoid duplicating the packet */
    if (ext->seg_left == 0) {
        DEBUG("Last destination.\n");
        /* restore the destination address to the original global form */
        memcpy(&hdr->dst, &hdr->src, 8);
        return res;
    }
    switch (ext->type) {
#ifdef MODULE_GNRC_RPL_SR
    case IPV6_EXT_RH_TYPE_RPL_SRH:
        res = gnrc_rpl_srh_process(hdr, (gnrc_rpl_srh_t *)ext, &err_ptr);
        break;
#endif
    default:
        res = GNRC_IPV6_EXT_RH_ERROR;
        err_ptr = &ext->type;
        break;
    }
    switch (res) {
    case GNRC_IPV6_EXT_RH_FORWARDED:
        _forward_pkt(pkt, hdr);
        break;
    case GNRC_IPV6_EXT_RH_AT_DST:
        break;
    default:
#ifdef MODULE_GNRC_ICMPV6_ERROR
        if (err_ptr) {
            gnrc_icmpv6_error_param_prob_send(
                ICMPV6_ERROR_PARAM_PROB_HDR_FIELD,
                err_ptr, pkt
                );
        }
#else
        (void)err_ptr;
#endif
        gnrc_pktbuf_release_error(pkt, EINVAL);
        break;
    }
    return res;
}

/** @} */
