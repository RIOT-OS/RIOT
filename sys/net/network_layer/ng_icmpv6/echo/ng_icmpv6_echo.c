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

#include "log.h"
#include "net/ng_netbase.h"

#include "od.h"
#include "net/ng_icmpv6.h"
#include "net/ng_icmpv6/echo.h"
#include "utlist.h"

#if (LOG_LEVEL >= LOG_LEVEL_KERNEL)
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

ng_pktsnip_t *ng_icmpv6_echo_build(uint8_t type, uint16_t id, uint16_t seq,
                                   uint8_t *data, size_t data_len)
{
    ng_pktsnip_t *pkt;
    ng_icmpv6_echo_t *echo;

    if ((pkt = ng_icmpv6_build(NULL, type, 0, data_len + sizeof(ng_icmpv6_echo_t))) == NULL) {
        return NULL;
    }

    LOG_DEBUG("icmpv6_echo: Building echo message with type=%" PRIu8 "id=%" PRIu16
              ", seq=%" PRIu16, type, id, seq);
    echo = (ng_icmpv6_echo_t *)pkt->data;
    echo->id = byteorder_htons(id);
    echo->seq = byteorder_htons(seq);

    if (data != NULL) {
        memcpy(echo + 1, data, data_len);
#if defined(MODULE_OD) && (LOG_LEVEL >= LOG_DEBUG)
        LOG_DEBUG(", payload:\n");
        od_hex_dump(data, data_len, OD_WIDTH_DEFAULT);
#endif
        LOG_DEBUG("\n");
    }
#if (LOG_LEVEL >= LOG_DEBUG)
    else {
        LOG_DEBUG("\n");
    }
#endif

    return pkt;
}

void ng_icmpv6_echo_req_handle(kernel_pid_t iface, ng_ipv6_hdr_t *ipv6_hdr,
                               ng_icmpv6_echo_t *echo, uint16_t len)
{
    uint8_t *payload = ((uint8_t *)echo) + sizeof(ng_icmpv6_echo_t);
    ng_pktsnip_t *hdr, *pkt;
    ng_netreg_entry_t *sendto = NULL;

    if ((echo == NULL) || (len < sizeof(ng_icmpv6_echo_t))) {
        LOG_WARNING("icmpv6_echo: echo was NULL or len (%" PRIu16
              ") was < sizeof(ng_icmpv6_echo_t)\n", len);
        return;
    }

    pkt = ng_icmpv6_echo_build(NG_ICMPV6_ECHO_REP, byteorder_ntohs(echo->id),
                               byteorder_ntohs(echo->seq), payload,
                               len - sizeof(ng_icmpv6_echo_t));

    if (pkt == NULL) {
        LOG_ERROR("icmpv6_echo: no space left in packet buffer\n");
        return;
    }

    if (ng_ipv6_addr_is_multicast(&ipv6_hdr->dst)) {
        hdr = ng_ipv6_hdr_build(pkt, NULL, 0, (uint8_t *)&ipv6_hdr->src,
                                sizeof(ng_ipv6_addr_t));
    }
    else {
        hdr = ng_ipv6_hdr_build(pkt, (uint8_t *)&ipv6_hdr->dst,
                                sizeof(ng_ipv6_addr_t), (uint8_t *)&ipv6_hdr->src,
                                sizeof(ng_ipv6_addr_t));
    }

    if (hdr == NULL) {
        LOG_ERROR("icmpv6_echo: no space left in packet buffer\n");
        ng_pktbuf_release(pkt);
        return;
    }

    pkt = hdr;
    hdr = ng_netif_hdr_build(NULL, 0, NULL, 0);

    ((ng_netif_hdr_t *)hdr->data)->if_pid = iface;

    LL_PREPEND(pkt, hdr);

    sendto = ng_netreg_lookup(NG_NETTYPE_IPV6, NG_NETREG_DEMUX_CTX_ALL);

    if (sendto == NULL) {
        LOG_WARNING("icmpv6_echo: no receivers for IPv6 packets\n");
        ng_pktbuf_release(pkt);
        return;
    }

    /* ICMPv6 is not interested anymore so `- 1` */
    ng_pktbuf_hold(pkt, ng_netreg_num(NG_NETTYPE_IPV6, NG_NETREG_DEMUX_CTX_ALL) - 1);

    while (sendto != NULL) {
        ng_netapi_send(sendto->pid, pkt);
        sendto = ng_netreg_getnext(sendto);
    }
}

/** @} */
