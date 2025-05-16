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
 * @brief       Internal data types used by GNRC_MAC
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */


#include <stdint.h>
#include <stdbool.h>

#include "net/gnrc/pkt.h"
#include "net/gnrc/priority_pktqueue.h"
#include "net/ieee802154.h"
#include "net/gnrc/mac/mac.h"
#ifdef MODULE_GNRC_LWMAC
#include "net/gnrc/lwmac/types.h"
#endif
#ifdef MODULE_GNRC_GOMACH
#include "net/gnrc/gomach/types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   MAC message type for getting radio's duty-cycle.
 */
#define GNRC_MAC_TYPE_GET_DUTYCYCLE      (0x4401)

/**
 * @brief definition for device transmission feedback types
 */
typedef enum {
    TX_FEEDBACK_UNDEF = 0,      /**< Transmission just start, no Tx feedback yet */
    TX_FEEDBACK_SUCCESS,        /**< Transmission succeeded */
    TX_FEEDBACK_NOACK,          /**< No ACK for the transmitted packet */
    TX_FEEDBACK_BUSY            /**< found medium busy when doing transmission */
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
    gnrc_priority_pktqueue_t queue;                                         /**< RX packet queue */
    gnrc_priority_pktqueue_node_t _queue_nodes[GNRC_MAC_RX_QUEUE_SIZE];     /**< RX queue nodes */
#endif /* (GNRC_MAC_RX_QUEUE_SIZE != 0) || defined(DOXYGEN) */

#if (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0) || defined(DOXYGEN)
    gnrc_pktsnip_t *dispatch_buffer[GNRC_MAC_DISPATCH_BUFFER_SIZE];      /**< dispatch packet buffer */
#endif /* (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0) || defined(DOXYGEN) */

#ifdef MODULE_GNRC_LWMAC
    gnrc_lwmac_l2_addr_t l2_addr; /**< Records the sender's address */
    gnrc_lwmac_rx_state_t state;  /**< LWMAC specific internal reception state */
    uint8_t rx_bad_exten_count;   /**< Count how many unnecessary RX extensions have been executed */
#endif

#ifdef MODULE_GNRC_GOMACH
    gnrc_gomach_listen_state_t listen_state;                              /**< Listen state. */
    gnrc_gomach_slosch_unit_t slosch_list[GNRC_GOMACH_SLOSCH_UNIT_COUNT]; /**< Queue-indicator record units. */
    gnrc_gomach_vtdma_manag_t vtdma_manag;                                /**< vTDMA management unit. */
    gnrc_gomach_dupchk_t check_dup_pkt;                                   /**< Check duplicate packet unit. */
#endif
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
#endif  /* ((GNRC_MAC_RX_QUEUE_SIZE != 0) && (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0)) || defined(DOXYGEN) */
#endif  /* ((GNRC_MAC_RX_QUEUE_SIZE != 0) || (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0)) || defined(DOXYGEN) */

#if (CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0) || defined(DOXYGEN)
/**
 * @brief type for storing states of TX neighbor node.
 */
typedef struct {
    uint8_t l2_addr[IEEE802154_LONG_ADDRESS_LEN];       /**< Address of neighbor node */
    uint8_t l2_addr_len;                                /**< Neighbor address length */
    uint32_t phase;                                     /**< Neighbor's wake-up Phase */

#if (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN)
    gnrc_priority_pktqueue_t queue;                  /**< TX queue for this particular Neighbor */
#endif /* (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN) */

#ifdef MODULE_GNRC_GOMACH
    uint16_t pub_chanseq;   /**< Neighbor's current public channel sequence. */
    uint32_t cp_phase;      /**< Neighbor's wake-up phase. */
    uint8_t mac_type;       /**< Neighbor's phase-track indicator. */
#endif
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
#endif  /* (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN) */
#endif  /* (CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0) || defined(DOXYGEN) */

#if ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0)) || defined(DOXYGEN)
/**
 * @brief MAC internal type for storing transmission state parameters and
 *        state machines.
 *        This structure can be extended to contain more needed
 *        states and parameters. Please guard them by appropriate
 *        \#ifdef directives when applicable.
 */
typedef struct {
#if (CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0) || defined(DOXYGEN)
    gnrc_mac_tx_neighbor_t neighbors[CONFIG_GNRC_MAC_NEIGHBOR_COUNT + 1];      /**< Neighbor information units for one-hop neighbors.
                                                                             First unit is for broadcast (+1) */
    gnrc_mac_tx_neighbor_t *current_neighbor;                           /**< Neighbor information unit of destination node to which
                                                                             the current packet will be sent */
#endif /* (CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0) || defined(DOXYGEN) */

#if (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN)
#if (CONFIG_GNRC_MAC_NEIGHBOR_COUNT == 0) || defined(DOXYGEN)
    gnrc_priority_pktqueue_t queue;                                     /**< If neighbor queues is not used, define
                                                                             a single queue for managing TX packets. */
#endif /* (CONFIG_GNRC_MAC_NEIGHBOR_COUNT == 0) || defined(DOXYGEN) */

    gnrc_priority_pktqueue_node_t _queue_nodes[GNRC_MAC_TX_QUEUE_SIZE]; /**< Shared buffer for TX queue nodes */
    gnrc_pktsnip_t *packet;                                             /**< currently scheduled packet for sending */
#endif /* (GNRC_MAC_TX_QUEUE_SIZE != 0) || defined(DOXYGEN) */

#ifdef MODULE_GNRC_LWMAC
    gnrc_lwmac_tx_state_t state;       /**< LWMAC specific internal transmission state */
    uint32_t wr_sent;                  /**< Count how many WRs were sent until WA received */
    uint32_t timestamp;                /**< Records the receiver's current phase */
    uint8_t bcast_seqnr;               /**< Sequence number for broadcast data to filter at receiver */
    uint8_t tx_burst_count;            /**< Count how many consecutive packets have been transmitted */
    uint8_t tx_retry_count;            /**< Count how many Tx-retrials have been executed before packet drop */
    uint8_t preamble_fail_counts;      /**< Preamble trial failure count. */
#endif

#ifdef MODULE_GNRC_GOMACH
    gnrc_gomach_transmit_state_t transmit_state;  /**< Basic transmission state. */
    gnrc_gomach_bcast_state_t bcast_state;        /**< Broadcast state. */
    gnrc_gomach_t2k_state_t t2k_state;            /**< t2k (transmit-to-known) state. */
    gnrc_gomach_t2u_state_t t2u_state;            /**< t2u (transmit-to-unknown) state. */
    uint8_t preamble_sent;                        /**< Preamble sent count. */
    uint8_t broadcast_seq;                        /**< Node's broadcast sequence. */
    uint8_t tx_seq;                               /**< Node's MAC transmission (TX) sequence. */
    gnrc_gomach_vtdma_t vtdma_para;               /**< Node's vTMDA slots allocation management unit. */
    uint8_t no_ack_counter;                       /**< Counter for recording no-ACK times for data transmission. */
    uint8_t t2u_retry_counter;                    /**< Counter for recording t2u attempt failures. */
    uint8_t last_tx_neighbor_id;                  /**< Record last TX neighbor's sequence in the neighbor list. */
    uint8_t tx_busy_count;                        /**< Counter recording csma busy feedback times. */
    uint8_t t2u_fail_count;                       /**< Preamble trial failure count. */
#endif
} gnrc_mac_tx_t;

/**
 * @brief Static initializer for gnrc_mac_tx_t.
 */
#if ((GNRC_MAC_TX_QUEUE_SIZE != 0) && (CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0)) || defined(DOXYGEN)
#define GNRC_MAC_TX_INIT { \
        { GNRC_MAC_TX_NEIGHBOR_INIT }, \
        NULL, \
        { PRIORITY_PKTQUEUE_NODE_INIT(0, NULL) }, \
        NULL, \
}
#elif ((GNRC_MAC_TX_QUEUE_SIZE != 0) && (CONFIG_GNRC_MAC_NEIGHBOR_COUNT == 0)) || defined(DOXYGEN)
#define GNRC_MAC_TX_INIT { \
        PRIORITY_PKTQUEUE_INIT, \
        { PRIORITY_PKTQUEUE_NODE_INIT(0, NULL) }, \
        NULL, \
}
#elif ((GNRC_MAC_TX_QUEUE_SIZE == 0) && (CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0)) || defined(DOXYGEN)
#define GNRC_MAC_TX_INIT { \
        { GNRC_MAC_TX_NEIGHBOR_INIT }, \
        NULL, \
}
#endif  /* ((GNRC_MAC_TX_QUEUE_SIZE != 0) && (CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0)) || defined(DOXYGEN) */
#endif  /* ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0)) || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

/** @} */
