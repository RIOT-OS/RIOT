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

#include "net/netbuf.h"

#ifdef MODULE_GNRC_IPV6
#include "net/ipv6/hdr.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#if defined(MODULE_OD) && ENABLE_DEBUG
#include "od.h"
#endif

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif);

static const gnrc_netif_ops_t ieee802154_ops = {
    .init = gnrc_netif_default_init,
    .send = NULL, /* to be migrated to netif->ops */
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

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    ieee802154_rf_rx_done(netif->dev);
    return NULL;
}

/* Implementation of netif_ieee802144 send function.
 * This function sits on top of the IEEE802154 MAC layer.
 * Nothe this functions is (mostly) GNRC agnostic! */
void netif_ieee802154_send(netif_t *netif, netif_pkt_t *pkt)
{
    //const uint8_t *src, *dst = NULL;
    uint8_t *src, *dst = NULL;
    size_t src_len, dst_len;

    /* prepare destination address */
    if (pkt->flags & /* If any of these flags is set assume broadcast */
        (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        dst = (uint8_t*) ieee802154_addr_bcast;
        dst_len = IEEE802154_ADDR_BCAST_LEN;
    }
    else {
        dst = pkt->dst_l2addr;
        dst_len = pkt->dst_l2addr_len;
    }
    src_len = pkt->src_l2addr_len;
    if (src_len > 0) {
        src = pkt->src_l2addr;
    }
    else {
        src_len = ((gnrc_netif_t*) netif)->l2addr_len;
        src = ((gnrc_netif_t*) netif)->l2addr;
    }

#ifdef MODULE_NETSTATS_L2
    if (netif_hdr->flags &
            (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        netif->stats.tx_mcast_count++;
    }
    else {
        netif->stats.tx_unicast_count++;
    }
#endif

    
    gnrc_netif_t *gnrc_netif = (gnrc_netif_t*) netif;
    ieee802154_mac_send((netdev_ieee802154_t*) gnrc_netif->dev, &pkt->msdu, pkt->flags & GNRC_NETIF_HDR_FLAGS_MORE_DATA,
            src, src_len, dst, dst_len);

    /* release old data */
    netbuf_free(pkt->ctx);
}

void netif_send(netif_t *netif, netif_pkt_t *pkt)
{
    /* netif->ops->send(netif, pkt); */
    netif_ieee802154_send(netif, pkt); /* We use this one, since there's no netif->ops yet :( */
}

int gnrc_netif_send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    /* ========== BEGIN GNRC_NETIF_GLUE_CODE ========== */
    gnrc_netif_hdr_t *netif_hdr;
    netif_pkt_t netif_pkt = {0};

    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("_send_ieee802154: first header is not generic netif header\n");
        return -EBADMSG;
    }
    if (pkt == NULL) {
        DEBUG("_send_ieee802154: pkt was NULL\n");
        return -EINVAL;
    }

    netif_hdr = pkt->data;

    netif_pkt.flags = netif_hdr->flags;
    netif_pkt.src_l2addr = gnrc_netif_hdr_get_src_addr(netif_hdr);
    netif_pkt.src_l2addr_len = netif_hdr->src_l2addr_len;
    netif_pkt.dst_l2addr = gnrc_netif_hdr_get_dst_addr(netif_hdr);
    netif_pkt.dst_l2addr_len = netif_hdr->dst_l2addr_len;
    memcpy(&netif_pkt.msdu, pkt->next, sizeof(iolist_t));
    netif_pkt.ctx = pkt;

    netif_send((netif_t*) netif, &netif_pkt);
    /* ========== END GNRC_NETIF_GLUE_CODE ========== */
    return 0;
}
/** @} */
