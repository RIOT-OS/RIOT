/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_GNRC_NETIF_MAC_H
#define NET_GNRC_NETIF_MAC_H
/**
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief   @ref net_gnrc_mac definitions for @ref net_gnrc_netif
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "net/gnrc/mac/types.h"
#include "net/csma_sender.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mask for @ref gnrc_mac_tx_feedback_t
 */
#define GNRC_NETIF_MAC_INFO_TX_FEEDBACK_MASK   (0x0003U)

/**
 * @brief   Flag to track if a transmission might have corrupted a received
 *          packet
 */
#define GNRC_NETIF_MAC_INFO_RX_STARTED         (0x0004U)

/**
 * @brief   Flag to track if a device has enabled CSMA for transmissions
 *
 * In case the device doesn't support on-chip CSMA and this flag is set for
 * requiring CSMA transmission, then, the device will run software CSMA
 * using `csma_sender` APIs.
 */
#define GNRC_NETIF_MAC_INFO_CSMA_ENABLED       (0x0100U)

#if defined(MODULE_GNRC_LWMAC) || defined(MODULE_GNRC_GOMACH)
/**
 * @brief Data type to hold MAC protocols
 */
typedef union {
#ifdef MODULE_GNRC_LWMAC
    /**
     * @brief LWMAC specific structure object for storing LWMAC internal states.
     */
    gnrc_lwmac_t lwmac;
#endif

#ifdef MODULE_GNRC_GOMACH
    /**
     * @brief GoMacH specific structure object for storing GoMacH internal states.
     */
    gnrc_gomach_t gomach;
#endif
} gnrc_mac_prot_t;
#endif

/**
 * @brief   @ref net_gnrc_mac component of @ref gnrc_netif_mac_t
 */
typedef struct {
    /**
     * @brief general information for the MAC protocol
     */
    uint16_t mac_info;

    /**
     * @brief device's software CSMA configuration
     */
    csma_sender_conf_t csma_conf;

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
#if ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0)) || DOXYGEN
    /**
     * @brief MAC internal object which stores transmission parameters, queues, and
     *        state machines.
     *
     * @note    Only available if @ref GNRC_MAC_TX_QUEUE_SIZE or
     *          @ref CONFIG_GNRC_MAC_NEIGHBOR_COUNT is greater than 0.
     */
    gnrc_mac_tx_t tx;
#endif  /* ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (CONFIG_GNRC_MAC_NEIGHBOR_COUNT == 0)) || DOXYGEN */

#if defined(MODULE_GNRC_LWMAC) || defined(MODULE_GNRC_GOMACH)
    gnrc_mac_prot_t prot;
#endif
} gnrc_netif_mac_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_NETIF_MAC_H */
