/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     net_gnrc_icmpv6
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc.h"
#include "net/gnrc/ndp.h"
#include "net/protnum.h"
#include "od.h"
#include "utlist.h"

#include "net/gnrc/icmpv6.h"
#include "net/gnrc/icmpv6/echo.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static inline uint16_t _calc_csum(gnrc_pktsnip_t *hdr,
                                  gnrc_pktsnip_t *pseudo_hdr,
                                  gnrc_pktsnip_t *payload)
{
    uint16_t csum = 0;
    uint16_t len = (uint16_t)hdr->size;

    while (payload && (payload != hdr)) {
        csum = inet_csum(csum, payload->data, payload->size);
        len += (uint16_t)payload->size;
        payload = payload->next;
    }

    csum = inet_csum(csum, hdr->data, hdr->size);
    csum = ipv6_hdr_inet_csum(csum, pseudo_hdr->data, PROTNUM_ICMPV6, len);

    return ~csum;
}

void gnrc_icmpv6_demux(kernel_pid_t iface, gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *icmpv6, *ipv6;
    icmpv6_hdr_t *hdr;
    gnrc_netreg_entry_t *sendto;

    LL_SEARCH_SCALAR(pkt, icmpv6, type, GNRC_NETTYPE_ICMPV6);

    assert(icmpv6 != NULL);

    /* there can be extension headers between IPv6 and ICMPv6 header so we have
     * to search it */
    LL_SEARCH_SCALAR(icmpv6, ipv6, type, GNRC_NETTYPE_IPV6);

    assert(ipv6 != NULL);

    hdr = (icmpv6_hdr_t *)icmpv6->data;

    if (_calc_csum(icmpv6, ipv6, pkt)) {
        DEBUG("icmpv6: wrong checksum.\n");
        /* don't release: IPv6 does this */
        return;
    }

    switch (hdr->type) {
        /* TODO: handle ICMPv6 errors */
#ifdef MODULE_GNRC_ICMPV6_ECHO
        case ICMPV6_ECHO_REQ:
            DEBUG("icmpv6: handle echo request.\n");
            gnrc_icmpv6_echo_req_handle(iface, (ipv6_hdr_t *)ipv6->data,
                                        (icmpv6_echo_t *)hdr, icmpv6->size);
            break;
#endif

#if (defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_ROUTER))
        case ICMPV6_RTR_SOL:
            DEBUG("icmpv6: router solicitation received\n");
            gnrc_ndp_rtr_sol_handle(iface, pkt, ipv6->data, (ndp_rtr_sol_t *)hdr,
                                    icmpv6->size);
            break;
#endif

#ifdef MODULE_GNRC_NDP
        case ICMPV6_RTR_ADV:
            DEBUG("icmpv6: router advertisement received\n");
            gnrc_ndp_rtr_adv_handle(iface, pkt, ipv6->data, (ndp_rtr_adv_t *)hdr,
                                    icmpv6->size);
            break;

        case ICMPV6_NBR_SOL:
            DEBUG("icmpv6: neighbor solicitation received\n");
            gnrc_ndp_nbr_sol_handle(iface, pkt, ipv6->data, (ndp_nbr_sol_t *)hdr,
                                    icmpv6->size);
            break;

        case ICMPV6_NBR_ADV:
            DEBUG("icmpv6: neighbor advertisement received\n");
            gnrc_ndp_nbr_adv_handle(iface, pkt, ipv6->data, (ndp_nbr_adv_t *)hdr,
                                    icmpv6->size);
            break;
#endif

        case ICMPV6_REDIRECT:
            DEBUG("icmpv6: redirect message received\n");
            /* TODO */
            break;

        default:
            DEBUG("icmpv6: unknown type field %" PRIu8 "\n", hdr->type);
            break;
    }

    /* ICMPv6-all will be send in gnrc_ipv6.c so only dispatch of subtypes is
     * needed */

    sendto = gnrc_netreg_lookup(GNRC_NETTYPE_ICMPV6, hdr->type);

    if (sendto == NULL) {
        DEBUG("icmpv6: no receivers for ICMPv6 type %" PRIu8 "\n", hdr->type);
        /* don't release: IPv6 does this */
        return;
    }

    /* IPv6 might still do stuff to the packet, so no `- 1` */
    gnrc_pktbuf_hold(pkt, gnrc_netreg_num(GNRC_NETTYPE_ICMPV6, hdr->type));

    while (sendto != NULL) {
        if (gnrc_netapi_receive(sendto->pid, pkt) < 1) {
            DEBUG("icmpv6: unable to deliver packet\n");
            gnrc_pktbuf_release(pkt);
        }
        sendto = gnrc_netreg_getnext(sendto);
    }
}

gnrc_pktsnip_t *gnrc_icmpv6_build(gnrc_pktsnip_t *next, uint8_t type, uint8_t code,
                                  size_t size)
{
    gnrc_pktsnip_t *pkt;
    icmpv6_hdr_t *icmpv6;

    pkt = gnrc_pktbuf_add(next, NULL, size, GNRC_NETTYPE_ICMPV6);

    if (pkt == NULL) {
        DEBUG("icmpv6_echo: no space left in packet buffer\n");
        return NULL;
    }

    DEBUG("icmpv6: Building ICMPv6 message with type=%" PRIu8 ", code=%" PRIu8 "\n",
          type, code);
    icmpv6 = (icmpv6_hdr_t *)pkt->data;
    icmpv6->type = type;
    icmpv6->code = code;
    icmpv6->csum.u16 = 0;

    return pkt;
}

int gnrc_icmpv6_calc_csum(gnrc_pktsnip_t *hdr, gnrc_pktsnip_t *pseudo_hdr)
{
    uint32_t csum = 0;

    if (hdr == NULL) {
        return -EFAULT;
    }
    if (hdr->type != GNRC_NETTYPE_ICMPV6) {
        return -EBADMSG;
    }

    csum = _calc_csum(hdr, pseudo_hdr, hdr->next);

    if (csum == 0) {
        return -ENOENT;
    }

    ((icmpv6_hdr_t *)hdr->data)->csum = byteorder_htons(csum);

    return 0;
}

/**
 * @}
 */
