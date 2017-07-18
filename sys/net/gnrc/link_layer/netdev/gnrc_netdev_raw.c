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
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "net/gnrc/netdev/raw.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define IP_VERSION_MASK (0xf0)
#define IP_VERSION4     (0x40)
#define IP_VERSION6     (0x60)

static gnrc_pktsnip_t *_recv(gnrc_netdev_t *gnrc_netdev)
{
    netdev_t *dev = gnrc_netdev->dev;
    int bytes_expected = dev->driver->recv(dev, NULL, 0, NULL);
    gnrc_pktsnip_t *pkt = NULL;

    if (bytes_expected > 0) {
        int nread;

        pkt = gnrc_pktbuf_add(NULL, NULL, bytes_expected, GNRC_NETTYPE_UNDEF);

        if (!pkt) {
            DEBUG("gnrc_netdev_raw: cannot allocate pktsnip.\n");
            /* drop packet */
            dev->driver->recv(dev, NULL, bytes_expected, NULL);
            return pkt;
        }
        nread = dev->driver->recv(dev, pkt->data, bytes_expected, NULL);
        if (nread <= 0) {
            DEBUG("gnrc_netdev_raw: read error.\n");
            gnrc_pktbuf_release(pkt);
            return NULL;
        }
        if (nread < bytes_expected) {
            /* we've got less then the expected packet size,
             * so free the unused space.*/
            DEBUG("gnrc_netdev_raw: reallocating.\n");
            gnrc_pktbuf_realloc_data(pkt, nread);
        }
        switch (((uint8_t *)pkt->data)[0] & IP_VERSION_MASK) {
#ifdef MODULE_GNRC_IPV6
            case IP_VERSION6:
                pkt->type = GNRC_NETTYPE_IPV6;
                break;
#endif
            default:
                /* leave UNDEF */
                break;
        }
    }
    return pkt;
}

static int _send(gnrc_netdev_t *gnrc_netdev, gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *vector;
    int res = -ENOBUFS;
    size_t n;

    if (pkt->type == GNRC_NETTYPE_NETIF) {
        /* we don't need the netif snip: remove it */
        pkt = gnrc_pktbuf_remove_snip(pkt, pkt);
    }
    vector = gnrc_pktbuf_get_iovec(pkt, &n);
    if (vector != NULL) {
        struct iovec *v = (struct iovec *)vector->data;
        netdev_t *dev = gnrc_netdev->dev;

#ifdef MODULE_NETSTATS_L2
        gnrc_netdev->dev->stats.tx_unicast_count++;
#endif
        res = dev->driver->send(dev, v, n);
    }
    return res;
}

int gnrc_netdev_raw_init(gnrc_netdev_t *gnrc_netdev, netdev_t *dev)
{
    gnrc_netdev->send = _send;
    gnrc_netdev->recv = _recv;
    gnrc_netdev->dev = dev;

    return 0;
}
/** @} */
