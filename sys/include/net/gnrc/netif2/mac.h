/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif2
 * @{
 *
 * @file
 * @brief   @ref net_gnrc_mac definitions for @ref net_gnrc_netif2
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF2_MAC_H
#define NET_GNRC_NETIF2_MAC_H

#include "net/gnrc/mac/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   @ref net_gnrc_mac component of @ref gnrc_netif2_mac_t
 */
typedef struct {
#if ((GNRC_MAC_RX_QUEUE_SIZE != 0) || (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0)) || DOXYGEN
    /**
     * @brief MAC internal object which stores reception parameters, queues, and
     *        state machines.
     *
     * @note    Only available if @ref GNRC_MAC_RX_QUEUE_SIZE or
     *          @ref GNRC_MAC_DISPATCH_BUFFER_SIZE is greater than 0.
     */
    gnrc_mac_rx_t rx;
#endif /* ((GNRC_MAC_RX_QUEUE_SIZE != 0) || (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0)) || DOXYGEN */
#if ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (GNRC_MAC_NEIGHBOR_COUNT != 0)) || DOXYGEN
    /**
     * @brief MAC internal object which stores transmission parameters, queues, and
     *        state machines.
     *
     * @note    Only available if @ref GNRC_MAC_TX_QUEUE_SIZE or
     *          @ref GNRC_MAC_NEIGHBOR_COUNT is greater than 0.
     */
    gnrc_mac_tx_t tx;
#endif  /* ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (GNRC_MAC_NEIGHBOR_COUNT == 0)) || DOXYGEN */
} gnrc_netif2_mac_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF2_MAC_H */
/** @} */
