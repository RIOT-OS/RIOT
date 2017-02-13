/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc
 * @{
 *
 * @file
 * @brief       GNRC to netdev adapter for XBee devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "assert.h"
#include "xbee.h"
#include "net/gnrc.h"
#include "net/gnrc/netdev2/xbee_adpt.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define BCAST   (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)

static gnrc_pktsnip_t *xbee_adpt_recv(gnrc_netdev2_t *gnrc_netdev2)
{
    netdev2_t *dev = gnrc_netdev2->dev;
    int pktlen;
    int xhdr_len;
    gnrc_pktsnip_t *payload;
    gnrc_pktsnip_t *netif_snip;
    gnrc_pktsnip_t *xhdr_snip;
    gnrc_netif_hdr_t *netif;
    xbee_l2hdr_t l2hdr;

    assert(dev);

    /* see how much data there is to process */
    pktlen = dev->driver->recv(dev, NULL, 0, NULL);
    if (pktlen <= 0) {
        DEBUG("[xbee-gnrc] recv: no data available to process\n");
        return NULL;
    }

    /* allocate space for the packet in the pktbuf */
    payload = gnrc_pktbuf_add(NULL, NULL, pktlen, XBEE_DEFAULT_PROTOCOL);
    if (payload == NULL) {
        DEBUG("[xbee-gnrc] recv: unable to allocate space in the pktbuf\n");
        /* tell the driver to drop the packet */
        dev->driver->recv(dev, NULL, 1, NULL);
        return NULL;
    }

    /* copy the complete including the XBee header into the packet buffer */
    dev->driver->recv(dev, payload->data, pktlen, NULL);

    /* try to parse the L2 header data from the XBee header */
    xhdr_len = xbee_parse_hdr((xbee_t *)dev, (uint8_t *)payload->data, &l2hdr);
    if (xhdr_len < 0) {
        DEBUG("[xbee-gnrc] recv: unable to parse XBee header\n");
        gnrc_pktbuf_release(payload);
        return NULL;
    }

    /* crop the XBee header from the payload */
    xhdr_snip = gnrc_pktbuf_mark(payload, xhdr_len, GNRC_NETTYPE_UNDEF);
    if (xhdr_snip == NULL) {
        DEBUG("[xbee-gnrc] recv: unable to mark XBee header snip\n");
        gnrc_pktbuf_release(payload);
        return NULL;
    }
    gnrc_pktbuf_remove_snip(payload, xhdr_snip);

    /* create a netif hdr from the obtained data */
    netif_snip = gnrc_netif_hdr_build(l2hdr.src_addr, l2hdr.addr_len,
                                      l2hdr.dst_addr, l2hdr.addr_len);
    if (netif_snip == NULL) {
        DEBUG("[xbee-gnrc] recv: unable to allocate netif header\n");
        gnrc_pktbuf_release(payload);
        return NULL;
    }
    netif = (gnrc_netif_hdr_t *)netif_snip->data;
    netif->if_pid = gnrc_netdev2->pid;
    netif->rssi = l2hdr.rssi;
    if (l2hdr.bcast) {
        netif->flags = GNRC_NETIF_HDR_FLAGS_BROADCAST;
    }

    DEBUG("[xbee-gnrc] recv: successfully parsed packet\n");

    /* and append the netif header */
    LL_APPEND(payload, netif_snip);

    return payload;
}

static int xbee_adpt_send(gnrc_netdev2_t *dev, gnrc_pktsnip_t *pkt)
{
    int res;
    size_t size;
    size_t count;
    gnrc_pktsnip_t *vec;
    gnrc_netif_hdr_t *hdr;
    uint8_t xhdr[XBEE_MAX_TXHDR_LENGTH];

    /* check device descriptor and packet */
    assert(dev && pkt);

    /* get the payload size and the dst address details */
    size = gnrc_pkt_len(pkt->next);
    DEBUG("[xbee-gnrc] send: payload of packet is %i\n", (int)size);
    hdr = (gnrc_netif_hdr_t *)pkt->data;
    if (hdr->flags & BCAST) {
        uint16_t addr = 0xffff;
        res = xbee_build_hdr((xbee_t *)dev, xhdr, size, &addr, 2);
        DEBUG("[xbee-gnrc] send: preparing to send broadcast\n");
    }
    else {
        uint8_t *addr = gnrc_netif_hdr_get_dst_addr(hdr);
        res = xbee_build_hdr((xbee_t *)dev, xhdr, size, addr, hdr->dst_l2addr_len);
        if (res < 0) {
            if (res == -EOVERFLOW) {
                DEBUG("[xbee-gnrc] send: payload length exceeds max limit\n");
            }
            else if (res == -ENOMSG) {
                DEBUG("[xbee-gnrc] send: invalid destination l2 address\n");
            }
            return res;
        }
        if (hdr->dst_l2addr_len == IEEE802154_SHORT_ADDRESS_LEN) {
            DEBUG("[xbee-gnrc] send: preparing to send unicast to %02x:%02x\n",
                  (int)addr[0], (int)addr[1]);
        }
        else {
            DEBUG("[xbee-gnrc] send: preparing to send unicast to "
                  "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
                  (int)addr[0], (int)addr[1], (int)addr[2], (int)addr[3],
                  (int)addr[4], (int)addr[5], (int)addr[6], (int)addr[7]);
        }
    }

    /* now let's extract the iovector and send out the stuff */
    vec = gnrc_pktbuf_get_iovec(pkt, &count);
    if (vec != NULL) {
        pkt = vec;
        struct iovec *vector = (struct iovec *)pkt->data;
        vector[0].iov_base = xhdr;
        vector[0].iov_len = res;
#ifdef MODULE_NETSTATS_L2
        if (hdr->flags & BCAST) {
            dev->dev->stats.tx_mcast_count++;
        }
        else {
            dev->dev->stats.tx_unicast_count++;
        }
#endif
        DEBUG("[xbee-gnrc] send: triggering the drivers send function\n");
        res = dev->dev->driver->send(dev->dev, vector, count);
    }
    else {
        DEBUG("[xbee-gnrc] send: unable to create iovec\n");
    }

    gnrc_pktbuf_release(pkt);

    return res;
}

void gnrc_netdev2_xbee_init(gnrc_netdev2_t *gnrc_netdev2, xbee_t *dev)
{
    assert(gnrc_netdev2 && dev);

    gnrc_netdev2->send = xbee_adpt_send;
    gnrc_netdev2->recv = xbee_adpt_recv;
    gnrc_netdev2->dev = (netdev2_t *)dev;
}
