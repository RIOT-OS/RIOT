/*
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
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "net/gnrc.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/netdev/ieee802154.h"

#ifdef MODULE_GNRC_IPV6
#include "net/ipv6/hdr.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#if defined(MODULE_OD) && ENABLE_DEBUG
#include "od.h"
#endif

#ifdef MODULE_NETDEV_IEEE802154
static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);
static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif);

static const gnrc_netif_ops_t ieee802154_ops = {
    .send = _send,
    .recv = _recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
};

gnrc_netif_t *gnrc_netif_ieee802154_create(char *stack, int stacksize,
                                           char priority, char *name,
                                           netdev_t *dev)
{
    return gnrc_netif_create(stack, stacksize, priority, name, dev,
                             &ieee802154_ops);
}

static gnrc_pktsnip_t *_make_netif_hdr(netdev_ieee802154_data_hdr_t *l2data_hdr)
{
    gnrc_pktsnip_t *snip;
    /* allocate space for header */
    snip = gnrc_netif_hdr_build(l2data_hdr->src, (size_t)l2data_hdr->src_len,
            l2data_hdr->dst, (size_t)l2data_hdr->dst_len);
    if (snip == NULL) {
        DEBUG("_make_netif_hdr: no space left in packet buffer\n");
        return NULL;
    }
    /* set broadcast flag for broadcast destination */
    if ((l2data_hdr->dst_len == 2) && (l2data_hdr->dst[0] == 0xff) &&
            (l2data_hdr->dst[1] == 0xff)) {
        gnrc_netif_hdr_t *hdr = snip->data;
        hdr->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    }
    return snip;
}

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    netdev_ieee802154_rx_info_t rx_info;
    gnrc_pktsnip_t *pkt = NULL;
    int bytes_expected = netdev_ieee802154_recv(dev, NULL, 0, NULL);

    if (bytes_expected >= (int)IEEE802154_MIN_FRAME_LEN) {
        int nread;

        pkt = gnrc_pktbuf_add(NULL, NULL, bytes_expected, GNRC_NETTYPE_UNDEF);
        if (pkt == NULL) {
            DEBUG("_recv_ieee802154: cannot allocate pktsnip.\n");
            return NULL;
        }
        nread = netdev_ieee802154_recv(dev, pkt->data, bytes_expected, &rx_info);
        if (nread <= 0) {
            gnrc_pktbuf_release(pkt);
            return NULL;
        }
        netopt_enable_t raw;
        if ((dev->driver->get(dev, NETOPT_RAWMODE, &raw, sizeof(raw)) == sizeof(raw))
                && raw == NETOPT_DISABLE) {
            gnrc_pktsnip_t *ieee802154_hdr, *netif_hdr;
            gnrc_netif_hdr_t *hdr;
#if ENABLE_DEBUG
            char src_str[GNRC_NETIF_HDR_L2ADDR_PRINT_LEN];
#endif
            nread -= sizeof(netdev_ieee802154_data_hdr_t);
            /* mark IEEE 802.15.4 header */
            ieee802154_hdr = gnrc_pktbuf_mark(pkt, sizeof(netdev_ieee802154_data_hdr_t),
                    GNRC_NETTYPE_UNDEF);
            if (ieee802154_hdr == NULL) {
                DEBUG("_recv_ieee802154: no space left in packet buffer\n");
                gnrc_pktbuf_release(pkt);
                return NULL;
            }
            netif_hdr = _make_netif_hdr(ieee802154_hdr->data);
            if (netif_hdr == NULL) {
                DEBUG("_recv_ieee802154: no space left in packet buffer\n");
                gnrc_pktbuf_release(pkt);
                return NULL;
            }

            hdr = netif_hdr->data;

#ifdef MODULE_L2FILTER
            if (!l2filter_pass(dev->filter, gnrc_netif_hdr_get_src_addr(hdr),
                               hdr->src_l2addr_len)) {
                gnrc_pktbuf_release(pkt);
                gnrc_pktbuf_release(netif_hdr);
                DEBUG("_recv_ieee802154: packet dropped by l2filter\n");
                return NULL;
            }
#endif

            hdr->lqi = rx_info.lqi;
            hdr->rssi = rx_info.rssi;
            hdr->if_pid = thread_getpid();
            dev->driver->get(dev, NETOPT_PROTO, &pkt->type,
                    sizeof(gnrc_nettype_t));
#if ENABLE_DEBUG
            DEBUG("_recv_ieee802154: received packet from %s of length %u\n",
                  gnrc_netif_addr_to_str(gnrc_netif_hdr_get_src_addr(hdr),
                                         hdr->src_l2addr_len,
                                         src_str),
                  nread);
#if defined(MODULE_OD)
            od_hex_dump(pkt->data, nread, OD_WIDTH_DEFAULT);
#endif
#endif
            gnrc_pktbuf_remove_snip(pkt, ieee802154_hdr);
            LL_APPEND(pkt, netif_hdr);
        }

        DEBUG("_recv_ieee802154: reallocating.\n");
        gnrc_pktbuf_realloc_data(pkt, nread);
    } else if (bytes_expected > 0) {
        DEBUG("_recv_ieee802154: received frame is too short\n");
        dev->driver->recv(dev, NULL, bytes_expected, NULL);
    }

    return pkt;
}

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    netdev_t *dev = netif->dev;
    gnrc_netif_hdr_t *netif_hdr;
    int res = 0;
    const uint8_t *dst, *src;
    netdev_ieee802154_data_hdr_t l2data_hdr;

    if (pkt == NULL) {
        DEBUG("_send_ieee802154: pkt was NULL\n");
        return -EINVAL;
    }
    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("_send_ieee802154: first header is not generic netif header\n");
        return -EBADMSG;
    }
    netif_hdr = pkt->data;
    /* prepare destination address */
    if (netif_hdr->flags & /* If any of these flags is set assume broadcast */
        (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        dst = ieee802154_addr_bcast;
        l2data_hdr.dst_len = IEEE802154_ADDR_BCAST_LEN;
    }
    else {
        dst = gnrc_netif_hdr_get_dst_addr(netif_hdr);
        l2data_hdr.dst_len = netif_hdr->dst_l2addr_len;
    }
    memcpy(&l2data_hdr.dst, dst, l2data_hdr.dst_len);

    /* prepare source address */
    uint8_t src_len = netif_hdr->src_l2addr_len;
    if (src_len > 0) {
        src = gnrc_netif_hdr_get_src_addr(netif_hdr);
    }
    else {
        src_len = netif->l2addr_len;
        src = netif->l2addr;
    }
    l2data_hdr.src_len = src_len;
    memcpy(&l2data_hdr.src, src, l2data_hdr.src_len);

#ifdef MODULE_NETSTATS_L2
    if (netif_hdr->flags &
            (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        netif->dev->stats.tx_mcast_count++;
    }
    else {
        netif->dev->stats.tx_unicast_count++;
    }
#endif
    /* prepare iolist for netdev / mac layer */
    iolist_t iolist = {
        .iol_next = (iolist_t *)pkt->next,
        .iol_base = &l2data_hdr,
        .iol_len = sizeof(l2data_hdr)
    };

#ifdef MODULE_GNRC_MAC
    if (netif->mac.mac_info & GNRC_NETIF_MAC_INFO_CSMA_ENABLED) {
        res = csma_sender_csma_ca_send(dev, &iolist, &netif->mac.csma_conf);
    }
    else {
        res = dev->driver->send(dev, &iolist);
    }
#else
    res = netdev_ieee802154_send(dev, &iolist);
#endif

    /* release old data */
    gnrc_pktbuf_release(pkt);
    return res;
}
#else   /* MODULE_NETDEV_IEEE802154 */
typedef int dont_be_pedantic;
#endif  /* MODULE_NETDEV_IEEE802154 */
/** @} */
