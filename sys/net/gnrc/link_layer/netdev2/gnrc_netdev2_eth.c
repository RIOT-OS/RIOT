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
 * @brief       gnrc netdev2 ethernet glue code
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include "net/gnrc.h"
#include "net/gnrc/netdev2.h"
#include "net/ethernet/hdr.h"

#include "od.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static gnrc_pktsnip_t *_recv(gnrc_netdev2_t *gnrc_netdev2)
{
    netdev2_t *dev = gnrc_netdev2->dev;
    int bytes_expected = dev->driver->recv(dev, NULL, 0);
    gnrc_pktsnip_t *pkt = NULL;

    if (bytes_expected) {
        pkt = gnrc_pktbuf_add(NULL, NULL,
                bytes_expected,
                GNRC_NETTYPE_UNDEF);

        if(!pkt) {
            DEBUG("_recv_ethernet_packet: cannot allocate pktsnip.\n");
            goto out;
        }

        int nread = dev->driver->recv(dev, pkt->data, bytes_expected);
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
            DEBUG("gnrc_netdev2_eth: no space left in packet buffer\n");
            goto safe_out;
        }

        ethernet_hdr_t *hdr = (ethernet_hdr_t *)eth_hdr->data;

        /* set payload type from ethertype */
        pkt->type = gnrc_nettype_from_ethertype(byteorder_ntohs(hdr->type));

        /* create netif header */
        gnrc_pktsnip_t *netif_hdr;
        netif_hdr = gnrc_pktbuf_add(NULL, NULL,
                sizeof(gnrc_netif_hdr_t) + (2 * ETHERNET_ADDR_LEN),
                GNRC_NETTYPE_NETIF);

        if (netif_hdr == NULL) {
            DEBUG("gnrc_netdev2_eth: no space left in packet buffer\n");
            pkt = eth_hdr;
            goto safe_out;
        }

        gnrc_netif_hdr_init(netif_hdr->data, ETHERNET_ADDR_LEN, ETHERNET_ADDR_LEN);
        gnrc_netif_hdr_set_src_addr(netif_hdr->data, hdr->src, ETHERNET_ADDR_LEN);
        gnrc_netif_hdr_set_dst_addr(netif_hdr->data, hdr->dst, ETHERNET_ADDR_LEN);
        ((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid = thread_getpid();

        DEBUG("gnrc_netdev2_eth: received packet from %02x:%02x:%02x:%02x:%02x:%02x "
                "of length %zu\n",
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

static inline void _addr_set_broadcast(uint8_t *dst)
{
    memset(dst, 0xff, ETHERNET_ADDR_LEN);
}

#define _IPV6_DST_OFFSET    (36)    /* sizeof(ipv6_hdr_t) - 4  */

static inline void _addr_set_multicast(uint8_t *dst, gnrc_pktsnip_t *payload)
{
    switch (payload->type) {
#ifdef MODULE_IPV6
        case GNRC_NETTYPE_IPV6:
            dst[0] = 0x33;
            dst[1] = 0x33;
            memcpy(dst + 2, ((uint8_t *)payload->data) + _IPV6_DST_OFFSET, 4);
            /* TODO change to proper types when gnrc_ipv6_hdr_t got merged */
            break;
#endif
        default:
            _addr_set_broadcast(dst);
            break;
    }
}

static int _send(gnrc_netdev2_t *gnrc_netdev2, gnrc_pktsnip_t *pkt)
{
    ethernet_hdr_t hdr;
    gnrc_netif_hdr_t *netif_hdr;
    gnrc_pktsnip_t *payload;

    netdev2_t *dev = gnrc_netdev2->dev;

    if (pkt == NULL) {
        DEBUG("gnrc_netdev2_eth: pkt was NULL");
        return -EINVAL;
    }

    payload = pkt->next;

    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("gnrc_netdev2_eth: First header was not generic netif header\n");
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
            DEBUG("gnrc_netdev2_eth: empty multicast packets over Ethernet "\
                  "are not yet supported\n");
            return -ENOTSUP;
        }
        _addr_set_multicast(hdr.dst, payload);
    }
    else if (netif_hdr->dst_l2addr_len == ETHERNET_ADDR_LEN) {
        memcpy(hdr.dst, gnrc_netif_hdr_get_dst_addr(netif_hdr),
               ETHERNET_ADDR_LEN);
    }
    else {
        DEBUG("gnrc_netdev2_eth: destination address had unexpected format\n");
        return -EBADMSG;
    }

    DEBUG("gnrc_netdev2_eth: send to %02x:%02x:%02x:%02x:%02x:%02x\n",
          hdr.dst[0], hdr.dst[1], hdr.dst[2],
          hdr.dst[3], hdr.dst[4], hdr.dst[5]);

    size_t n;
    pkt = gnrc_pktbuf_get_iovec(pkt, &n);
    struct iovec *vector = (struct iovec *)pkt->data;
    vector[0].iov_base = (char*)&hdr;
    vector[0].iov_len = sizeof(ethernet_hdr_t);
    dev->driver->send(dev, vector, n);

    gnrc_pktbuf_release(pkt);

    return 0;
}

int gnrc_netdev2_eth_init(gnrc_netdev2_t *gnrc_netdev2, netdev2_t *dev)
{
    gnrc_netdev2->send = _send;
    gnrc_netdev2->recv = _recv;
    gnrc_netdev2->dev = dev;

    return 0;
}
