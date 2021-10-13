/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <assert.h>
#include <string.h>

#include "net/ethernet/hdr.h"
#include "net/gnrc.h"
#include "net/gnrc/netif/ethernet.h"
#include "net/netdev/eth.h"
#ifdef MODULE_GNRC_IPV6
#include "net/ipv6/hdr.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#if defined(MODULE_OD) && ENABLE_DEBUG
#include "od.h"
#endif

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);
static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif);
#ifdef MODULE_GNRC_SIXLOENC
static int _set(gnrc_netif_t *netif, const gnrc_netapi_opt_t *opt);
#else
#define _set    gnrc_netif_set_from_netdev
#endif /* MODULE_GNRC_SIXLOENC */

static char addr_str[ETHERNET_ADDR_LEN * 3];

static const gnrc_netif_ops_t ethernet_ops = {
    .init = gnrc_netif_default_init,
    .send = _send,
    .recv = _recv,
    .get = gnrc_netif_get_from_netdev,
    .set = _set,
};

int gnrc_netif_ethernet_create(gnrc_netif_t *netif, char *stack, int stacksize,
                               char priority, char *name, netdev_t *dev)
{
    return gnrc_netif_create(netif, stack, stacksize, priority, name, dev,
                             &ethernet_ops);
}

static inline void _addr_set_broadcast(uint8_t *dst)
{
    memset(dst, 0xff, ETHERNET_ADDR_LEN);
}

static inline void _addr_set_multicast(gnrc_netif_t *netif, uint8_t *dst,
                                       gnrc_pktsnip_t *payload)
{
    switch (payload->type) {
#ifdef MODULE_GNRC_IPV6
        case GNRC_NETTYPE_IPV6: {
            ipv6_hdr_t *ipv6 = payload->data;
            gnrc_netif_ipv6_group_to_l2_group(netif, &ipv6->dst, dst);
            break;
        }
#endif
        default:
            (void)netif;
            _addr_set_broadcast(dst);
            break;
    }
}

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    ethernet_hdr_t hdr;
    gnrc_netif_hdr_t *netif_hdr;
    gnrc_pktsnip_t *payload;
    int res;

    netdev_t *dev = netif->dev;

    if (pkt == NULL) {
        DEBUG("gnrc_netif_ethernet: pkt was NULL\n");
        return -EINVAL;
    }

    payload = pkt->next;

    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("gnrc_netif_ethernet: First header was not generic netif header\n");
        return -EBADMSG;
    }

    if (payload) {
        hdr.type = byteorder_htons(gnrc_nettype_to_ethertype(payload->type));
    }
    else {
        hdr.type = byteorder_htons(ETHERTYPE_UNKNOWN);
    }

    netif_hdr = pkt->data;

    /* set ethernet header */
    if (netif_hdr->src_l2addr_len == ETHERNET_ADDR_LEN) {
        memcpy(hdr.dst, gnrc_netif_hdr_get_src_addr(netif_hdr),
               netif_hdr->src_l2addr_len);
    }
    else {
        dev->driver->get(dev, NETOPT_ADDRESS, hdr.src, ETHERNET_ADDR_LEN);
    }

    if (netif_hdr->flags & GNRC_NETIF_HDR_FLAGS_BROADCAST) {
        _addr_set_broadcast(hdr.dst);
    }
    else if (netif_hdr->flags & GNRC_NETIF_HDR_FLAGS_MULTICAST) {
        if (payload == NULL) {
            DEBUG("gnrc_netif_ethernet: empty multicast packets over Ethernet "
                  "are not yet supported\n");
            return -ENOTSUP;
        }
        _addr_set_multicast(netif, hdr.dst, payload);
    }
    else if (netif_hdr->dst_l2addr_len == ETHERNET_ADDR_LEN) {
        memcpy(hdr.dst, gnrc_netif_hdr_get_dst_addr(netif_hdr),
               ETHERNET_ADDR_LEN);
    }
    else {
        DEBUG("gnrc_netif_ethernet: destination address had unexpected "
              "format\n");
        return -EBADMSG;
    }

    DEBUG("gnrc_netif_ethernet: send to %02x:%02x:%02x:%02x:%02x:%02x\n",
          hdr.dst[0], hdr.dst[1], hdr.dst[2],
          hdr.dst[3], hdr.dst[4], hdr.dst[5]);

    iolist_t iolist = {
        .iol_next = (iolist_t *)payload,
        .iol_base = &hdr,
        .iol_len = sizeof(ethernet_hdr_t)
    };

#ifdef MODULE_NETSTATS_L2
    if ((netif_hdr->flags & GNRC_NETIF_HDR_FLAGS_BROADCAST) ||
        (netif_hdr->flags & GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        netif->stats.tx_mcast_count++;
    }
    else {
        netif->stats.tx_unicast_count++;
    }
#endif
    res = dev->driver->send(dev, &iolist);

    gnrc_pktbuf_release(pkt);

    return res;
}

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    gnrc_pktsnip_t *pkt = NULL;
    netdev_eth_rx_info_t rx_info = { .flags = 0 };
    int bytes_expected = dev->driver->recv(dev, NULL, 0, NULL);

    if (bytes_expected > 0) {
        pkt = gnrc_pktbuf_add(NULL, NULL,
                              bytes_expected,
                              GNRC_NETTYPE_UNDEF);

        if (!pkt) {
            DEBUG("gnrc_netif_ethernet: cannot allocate pktsnip.\n");

            /* drop the packet */
            dev->driver->recv(dev, NULL, bytes_expected, NULL);

            goto out;
        }

        int nread = dev->driver->recv(dev, pkt->data, bytes_expected, &rx_info);
        if (nread <= 0) {
            DEBUG("gnrc_netif_ethernet: read error.\n");
            goto safe_out;
        }
#ifdef MODULE_NETSTATS_L2
        netif->stats.rx_count++;
        netif->stats.rx_bytes += nread;
#endif

        if (nread < bytes_expected) {
            /* we've got less than the expected packet size,
             * so free the unused space.*/

            DEBUG("gnrc_netif_ethernet: reallocating.\n");
            gnrc_pktbuf_realloc_data(pkt, nread);
        }

        DEBUG("gnrc_netif_ethernet: received packet from %s of length %d\n",
              gnrc_netif_addr_to_str(pkt->data, ETHERNET_ADDR_LEN, addr_str),
              nread);
#if defined(MODULE_OD) && ENABLE_DEBUG
        od_hex_dump(pkt->data, nread, OD_WIDTH_DEFAULT);
#endif
        /* mark ethernet header */
        gnrc_pktsnip_t *eth_hdr = gnrc_pktbuf_mark(pkt, sizeof(ethernet_hdr_t), GNRC_NETTYPE_UNDEF);
        if (!eth_hdr) {
            DEBUG("gnrc_netif_ethernet: no space left in packet buffer\n");
            goto safe_out;
        }

        ethernet_hdr_t *hdr = (ethernet_hdr_t *)eth_hdr->data;

#ifdef MODULE_L2FILTER
        if (!l2filter_pass(dev->filter, hdr->src, ETHERNET_ADDR_LEN)) {
            DEBUG("gnrc_netif_ethernet: incoming packet filtered by l2filter\n");
            goto safe_out;
        }
#endif

        /* set payload type from ethertype */
        pkt->type = gnrc_nettype_from_ethertype(byteorder_ntohs(hdr->type));

        /* create netif header */
        gnrc_pktsnip_t *netif_hdr;
        netif_hdr = gnrc_pktbuf_add(NULL, NULL,
                                    sizeof(gnrc_netif_hdr_t) + (2 * ETHERNET_ADDR_LEN),
                                    GNRC_NETTYPE_NETIF);

        if (netif_hdr == NULL) {
            DEBUG("gnrc_netif_ethernet: no space left in packet buffer\n");
            pkt = eth_hdr;
            goto safe_out;
        }

        gnrc_netif_hdr_init(netif_hdr->data, ETHERNET_ADDR_LEN, ETHERNET_ADDR_LEN);
        gnrc_netif_hdr_set_src_addr(netif_hdr->data, hdr->src, ETHERNET_ADDR_LEN);
        gnrc_netif_hdr_set_dst_addr(netif_hdr->data, hdr->dst, ETHERNET_ADDR_LEN);
        gnrc_netif_hdr_set_netif(netif_hdr->data, netif);
        if (rx_info.flags & NETDEV_ETH_RX_INFO_FLAG_TIMESTAMP) {
            gnrc_netif_hdr_set_timestamp(netif_hdr->data, rx_info.timestamp);
        }

        gnrc_pktbuf_remove_snip(pkt, eth_hdr);
        pkt = gnrc_pkt_append(pkt, netif_hdr);
    }

out:
    return pkt;

safe_out:
    gnrc_pktbuf_release(pkt);
    return NULL;
}

#ifdef MODULE_GNRC_SIXLOENC
static int _set(gnrc_netif_t *netif, const gnrc_netapi_opt_t *opt)
{
    if (opt->opt == NETOPT_6LO) {
        assert(opt->data_len == sizeof(netopt_enable_t));
        if (*((netopt_enable_t *)opt->data) == NETOPT_ENABLE) {
            netif->flags |= (GNRC_NETIF_FLAGS_6LO | GNRC_NETIF_FLAGS_6LO_HC |
                             GNRC_NETIF_FLAGS_6LN);
        }
        else {
            netif->flags &= ~(GNRC_NETIF_FLAGS_6LO | GNRC_NETIF_FLAGS_6LO_HC |
                              GNRC_NETIF_FLAGS_6LN);
        }
        return sizeof(netopt_enable_t);
    }
    return gnrc_netif_set_from_netdev(netif, opt);
}
#endif /* MODULE_GNRC_SIXLOENC */
/** @} */
