/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_mac
 * @{
 *
 * @file
 * @brief       Internal data types used by GNRC_MAC
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#ifndef NET_GNRC_MAC_TYPES_H
#define NET_GNRC_MAC_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <kernel_types.h>
#include <net/gnrc.h>
#include <net/gnrc/priority_pktqueue.h>
#include <net/ieee802154.h>
#include <net/gnrc/mac/mac.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief definition for device transmission feedback types
 */
typedef enum {
    TX_FEEDBACK_UNDEF = 0,    /**< Transmission just start, no Tx feedback yet */
    TX_FEEDBACK_SUCCESS,      /**< Transmission succeeded */
    TX_FEEDBACK_NOACK,        /**< No ACK for the transmitted packet */
    TX_FEEDBACK_BUSY          /**< found medium busy when doing transmission */
} gnrc_mac_tx_feedback_t;

/**
 * @brief Static initializer for gnrc_mac_tx_feedback_t.
 */
#define GNRC_MAC_TX_FEEDBACK_INIT { TX_FEEDBACK_UNDEF }

#if ((GNRC_MAC_RX_QUEUE_SIZE != 0) || (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0)) || defined(DOXYGEN)
/**
 * @brief MAC internal type for storing reception state parameters and
 *        state machines.
 *        This structure can be extended to contain more needed
 *        states and parameters. Please guard them by appropriate
 *        \#ifdef directives when applicable.
 */
typedef struct {
#if (GNRC_MAC_RX_QUEUE_SIZE != 0) || defined(DOXYGEN)
    gnrc_priority_pktqueue_t queue;                                      /**< RX packet queue */
    gnrc_priority_pktqueue_node_t _queue_nodes[GNRC_MAC_RX_QUEUE_SIZE];  /**< RX queue nodes */
#endif /* (GNRC_MAC_RX_QUEUE_SIZE != 0) || defined(DOXYGEN) */

#if (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0) || defined(DOXYGEN)
    gnrc_pktsnip_t* dispatch_buffer[GNRC_MAC_DISPATCH_BUFFER_SIZE];      /**< dispatch packet buffer */
#endif /* (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0) || defined(DOXYGEN) */
} gnrc_mac_rx_t;

/**
 * @brief Static initializer for gnrc_mac_rx_t.
 */
#if ((GNRC_MAC_RX_QUEUE_SIZE != 0) && (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0)) || defined(DOXYGEN)
#define GNRC_MAC_RX_INIT { \
    PRIORITY_PKTQUEUE_INIT, \
    { PRIORITY_PKTQUEUE_NODE_INIT(0, NULL) }, \
    { NULL }, \
}
#elif (GNRC_MAC_RX_QUEUE_SIZE != 0) && (GNRC_MAC_DISPATCH_BUFFER_SIZE == 0) || defined(DOXYGEN)
#define GNRC_MAC_RX_INIT { \
    PRIORITY_PKTQUEUE_INIT, \
    { PRIORITY_PKTQUEUE_NODE_INIT(0, NULL) }, \
}
#elif (GNRC_MAC_RX_QUEUE_SIZE == 0) && (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0) || defined(DOXYGEN)
#define GNRC_MAC_RX_INIT { \
    { NULL }, \
}
#endif /* ((GNRC_MAC_RX_QUEUE_SIZE != 0) && (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0)) || defined(DOXYGEN) */
#endif /* ((GNRC_MAC_RX_QUEUE_SIZE != 0) || (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0)) || defined(DOXYGEN) */

#if (GNRC_MAC_NEIGHBOR_COUNT != 0) || defined(DOXYGEN)
/**
 * @brief type for storing states of TX neighbor node.
 */
typedef struct {
    uint8_t  l2_addr[IEEE802154_LONG_ADDRESS_LEN];   /**< Address of neighbor node */
    uint8_t  l2_addr_len;                            /**< Neighbor address length */
    uint32_t phase;                                  /**< Neighbor's wake-up Phase */

#if (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN)
    gnrc_priority_pktqueue_t queue;                  /**< TX queue for this particular Neighbor */
#endif /* (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN) */
} gnrc_mac_tx_neighbor_t;

/**
 * @brief Uninitialized phase value.
 */
#define GNRC_MAC_PHASE_UNINITIALIZED   (0)

/**
 * @brief Maximum phase value.
 */
#define GNRC_MAC_PHASE_MAX             (-1)

/**
 * @brief Static initializer for gnrc_mac_tx_neighbor_t.
 */
#if (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN)
#define GNRC_MAC_TX_NEIGHBOR_INIT { \
    { 0 }, \
    0, \
    GNRC_MAC_PHASE_UNINITIALIZED, \
    PRIORITY_PKTQUEUE_INIT, \
}
#else
#define GNRC_MAC_TX_NEIGHBOR_INIT { \
    { 0 }, \
    0, \
    GNRC_MAC_PHASE_UNINITIALIZED, \
}
#endif /* (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN) */
#endif /* (GNRC_MAC_NEIGHBOR_COUNT != 0) || defined(DOXYGEN) */

#if ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (GNRC_MAC_NEIGHBOR_COUNT != 0)) || defined(DOXYGEN)
/**
 * @brief MAC internal type for storing transmission state parameters and
 *        state machines.
 *        This structure can be extended to contain more needed
 *        states and parameters. Please guard them by appropriate
 *        \#ifdef directives when applicable.
 */
typedef struct {
#if (GNRC_MAC_NEIGHBOR_COUNT != 0) || defined(DOXYGEN)
    gnrc_mac_tx_neighbor_t neighbors[GNRC_MAC_NEIGHBOR_COUNT + 1];      /**< Neighbor information units for one-hop neighbors.
                                                                             First unit is for broadcast (+1) */
    gnrc_mac_tx_neighbor_t* current_neighbor;                           /**< Neighbor information unit of destination node to which
                                                                             the current packet will be sent */
#endif /* (GNRC_MAC_NEIGHBOR_COUNT != 0) || defined(DOXYGEN) */

#if (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN)
#if (GNRC_MAC_NEIGHBOR_COUNT == 0) || defined(DOXYGEN)
    gnrc_priority_pktqueue_t queue;                                     /**< If neighbor queues is not used, define
                                                                             a single queue for managing TX packets. */
#endif /* (GNRC_MAC_NEIGHBOR_COUNT == 0) || defined(DOXYGEN) */

    gnrc_priority_pktqueue_node_t _queue_nodes[GNRC_MAC_TX_QUEUE_SIZE]; /**< Shared buffer for TX queue nodes */
    gnrc_pktsnip_t* packet;                                             /**< currently scheduled packet for sending */
#endif /* (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN) */
} gnrc_mac_tx_t;

/**
 * @brief Static initializer for gnrc_mac_tx_t.
 */
#if ((GNRC_MAC_TX_QUEUE_SIZE != 0) && (GNRC_MAC_NEIGHBOR_COUNT != 0)) || defined(DOXYGEN)
#define GNRC_MAC_TX_INIT { \
    { GNRC_MAC_TX_NEIGHBOR_INIT }, \
    NULL, \
    { PRIORITY_PKTQUEUE_NODE_INIT(0, NULL) }, \
    NULL, \
}
#elif ((GNRC_MAC_TX_QUEUE_SIZE != 0) && (GNRC_MAC_NEIGHBOR_COUNT == 0)) || defined(DOXYGEN)
#define GNRC_MAC_TX_INIT { \
    PRIORITY_PKTQUEUE_INIT, \
    { PRIORITY_PKTQUEUE_NODE_INIT(0, NULL) }, \
    NULL, \
}
#elif ((GNRC_MAC_TX_QUEUE_SIZE == 0) && (GNRC_MAC_NEIGHBOR_COUNT != 0)) || defined(DOXYGEN)
#define GNRC_MAC_TX_INIT { \
    { GNRC_MAC_TX_NEIGHBOR_INIT }, \
    NULL, \
}
#endif /* ((GNRC_MAC_TX_QUEUE_SIZE != 0) && (GNRC_MAC_NEIGHBOR_COUNT != 0)) || defined(DOXYGEN) */
#endif /* ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (GNRC_MAC_NEIGHBOR_COUNT != 0)) || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_MAC_TYPES_H */
/** @} */
