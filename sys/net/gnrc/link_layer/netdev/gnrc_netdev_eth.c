/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       gnrc netdev ethernet glue code
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include "net/gnrc.h"
#include "net/gnrc/netdev.h"
#include "net/ethernet/hdr.h"

#ifdef MODULE_GNRC_IPV6
#include "net/ipv6/hdr.h"
#endif

#include "od.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static gnrc_pktsnip_t *_recv(gnrc_netdev_t *gnrc_netdev)
{
    netdev_t *dev = gnrc_netdev->dev;
    int bytes_expected = dev->driver->recv(dev, NULL, 0, NULL);
    gnrc_pktsnip_t *pkt = NULL;

    if (bytes_expected > 0) {
        pkt = gnrc_pktbuf_add(NULL, NULL,
                bytes_expected,
                GNRC_NETTYPE_UNDEF);

        if(!pkt) {
            DEBUG("_recv_ethernet_packet: cannot allocate pktsnip.\n");

            /* drop the packet */
            dev->driver->recv(dev, NULL, bytes_expected, NULL);

            goto out;
        }

        int nread = dev->driver->recv(dev, pkt->data, bytes_expected, NULL);
        if(nread <= 0) {
            DEBUG("_recv_ethernet_packet: read error.\n");
            goto safe_out;
        }

        if (nread < bytes_expected) {
            /* we've got less then the expected packet size,
             * so free the unused space.*/

            DEBUG("_recv_ethernet_packet: reallocating.\n");
            gnrc_pktbuf_realloc_data(pkt, nread);
        }

        /* mark ethernet header */
        gnrc_pktsnip_t *eth_hdr = gnrc_pktbuf_mark(pkt, sizeof(ethernet_hdr_t), GNRC_NETTYPE_UNDEF);
        if (!eth_hdr) {
            DEBUG("gnrc_netdev_eth: no space left in packet buffer\n");
            goto safe_out;
        }

        ethernet_hdr_t *hdr = (ethernet_hdr_t *)eth_hdr->data;

#ifdef MODULE_L2FILTER
        if (!l2filter_pass(dev->filter, hdr->src, ETHERNET_ADDR_LEN)) {
            DEBUG("gnrc_netdev_eth: incoming packet filtered by l2filter\n");
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
            DEBUG("gnrc_netdev_eth: no space left in packet buffer\n");
            pkt = eth_hdr;
            goto safe_out;
        }

        gnrc_netif_hdr_init(netif_hdr->data, ETHERNET_ADDR_LEN, ETHERNET_ADDR_LEN);
        gnrc_netif_hdr_set_src_addr(netif_hdr->data, hdr->src, ETHERNET_ADDR_LEN);
        gnrc_netif_hdr_set_dst_addr(netif_hdr->data, hdr->dst, ETHERNET_ADDR_LEN);
        ((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid = thread_getpid();

        DEBUG("gnrc_netdev_eth: received packet from %02x:%02x:%02x:%02x:%02x:%02x "
                "of length %d\n",
                hdr->src[0], hdr->src[1], hdr->src[2], hdr->src[3], hdr->src[4],
                hdr->src[5], nread);
#if defined(MODULE_OD) && ENABLE_DEBUG
        od_hex_dump(hdr, nread, OD_WIDTH_DEFAULT);
#endif

        gnrc_pktbuf_remove_snip(pkt, eth_hdr);
        LL_APPEND(pkt, netif_hdr);
    }

out:
    return pkt;

safe_out:
    gnrc_pktbuf_release(pkt);
    return NULL;
}

int gnrc_netdev_eth_init(gnrc_netdev_t *gnrc_netdev, netdev_t *dev)
{
    gnrc_netdev->send = _send;
    gnrc_netdev->recv = _recv;
    gnrc_netdev->dev = dev;

    return 0;
}
