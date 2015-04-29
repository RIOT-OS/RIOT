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
#include "net/ng_ndp.h"
#include "od.h"
#include "utlist.h"

#include "net/ng_icmpv6.h"
#include "net/ng_icmpv6/echo.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static inline uint16_t _icmpv6_inet_csum(ng_ipv6_hdr_t *ipv6,
                                         ng_icmpv6_hdr_t *icmpv6, uint16_t len)
{

    uint16_t csum = 0;

    csum = ng_ipv6_hdr_inet_csum(csum, ipv6, NG_PROTNUM_ICMPV6, len);
    csum = ng_inet_csum(csum, (uint8_t *)icmpv6, len);

    return (csum == 0) ? 0 : ~csum;
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

    if (_icmpv6_inet_csum((ng_ipv6_hdr_t *)(ipv6->data), hdr, icmpv6->size)) {
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

        case NG_ICMPV6_RTR_SOL:
        case NG_ICMPV6_RTR_ADV:
        case NG_ICMPV6_NBR_SOL:
        case NG_ICMPV6_NBR_ADV:
        case NG_ICMPV6_REDIRECT:
            DEBUG("icmpv6: neighbor discovery message received\n");
            ng_ndp_demux(iface, pkt, ipv6->data, hdr, icmpv6->size);
            break;

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

    /* ICMPv6-all will be send send in ng_ipv6.c so only dispatch of
     * subtypes is needed */

    sendto = ng_netreg_lookup(NG_NETTYPE_ICMPV6, hdr->type);

    if (sendto == NULL) {
        DEBUG("icmpv6: no receivers for ICMPv6 type %" PRIu8 "\n", hdr->type);
        /* don't release: IPv6 does this */
        return;
    }

    ng_pktbuf_hold(pkt, ng_netreg_num(NG_NETTYPE_ICMPV6, hdr->type) - 1);
    /* ICMPv6 is not interested anymore so `- 1` */

    while (sendto != NULL) {
        ng_netapi_receive(sendto->pid, pkt);
        sendto = ng_netreg_getnext(sendto);
    }
}

int ng_icmpv6_calc_csum(ng_pktsnip_t *hdr, ng_pktsnip_t *pseudo_hdr)
{
    ng_icmpv6_hdr_t *icmpv6_hdr;
    uint32_t csum = 0;

    if (pseudo_hdr->type != NG_NETTYPE_IPV6) {
        DEBUG("icmpv6: Pseudo header type was not IPv6\n");
        return -ENOENT;
    }

    if (hdr->type != NG_NETTYPE_ICMPV6) {
        DEBUG("icmpv6: Header type was not ICMPv6\n");
        return -EINVAL;
    }

    icmpv6_hdr = (ng_icmpv6_hdr_t *)hdr->data;
    icmpv6_hdr->csum = byteorder_htons(0);

    /* calculate checksum for IPv6 pseudo header */
    csum = ng_ipv6_hdr_inet_csum(csum, pseudo_hdr->data, NG_PROTNUM_ICMPV6,
                                 ng_pkt_len(hdr));

    for (; hdr != NULL; hdr = hdr->next) {
        /* calculate checksum for ICMPv6 packet and its payload */
        csum = ng_inet_csum(csum, hdr->data, hdr->size);
    }

    icmpv6_hdr->csum = byteorder_htons((csum == 0) ? 0 : ~csum);

    return 0;
}

/**
 * @}
 */
