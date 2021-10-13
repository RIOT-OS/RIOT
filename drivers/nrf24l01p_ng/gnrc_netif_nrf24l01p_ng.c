/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p_ng
 * @{
 *
 * @file
 * @brief     Implementation of gnrc_netif Rx/Tx adaptation functions
 *            for the NRF24L01+ (NG) transceiver
 *
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <assert.h>

#define ENABLE_DEBUG 0
#include "debug.h"

#include "net/gnrc.h"
#include "luid.h"
#include "gnrc_netif_nrf24l01p_ng.h"
#include "nrf24l01p_ng.h"

/**
 * @brief   Broadcast/Multicast flag
 */
#define BCAST  (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)

static gnrc_pktsnip_t *_nrf24l01p_ng_pkt_recv(gnrc_netif_t *netif)
{
    /* get frame size */
    int frame_len = netif->dev->driver->recv(netif->dev, NULL, 1, NULL);

    /* allocate space for the packet in the pktbuf */
    gnrc_pktsnip_t *frame = gnrc_pktbuf_add(NULL, NULL, frame_len,
                                            NRF24L01P_NG_UPPER_LAYER_PROTOCOL);
    if (!frame) {
        DEBUG_PUTS("[nrf24l01p_ng] _pkt_recv: unable to allocate space");
        return NULL;
    }

    /* copy the payload into the packet buffer */
    frame_len = netif->dev->driver->recv(netif->dev, frame->data,
                                         frame_len, NULL);
    if (frame_len <= 0) {
        DEBUG_PUTS("[nrf24l01p_ng] _pkt_recv: driver error");
        gnrc_pktbuf_release(frame);
        return NULL;
    }
    return frame;
}

/**
 * @brief   Receives a @ref net_gnrc_pkt "packet" from the network interface
 *
 * @pre `netif != NULL`
 *
 * @note The function takes the bytes received via netdev_driver_t::recv()
 *       from gnrc_netif_t::dev and re-formats it to a
 *       @ref net_gnrc_pkt "packet" containing a @ref net_gnrc_netif_hdr
 *       and a payload header in receive order.
 *
 * @param[in] netif The network interface.
 *
 * @return  The packet received. Contains the payload (with the type marked
 *          accordingly) and a @ref net_gnrc_netif_hdr in receive order.
 * @return  NULL, if @ref net_gnrc_pktbuf was full.
 */
static gnrc_pktsnip_t *_nrf24l01p_ng_adpt_recv(gnrc_netif_t *netif)
{
    assert(netif);
    assert(netif->dev);

    gnrc_pktsnip_t *frame;
    gnrc_pktsnip_t *snip;
    gnrc_netif_hdr_t *netif_hdr;
    uint8_t dst_addr[NRF24L01P_NG_ADDR_WIDTH];
    uint8_t src_addr[NRF24L01P_NG_ADDR_WIDTH];
    uint8_t src_addr_len;

    if (!(frame = _nrf24l01p_ng_pkt_recv(netif))) {
        DEBUG_PUTS("[nrf24l01p_ng] _adpt_recv: no frame");
        return NULL;
    }
    if (!(snip = gnrc_pktbuf_mark(frame, sizeof(dst_addr), GNRC_NETTYPE_UNDEF))) {
        DEBUG_PUTS("[nrf24l01p_ng] _adpt_recv: unable to mark header snip");
        gnrc_pktbuf_release(frame);
        return NULL;
    }
    memcpy(dst_addr, snip->data, sizeof(dst_addr));
    gnrc_pktbuf_remove_snip(frame, snip);

    src_addr_len = ((uint8_t *)frame->data)[0];
    if (src_addr_len < NRF24L01P_NG_MIN_ADDR_WIDTH ||
        src_addr_len > NRF24L01P_NG_MAX_ADDR_WIDTH) {
        DEBUG_PUTS("[nrf24l01p_ng] _adpt_recv: Invalid source address length");
        gnrc_pktbuf_release(frame);
        return NULL;
    }
    if (!(snip = gnrc_pktbuf_mark(frame, 1 + src_addr_len, GNRC_NETTYPE_UNDEF))) {
        DEBUG_PUTS("[nrf24l01p_ng] _adpt_recv: unable to mark src header snip");
        gnrc_pktbuf_release(frame);
        return NULL;
    }
    memcpy(src_addr, ((uint8_t *)snip->data) + 1, src_addr_len);
    gnrc_pktbuf_remove_snip(frame, snip);

    if (!(snip = gnrc_netif_hdr_build(src_addr, src_addr_len,
                                     dst_addr, sizeof(dst_addr)))) {
        DEBUG_PUTS("[nrf24l01p_ng] _adpt_recv: unable to allocate netif header");
        gnrc_pktbuf_release(frame);
        return NULL;
    }
    netif_hdr = (gnrc_netif_hdr_t *)snip->data;
    const uint8_t bcast_addr[] = NRF24L01P_NG_BROADCAST_ADDR;
    if (!memcmp(dst_addr, bcast_addr, sizeof(dst_addr))) {
        netif_hdr->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    }
    gnrc_netif_hdr_set_netif(netif_hdr, netif);
    LL_APPEND(frame, snip);
#if IS_USED(MODULE_NETSTATS_L2)
        netif->stats.rx_count++;
        netif->stats.rx_bytes += frame->size;
#endif
    return frame;
}

/**
 * @brief   Send a @ref net_gnrc_pkt "packet" over the network interface
 *
 * @pre `netif != NULL && pkt != NULL`
 *
 * @note The function re-formats the content of @p pkt to a format expected
 *       by the netdev_driver_t::send() method of gnrc_netif_t::dev and
 *       releases the packet before returning (so no additional release
 *       should be required after calling this method).
 *
 * @param[in] netif The network interface.
 * @param[in] pkt   A packet to send.
 *
 * @return  The number of bytes actually sent on success
 * @return  -EBADMSG, if the @ref net_gnrc_netif_hdr in @p pkt is missing
 *          or is in an unexpected format.
 * @return  -ENOTSUP, if sending @p pkt in the given format isn't supported
 *          (e.g. empty payload with Ethernet).
 * @return  Any negative error code reported by gnrc_netif_t::dev.
 */
static int _nrf24l01p_ng_adpt_send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    assert(netif);
    assert(pkt);
    assert(netif->dev);

    netdev_t *netdev = netif->dev;
    gnrc_netif_hdr_t *netif_hdr = (gnrc_netif_hdr_t *)pkt->data;
    if (!netif_hdr) {
        return -EBADMSG;
    }
    const uint8_t bcast_addr[] = NRF24L01P_NG_BROADCAST_ADDR;
    const uint8_t *dst_addr = gnrc_netif_hdr_get_dst_addr(netif_hdr);
    uint8_t dst_addr_len = netif_hdr->dst_l2addr_len;

    if (netif_hdr->flags & BCAST) {
        DEBUG_PUTS("[nrf24l01p_ng] _adpt_send: preparing to send broadcast");
        dst_addr = bcast_addr;
        dst_addr_len = NRF24L01P_NG_ADDR_WIDTH;
#if IS_USED(MODULE_NETSTATS_L2)
        netif->stats.tx_mcast_count++;
#endif
    }
    else {
        if (!dst_addr ||
            dst_addr_len > NRF24L01P_NG_MAX_ADDR_WIDTH ||
            dst_addr_len < NRF24L01P_NG_MIN_ADDR_WIDTH) {
            return -EBADMSG;
        }
        if (!memcmp(dst_addr, bcast_addr, dst_addr_len)) {
            DEBUG_PUTS("[nrf24l01p_ng] _adpt_send: preparing to send broadcast");
#if IS_USED(MODULE_NETSTATS_L2)
            netif->stats.tx_mcast_count++;
#endif
        }
    }

    uint8_t src[1 + NRF24L01P_NG_ADDR_WIDTH];
    src[0] = NRF24L01P_NG_ADDR_WIDTH;
    memcpy(src + 1,
           NRF24L01P_NG_ADDR_P1(container_of(netdev, nrf24l01p_ng_t, netdev)),
           NRF24L01P_NG_ADDR_WIDTH);
    iolist_t iolist_src_addr = {
        .iol_next = ((iolist_t *)pkt->next),
        .iol_base = src,
        .iol_len = sizeof(src)
    };
    iolist_t iolist = {
        .iol_next = &iolist_src_addr,
        .iol_base = (uint8_t *)dst_addr,
        .iol_len = dst_addr_len
    };
    int res;
    while ((res = netdev->driver->send(netdev, &iolist)) < 0) {
        if (res == -EAGAIN) {
            /* pending interrupts? */
            continue;
        }
        else if (res == -EBUSY) {
            if (!IS_ACTIVE(MODULE_GNRC_NETIF_PKTQ)) {
                /* busy send */
                continue;
            }
        }
        break;
    }
    gnrc_pktbuf_release(pkt);
    return res;
}

static const gnrc_netif_ops_t nrf24l01p_ng_netif_ops = {
    .init = gnrc_netif_default_init,
    .send = _nrf24l01p_ng_adpt_send,
    .recv = _nrf24l01p_ng_adpt_recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
};

int gnrc_netif_nrf24l01p_ng_create(gnrc_netif_t *netif, char *stack,
                                   int stacksize, char priority, char *name,
                                   netdev_t *dev)
{
    return gnrc_netif_create(netif, stack, stacksize, priority, name,
                             dev, &nrf24l01p_ng_netif_ops);
}

void __attribute__((weak)) nrf24l01p_ng_eui_get(const netdev_t *netdev, uint8_t *eui)
{
    (void)netdev;
    do {
        luid_get_lb(eui, NRF24L01P_NG_ADDR_WIDTH);
    }
    while (eui[NRF24L01P_NG_ADDR_WIDTH - 1] ==
           ((uint8_t[])NRF24L01P_NG_BROADCAST_ADDR)[NRF24L01P_NG_ADDR_WIDTH - 1]);
}
