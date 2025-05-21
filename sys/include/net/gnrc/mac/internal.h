/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     net_gnrc_mac
 * @{
 *
 * @file
 * @brief       Definitions of internal functions of GNRC_MAC module
 * @internal
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#include <stdint.h>

#include "net/ieee802154.h"
#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief get the 'rx_started' state of the device
 *
 * This function checks whether the device has started receiving a packet.
 *
 * @param[in] netif the network interface
 *
 * @return          the rx_started state
 */
static inline bool gnrc_netif_get_rx_started(gnrc_netif_t *netif)
{
    return (netif->mac.mac_info & GNRC_NETIF_MAC_INFO_RX_STARTED);
}

/**
 * @brief set the rx_started state of the device
 *
 * This function is intended to be called only in netdev_t::event_callback().
 *
 * @param[in] netif       the network interface
 * @param[in] rx_started  the rx_started state
 */
static inline void gnrc_netif_set_rx_started(gnrc_netif_t *netif, bool rx_started)
{
    if (rx_started) {
        netif->mac.mac_info |= GNRC_NETIF_MAC_INFO_RX_STARTED;
    }
    else {
        netif->mac.mac_info &= ~GNRC_NETIF_MAC_INFO_RX_STARTED;
    }
}

/**
 * @brief get the transmission feedback of the device
 *
 * @param[in] netif  the network interface
 *
 * @return           the transmission feedback
 */
static inline gnrc_mac_tx_feedback_t gnrc_netif_get_tx_feedback(gnrc_netif_t *netif)
{
    return (gnrc_mac_tx_feedback_t)(netif->mac.mac_info &
                                    GNRC_NETIF_MAC_INFO_TX_FEEDBACK_MASK);
}

/**
 * @brief set the transmission feedback of the device
 *
 * This function is intended to be called only in netdev_t::event_callback().
 *
 * @param[in] netif  the network interface
 * @param[in] txf    the transmission feedback
 */
static inline void gnrc_netif_set_tx_feedback(gnrc_netif_t *netif,
                                              gnrc_mac_tx_feedback_t txf)
{
    /* check if gnrc_mac_tx_feedback does not collide with
     * GNRC_NETIF_MAC_INFO_RX_STARTED */
    assert(!(txf & GNRC_NETIF_MAC_INFO_RX_STARTED));
    /* unset previous value */
    netif->mac.mac_info &= ~GNRC_NETIF_MAC_INFO_TX_FEEDBACK_MASK;
    netif->mac.mac_info |= (uint16_t)(txf & GNRC_NETIF_MAC_INFO_TX_FEEDBACK_MASK);
}

#if (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN)
/**
 * @brief Queues the packet into the related transmission packet queue in netdev_t::tx.
 *        Note that, in case the `gnrc_mac_tx_neighbor_t` structure is in used (indicated
 *        by `CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0`), this function queues the packet to
 *        the queue associated to the pkt's destination neighbor, including a
 *        `broadcast-neighbor` (neighbor id is `0` in netdev_t::tx::neighbors) which
 *        specifically stores broadcasting packets.
 *        On the other hand, if `gnrc_mac_tx_neighbor_t` structure is not in used (indicated
 *        by `CONFIG_GNRC_MAC_NEIGHBOR_COUNT == 0`), this function queues the packet into the single
 *        priority TX queue defined in in netdev_t::tx.
 *
 * @param[in,out] tx        gnrc_mac transmission management object
 * @param[in]     priority  the priority of @p pkt
 * @param[in]     pkt       gnrc packet that will be queued
 *
 * @return                  true if queued successfully, otherwise false.
 */
bool gnrc_mac_queue_tx_packet(gnrc_mac_tx_t *tx, uint32_t priority, gnrc_pktsnip_t *pkt);
#endif /* (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN) */

#if (GNRC_MAC_RX_QUEUE_SIZE != 0) || defined(DOXYGEN)
/**
 * @brief Queues the packet into the reception packet queue in netdev_t::rx.
 *
 * @param[in,out] rx        gnrc_mac reception management object
 * @param[in]     priority  the priority of @p pkt
 * @param[in]     pkt       gnrc packet that will be queued
 *
 * @return                  true if queued successfully, otherwise false.
 */
bool gnrc_mac_queue_rx_packet(gnrc_mac_rx_t *rx, uint32_t priority, gnrc_pktsnip_t *pkt);
#endif /* (GNRC_MAC_RX_QUEUE_SIZE != 0) || defined(DOXYGEN) */

#if (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0) || defined(DOXYGEN)
/**
 * @brief Dispatch all the packets stored in netdev_t::rx:dispatch_buffer to upper layer.
 *
 * @param[in,out]  rx       gnrc_mac reception management object
 */
void gnrc_mac_dispatch(gnrc_mac_rx_t *rx);
#endif /* (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0) || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

/** @} */
