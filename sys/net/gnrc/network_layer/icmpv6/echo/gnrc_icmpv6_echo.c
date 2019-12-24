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

#include "net/gnrc.h"

#include "od.h"
#include "net/gnrc/icmpv6.h"
#include "net/gnrc/icmpv6/echo.h"
#include "net/gnrc/ipv6/hdr.h"
#include "utlist.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

gnrc_pktsnip_t *gnrc_icmpv6_echo_build(uint8_t type, uint16_t id, uint16_t seq,
                                       uint8_t *data, size_t data_len)
{
    gnrc_pktsnip_t *pkt;
    icmpv6_echo_t *echo;

    if ((pkt = gnrc_icmpv6_build(NULL, type, 0, data_len + sizeof(icmpv6_echo_t))) == NULL) {
        return NULL;
    }

    DEBUG("icmpv6_echo: Building echo message with type=%" PRIu8 "id=%" PRIu16
          ", seq=%" PRIu16, type, id, seq);
    echo = (icmpv6_echo_t *)pkt->data;
    echo->id = byteorder_htons(id);
    echo->seq = byteorder_htons(seq);

    if (data != NULL) {
        memcpy(echo + 1, data, data_len);
#if defined(MODULE_OD) && ENABLE_DEBUG
        DEBUG(", payload:\n");
        od_hex_dump(data, data_len, OD_WIDTH_DEFAULT);
#endif
    }
    DEBUG("\n");

    return pkt;
}

void gnrc_icmpv6_echo_req_handle(gnrc_netif_t *netif, ipv6_hdr_t *ipv6_hdr,
                                 icmpv6_echo_t *echo, uint16_t len)
{
    uint8_t *payload = ((uint8_t *)echo) + sizeof(icmpv6_echo_t);
    gnrc_pktsnip_t *hdr, *pkt;

    if ((echo == NULL) || (len < sizeof(icmpv6_echo_t))) {
        DEBUG("icmpv6_echo: echo was NULL or len (%" PRIu16
              ") was < sizeof(icmpv6_echo_t)\n", len);
        return;
    }

    pkt = gnrc_icmpv6_echo_build(ICMPV6_ECHO_REP, byteorder_ntohs(echo->id),
                                 byteorder_ntohs(echo->seq), payload,
                                 len - sizeof(icmpv6_echo_t));

    if (pkt == NULL) {
        DEBUG("icmpv6_echo: no space left in packet buffer\n");
        return;
    }

    if (ipv6_addr_is_multicast(&ipv6_hdr->dst)) {
        hdr = gnrc_ipv6_hdr_build(pkt, NULL, &ipv6_hdr->src);
    }
    else {
        hdr = gnrc_ipv6_hdr_build(pkt, &ipv6_hdr->dst, &ipv6_hdr->src);
    }

    if (hdr == NULL) {
        DEBUG("icmpv6_echo: no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    pkt = hdr;
    hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);

    if (hdr == NULL) {
        DEBUG("icmpv6_echo: no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    /* (netif == NULL) => ipv6_hdr->dst is loopback address */
    gnrc_netif_hdr_set_netif(hdr->data, netif);

    LL_PREPEND(pkt, hdr);

    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL,
                                   pkt)) {
        DEBUG("icmpv6_echo: no receivers for IPv6 packets\n");
        gnrc_pktbuf_release(pkt);
    }
}

/** @} */
