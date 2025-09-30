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

#include <inttypes.h>

#include "net/gnrc.h"

#include "od.h"
#include "net/gnrc/icmpv6.h"
#include "net/gnrc/icmpv6/echo.h"
#include "net/gnrc/ipv6/hdr.h"
#include "utlist.h"

#define ENABLE_DEBUG 0
#include "debug.h"

gnrc_pktsnip_t *gnrc_icmpv6_echo_build(uint8_t type, uint16_t id, uint16_t seq,
                                       uint8_t *data, size_t data_len)
{
    gnrc_pktsnip_t *pkt;
    icmpv6_echo_t *echo;

    if ((pkt = gnrc_icmpv6_build(NULL, type, 0, data_len + sizeof(icmpv6_echo_t))) == NULL) {
        return NULL;
    }

    DEBUG("icmpv6_echo: Building echo message with type=%" PRIu8 " id=%" PRIu16
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
    ((ipv6_hdr_t *)hdr->data)->nh = PROTNUM_ICMPV6;

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

    pkt = gnrc_pkt_prepend(pkt, hdr);

    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL,
                                   pkt)) {
        DEBUG("icmpv6_echo: no receivers for IPv6 packets\n");
        gnrc_pktbuf_release(pkt);
    }
}

static void _fill_payload(uint8_t *buf, size_t len, uint32_t now)
{
    uint8_t i = 0;

    if (len >= sizeof(uint32_t)) {
        memcpy(buf, &now, sizeof(now));
        len -= sizeof(now);
        buf += sizeof(now);
    }

    while (len--) {
        *buf++ = i++;
    }
}

static void _check_payload(const void *buf, size_t len, uint32_t now,
                           uint32_t *triptime, int *corrupt)
{
    uint8_t i = 0;
    const uint8_t *data = buf;

    if (len >= sizeof(uint32_t)) {
        *triptime = now - unaligned_get_u32(buf);
        len  -= sizeof(uint32_t);
        data += sizeof(uint32_t);
    }

    while (len--) {
        if (*data++ != i++) {
            *corrupt = data - (uint8_t *)buf - 1;
            break;
        }
    }
}

int gnrc_icmpv6_echo_send(const gnrc_netif_t *netif, const ipv6_addr_t *addr,
                          uint16_t id, uint16_t seq, uint8_t ttl, size_t len)
{
    int res = 0;
    gnrc_pktsnip_t *pkt, *tmp;
    ipv6_hdr_t *ipv6;
    uint8_t *databuf;

    /* max IPv6 payload 65535 minus 8 bytes of icmp header = 65527 */
    if (len > (UINT16_MAX - sizeof(icmpv6_hdr_t))) {
        DEBUG("error: wrong icmpv6 packet length\n");
        return -EINVAL;
    }

    pkt = gnrc_icmpv6_echo_build(ICMPV6_ECHO_REQ, id, seq, NULL, len);
    if (pkt == NULL) {
        DEBUG("error: packet buffer full\n");
        return -ENOMEM;
    }

    databuf = (uint8_t *)(pkt->data) + sizeof(icmpv6_echo_t);
    tmp = gnrc_ipv6_hdr_build(pkt, NULL, addr);
    if (tmp == NULL) {
        puts("error: packet buffer full");
        goto error_exit;
    }
    pkt = tmp;
    ipv6 = pkt->data;
    ipv6->nh = PROTNUM_ICMPV6;
    /* if ttl is unset (i.e. 0) gnrc_ipv6 will select hop limit */
    ipv6->hl = ttl;
    if (netif != NULL) {
        tmp = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
        if (tmp == NULL) {
            DEBUG("error: packet buffer full");
            res = -ENOMEM;
            goto error_exit;
        }
        gnrc_netif_hdr_set_netif(tmp->data, netif);
        pkt = gnrc_pkt_prepend(pkt, tmp);
    }

    /* add TX timestamp & test data */
    _fill_payload(databuf, len, ztimer_now(ZTIMER_USEC));

    res = !gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6,
                                     GNRC_NETREG_DEMUX_CTX_ALL,
                                     pkt);
    if (res) {
        DEBUG("error: unable to send ICMPv6 echo request\n");
        res = -EBADF;
    }

error_exit:
    if (res) {
        gnrc_pktbuf_release(pkt);
    }
    return res;
}

int gnrc_icmpv6_echo_rsp_handle(gnrc_pktsnip_t *pkt, size_t len,
                                gnrc_icmpv6_echo_rsp_handle_cb_t cb, void *ctx)
{
    gnrc_pktsnip_t *ipv6, *icmpv6;
    ipv6_hdr_t *ipv6_hdr;
    uint32_t now = ztimer_now(ZTIMER_USEC);
    uint32_t triptime = 0;
    int corrupted = -1;

    ipv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    icmpv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_ICMPV6);
    if ((ipv6 == NULL) || (icmpv6 == NULL)) {
        DEBUG("No IPv6 or ICMPv6 header found in reply");
        return -EINVAL;
    }
    ipv6_hdr = ipv6->data;
#ifdef MODULE_GNRC_IPV6_NIB
    /* successful ping to neighbor (NIB handles case if ipv6->src is not a
     * neighbor) can be taken as upper-layer hint for reachability:
     * https://tools.ietf.org/html/rfc4861#section-7.3.1 */
    gnrc_ipv6_nib_nc_mark_reachable(&ipv6_hdr->src);
#endif

    icmpv6_echo_t *icmpv6_hdr = icmpv6->data;
    _check_payload(icmpv6_hdr + 1, len, now, &triptime, &corrupted);

    return cb(pkt, corrupted, triptime, ctx);
}

/** @} */
