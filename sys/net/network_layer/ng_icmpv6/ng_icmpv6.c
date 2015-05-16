/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     net_ng_icmpv6
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
#include "net/ng_netbase.h"
#include "net/ng_protnum.h"
#include "net/ng_ipv6/hdr.h"
#include "od.h"
#include "utlist.h"

#include "net/ng_icmpv6.h"
#include "net/ng_icmpv6/echo.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static inline uint16_t _calc_csum(ng_pktsnip_t *hdr,
                                  ng_pktsnip_t *pseudo_hdr,
                                  ng_pktsnip_t *payload)
{
    uint16_t csum = 0;
    uint16_t len = (uint16_t)hdr->size;

    while (payload && (payload != hdr)) {
        csum = ng_inet_csum(csum, payload->data, payload->size);
        len += (uint16_t)payload->size;
        payload = payload->next;
    }

    csum = ng_inet_csum(csum, hdr->data, hdr->size);
    csum = ng_ipv6_hdr_inet_csum(csum, pseudo_hdr->data, NG_PROTNUM_ICMPV6,
                                 len);

    return ~csum;
}

void ng_icmpv6_demux(kernel_pid_t iface, ng_pktsnip_t *pkt)
{
    ng_pktsnip_t *icmpv6, *ipv6;
    ng_icmpv6_hdr_t *hdr;
    ng_netreg_entry_t *sendto;

    LL_SEARCH_SCALAR(pkt, icmpv6, type, NG_NETTYPE_ICMPV6);

    /* there can be extension headers between IPv6 and ICMPv6 header so we have
     * to search it */
    LL_SEARCH_SCALAR(icmpv6, ipv6, type, NG_NETTYPE_IPV6);

    hdr = (ng_icmpv6_hdr_t *)icmpv6->data;

    if (_calc_csum(icmpv6, ipv6, pkt)) {
        DEBUG("icmpv6: wrong checksum.\n");
        /* don't release: IPv6 does this */
        return;
    }

    switch (hdr->type) {
        /* TODO: handle ICMPv6 errors */
#ifdef MODULE_NG_ICMPV6_ECHO
        case NG_ICMPV6_ECHO_REQ:
            DEBUG("icmpv6: handle echo request.\n");
            ng_icmpv6_echo_req_handle(iface, (ng_ipv6_hdr_t *)ipv6->data,
                                      (ng_icmpv6_echo_t *)hdr, icmpv6->size);
            break;
#endif

#ifdef MODULE_NG_NDP
        case NG_ICMPV6_RTR_SOL:
        case NG_ICMPV6_RTR_ADV:
        case NG_ICMPV6_NBR_SOL:
        case NG_ICMPV6_NBR_ADV:
        case NG_ICMPV6_REDIRECT:
            DEBUG("icmpv6: neighbor discovery message received\n");
            /* TODO */
            break;
#endif

#ifdef MODULE_NG_RPL
        case NG_ICMPV6_RPL_CTRL:
            DEBUG("icmpv6: RPL control message received\n");
            /* TODO */
            break;
#endif

        default:
            DEBUG("icmpv6: unknown type field %" PRIu8 "\n", hdr->type);
            break;
    }

    /* ICMPv6-all will be send in ng_ipv6.c so only dispatch of subtypes is
     * needed */

    sendto = ng_netreg_lookup(NG_NETTYPE_ICMPV6, hdr->type);

    if (sendto == NULL) {
        DEBUG("icmpv6: no receivers for ICMPv6 type %" PRIu8 "\n", hdr->type);
        /* don't release: IPv6 does this */
        return;
    }

    /* ICMPv6 is not interested anymore so `- 1` */
    ng_pktbuf_hold(pkt, ng_netreg_num(NG_NETTYPE_ICMPV6, hdr->type) - 1);

    while (sendto != NULL) {
        ng_netapi_receive(sendto->pid, pkt);
        sendto = ng_netreg_getnext(sendto);
    }
}

ng_pktsnip_t *ng_icmpv6_build(ng_pktsnip_t *next, uint8_t type, uint8_t code,
                              size_t size)
{
    ng_pktsnip_t *pkt;
    ng_icmpv6_hdr_t *icmpv6;

    pkt = ng_pktbuf_add(next, NULL, size, NG_NETTYPE_ICMPV6);

    if (pkt == NULL) {
        DEBUG("icmpv6_echo: no space left in packet buffer\n");
        return NULL;
    }

    DEBUG("icmpv6: Building ICMPv6 message with type=%" PRIu8 ", code=%" PRIu8 "\n",
          type, code);
    icmpv6 = (ng_icmpv6_hdr_t *)pkt->data;
    icmpv6->type = type;
    icmpv6->code = code;
    icmpv6->csum.u16 = 0;

    return pkt;
}

int ng_icmpv6_calc_csum(ng_pktsnip_t *hdr, ng_pktsnip_t *pseudo_hdr)
{
    uint32_t csum = 0;

    if (hdr == NULL) {
        return -EFAULT;
    }
    if (hdr->type != NG_NETTYPE_ICMPV6) {
        return -EBADMSG;
    }

    csum = _calc_csum(hdr, pseudo_hdr, hdr->next);

    if (csum == 0) {
        return -ENOENT;
    }

    ((ng_icmpv6_hdr_t *)hdr->data)->csum = byteorder_htons(csum);

    return 0;
}

/**
 * @}
 */
