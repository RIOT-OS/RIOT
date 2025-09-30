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

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

#include "byteorder.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/protnum.h"
#include "od.h"
#include "utlist.h"

#include "net/gnrc/icmpv6.h"
#include "net/gnrc/icmpv6/echo.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static inline uint16_t _calc_csum(gnrc_pktsnip_t *hdr,
                                  gnrc_pktsnip_t *pseudo_hdr,
                                  gnrc_pktsnip_t *payload)
{
    uint16_t csum = 0;
    uint16_t len = (uint16_t)hdr->size;

    while (payload && (payload != hdr)) {
        csum = inet_csum_slice(csum, payload->data, payload->size, len);
        len += (uint16_t)payload->size;
        payload = payload->next;
    }

    csum = inet_csum(csum, hdr->data, hdr->size);
    csum = ipv6_hdr_inet_csum(csum, pseudo_hdr->data,
                            gnrc_nettype_to_protnum(pseudo_hdr->next->type), len);

    return ~csum;
}

void gnrc_icmpv6_demux(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *icmpv6, *ipv6;
    icmpv6_hdr_t *hdr;

    icmpv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_ICMPV6);

    assert(icmpv6 != NULL);

    /* there can be extension headers between IPv6 and ICMPv6 header so we have
     * to search it */
    ipv6 = gnrc_pktsnip_search_type(icmpv6, GNRC_NETTYPE_IPV6);

    assert(ipv6 != NULL);

    if (icmpv6->size < sizeof(icmpv6_hdr_t)) {
        DEBUG("icmpv6: packet too short.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* Note: size will be checked again in packet handlers */

    hdr = (icmpv6_hdr_t *)icmpv6->data;
    if (pkt != icmpv6 && _calc_csum(icmpv6, ipv6, pkt)) {
        DEBUG("icmpv6: wrong checksum.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    switch (hdr->type) {
        /* TODO: handle ICMPv6 errors */
#ifdef MODULE_GNRC_ICMPV6_ECHO
        case ICMPV6_ECHO_REQ:
            DEBUG("icmpv6: handle echo request.\n");
            gnrc_icmpv6_echo_req_handle(netif, (ipv6_hdr_t *)ipv6->data,
                                        (icmpv6_echo_t *)hdr, icmpv6->size);
            break;
#endif

        case ICMPV6_RTR_SOL:
        case ICMPV6_RTR_ADV:
        case ICMPV6_NBR_SOL:
        case ICMPV6_NBR_ADV:
        case ICMPV6_REDIRECT:
        case ICMPV6_DAR:
        case ICMPV6_DAC:
            DEBUG("icmpv6: NDP message received. Handle with gnrc_ipv6_nib\n");
            gnrc_ipv6_nib_handle_pkt(netif, ipv6->data, hdr, icmpv6->size);
            break;

        default:
            DEBUG("icmpv6: unknown type field %u\n", hdr->type);
            (void)netif;
            break;
    }

    /* ICMPv6-all will be send in gnrc_ipv6.c so only dispatch of subtypes is
     * needed */
    if (!gnrc_netapi_dispatch_receive(GNRC_NETTYPE_ICMPV6, hdr->type, pkt)) {
        DEBUG("icmpv6: no one interested in type %d\n", hdr->type);
        gnrc_pktbuf_release(pkt);
    }
}

gnrc_pktsnip_t *gnrc_icmpv6_build(gnrc_pktsnip_t *next, uint8_t type,
                                  uint8_t code, size_t size)
{
    gnrc_pktsnip_t *pkt;
    icmpv6_hdr_t *icmpv6;

    if ((pkt = gnrc_pktbuf_add(next, NULL, size, GNRC_NETTYPE_ICMPV6)) == NULL) {
        DEBUG("icmpv6: no space left in packet buffer\n");
        return NULL;
    }

    DEBUG("icmpv6: Building ICMPv6 message with type=%u, code=%u\n",
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
