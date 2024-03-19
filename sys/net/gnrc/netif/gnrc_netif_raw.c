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

#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/netif/raw.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define IP_VERSION_MASK (0xf0U)
#define IP_VERSION4     (0x40U)
#define IP_VERSION6     (0x60U)

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);
static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif);

static const gnrc_netif_ops_t raw_ops = {
    .init = gnrc_netif_default_init,
    .send = _send,
    .recv = _recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
};

int gnrc_netif_raw_create(gnrc_netif_t *netif, char *stack, int stacksize,
                          char priority, char *name, netdev_t *dev)
{
    return gnrc_netif_create(netif, stack, stacksize, priority, name, dev,
                             &raw_ops);
}

static inline uint8_t _get_version(uint8_t *data)
{
    return (data[0] & IP_VERSION_MASK);
}

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    int bytes_expected = dev->driver->recv(dev, NULL, 0, NULL);
    gnrc_pktsnip_t *pkt = NULL;

    if (bytes_expected > 0) {
        gnrc_pktsnip_t *hdr;
        int nread;

        pkt = gnrc_pktbuf_add(NULL, NULL, bytes_expected, GNRC_NETTYPE_UNDEF);

        if (!pkt) {
            DEBUG("gnrc_netif_raw: cannot allocate pktsnip.\n");
            /* drop packet */
            dev->driver->recv(dev, NULL, bytes_expected, NULL);
            return pkt;
        }
        nread = dev->driver->recv(dev, pkt->data, bytes_expected, NULL);
        if (nread <= 1) {   /* we need at least 1 byte to identify IP version */
            DEBUG("gnrc_netif_raw: read error.\n");
            gnrc_pktbuf_release(pkt);
            return NULL;
        }
        hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
        if (!hdr) {
            DEBUG("gnrc_netif_raw: cannot allocate pktsnip.\n");
            gnrc_pktbuf_release(pkt);
            return NULL;
        }
        gnrc_netif_hdr_set_netif(hdr->data, netif);
        LL_APPEND(pkt, hdr);
#ifdef MODULE_NETSTATS_L2
        netif->stats.rx_count++;
        netif->stats.rx_bytes += nread;
#endif

        if (nread < bytes_expected) {
            /* we've got less then the expected packet size,
             * so free the unused space.*/
            DEBUG("gnrc_netif_raw: reallocating.\n");
            gnrc_pktbuf_realloc_data(pkt, nread);
        }
        switch (_get_version(pkt->data)) {
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

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    int res = -ENOBUFS;

    if (pkt->type == GNRC_NETTYPE_NETIF) {
        /* we don't need the netif snip: remove it */
        pkt = gnrc_pktbuf_remove_snip(pkt, pkt);
    }

    netdev_t *dev = netif->dev;

#ifdef MODULE_NETSTATS_L2
    netif->stats.tx_unicast_count++;
#endif

    res = dev->driver->send(dev, (iolist_t *)pkt);
    if (gnrc_netif_netdev_legacy_api(netif)) {
        /* only for legacy drivers we need to release pkt here */
        gnrc_pktbuf_release(pkt);
    }
    return res;
}

/** @} */
