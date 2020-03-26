/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p
 * @{
 *
 * @file
 * @brief     Implementation of gnrc_netif Rx/Tx adaptation functions for the
 *            NRF24L01P transceiver
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <assert.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "net/gnrc.h"
#include "nrf24l01p_internal.h"
#include "nrf24l01p_custom_header.h"
#include "gnrc_netif_nrf24l01p.h"

/**
 * @brief   Broadcast/Multicast flag
 */
#define BCAST  (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)

static gnrc_pktsnip_t *_nrf24l01p_pkt_recv(gnrc_netif_t *netif)
{
    /* get upper size limit */
    int upper_frame_len = netif->dev->driver->recv(netif->dev, NULL, 0, NULL);

    uint8_t frame_buffer[upper_frame_len];
    /* copy the payload into the packet buffer */
    int frame_len = netif->dev->driver->recv(netif->dev, frame_buffer,
                                                 upper_frame_len, NULL);
    if (frame_len <= 0) {
        return NULL;
    }

    /* allocate space for the packet in the pktbuf */
    gnrc_pktsnip_t *frame = gnrc_pktbuf_add(NULL, NULL, frame_len,
                                            NRF24L01P_UPPER_LAYER_PROTOCOL);
    if (!frame) {
        DEBUG("[nrf24l01p] recv: unable to allocate space in the pktbuf\n");
        return NULL;
    }
    memcpy(frame->data, frame_buffer, frame_len);
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
static gnrc_pktsnip_t *nrf24l01p_adpt_recv(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    uint8_t header_len = 0;
    gnrc_pktsnip_t *frame;
    gnrc_pktsnip_t *hdr;
    gnrc_netif_hdr_t *netif_hdr;
    shockburst_hdr_t sb_hdr;

    assert(dev);

    if (!(frame = _nrf24l01p_pkt_recv(netif))) {
        return NULL;
    }
    sb_hdr.addr_width = ((uint8_t *)(frame->data))[header_len++];
    uint8_t dst_addr_width = sb_hdr_get_dst_addr_width(&sb_hdr);
    memcpy(sb_hdr.dst_addr,
           ((uint8_t *)(frame->data)) + header_len,
           dst_addr_width);
    header_len += dst_addr_width;
#if NRF24L01P_CUSTOM_HEADER
    uint8_t src_addr_width = sb_hdr_get_src_addr_width(&sb_hdr);
    memcpy(sb_hdr.src_addr,
           ((uint8_t *)(frame->data)) + header_len,
           src_addr_width);
    header_len += src_addr_width;
#endif
    DEBUG("[nrf24l01p] header length: %u\n", header_len);
    hdr = gnrc_pktbuf_mark(frame, header_len, GNRC_NETTYPE_UNDEF);
    if (!hdr) {
        DEBUG("[nrf24l01p] recv: unable to mark nrf24l01p header snip\n");
        gnrc_pktbuf_release(frame);
        return NULL;
    }
    gnrc_pktbuf_remove_snip(frame, hdr);
#if NRF24L01P_CUSTOM_HEADER
    hdr = gnrc_netif_hdr_build(sb_hdr.src_addr, src_addr_width, sb_hdr.dst_addr,
                               dst_addr_width);
#else
    hdr = gnrc_netif_hdr_build(NULL, 0, sb_hdr.dst_addr, dst_addr_width);
#endif
    if (!hdr) {
        DEBUG("[nrf24l01p] recv: unable to allocate netif header\n");
        gnrc_pktbuf_release(frame);
        return NULL;
    }
    netif_hdr = (gnrc_netif_hdr_t *)hdr->data;
    uint8_t bcast_addr[] = NRF24L01P_BROADCAST_ADDR;
    if (!memcmp(&sb_hdr.dst_addr, bcast_addr, dst_addr_width)) {
        netif_hdr->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    }
    gnrc_netif_hdr_set_netif(netif_hdr, netif);
    LL_APPEND(frame, hdr);

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
static int nrf24l01p_adpt_send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    assert(netif && pkt);
    netdev_t *netdev = (netdev_t *)netif->dev;
    gnrc_netif_hdr_t *netif_hdr = (gnrc_netif_hdr_t *)pkt->data;
    if (!netif_hdr) {
        return -EBADMSG;
    }
    uint8_t bcast_addr[] = NRF24L01P_BROADCAST_ADDR;
    uint8_t *tx_address;
    uint8_t tx_addr_len;
    if (netif_hdr->flags & BCAST) {
        DEBUG("[nrf24l01p-gnrc] send: preparing to send broadcast\n");
        tx_address = bcast_addr;
        tx_addr_len = sizeof(bcast_addr);
#if IS_USED(MODULE_NETSTATS_L2)
        netif->stats.tx_mcast_count++;
#endif
    }
    else {
        if (netif_hdr->dst_l2addr_len > NRF24L01P_MAX_ADDR_WIDTH ||
            netif_hdr->dst_l2addr_len < NRF24L01P_MIN_ADDR_WIDTH) {
            return -EBADMSG;
        }
        tx_address = gnrc_netif_hdr_get_dst_addr(netif_hdr);
        tx_addr_len = netif_hdr->dst_l2addr_len;
    }

    shockburst_hdr_t hdr;
    sb_hdr_init(&hdr);
    sb_hdr_set_dst_addr_width(&hdr, tx_addr_len);
    memcpy(hdr.dst_addr, tx_address, tx_addr_len);
#if NRF24L01P_CUSTOM_HEADER
    nrf24l01p_t *dev = (nrf24l01p_t *)netdev;
    uint8_t aw = nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width);
    sb_hdr_set_src_addr_width(&hdr, aw);
    memcpy(hdr.src_addr, dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr, aw);
#endif
    iolist_t iolist = (iolist_t){
        .iol_next = (iolist_t *)pkt->next,
        .iol_base = &hdr,
        .iol_len = sizeof(hdr)
    };
    int res;
    /* pending data in FIFO? */
    while (-EAGAIN == (res = netdev->driver->send(netdev, &iolist))) {}
    gnrc_pktbuf_release(pkt);
    return res;
}

static const gnrc_netif_ops_t nrf24l01p_netif_ops = {
    .init = gnrc_netif_default_init,
    .send = nrf24l01p_adpt_send,
    .recv = nrf24l01p_adpt_recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
};

gnrc_netif_t *gnrc_netif_nrf24l01p_create(char *stack, int stacksize,
                                          char priority, char *name,
                                          netdev_t *dev)
{
    return gnrc_netif_create(stack, stacksize, priority, name,
                             dev, &nrf24l01p_netif_ops);
}
