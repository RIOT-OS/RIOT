/*
 * Copyright (C) 2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netstats Packet statistics per module
 * @ingroup     net
 * @brief       Each module may store information about sent and received packets
 * @{
 *
 * @file
 * @brief       Definition of net statistics
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdint.h>
#include "cib.h"
#include "net/l2util.h"
#include "mutex.h"

#ifndef NET_NETSTATS_H
#define NET_NETSTATS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The max number of entries in the peer stats table
 */
#ifndef NETSTATS_NB_SIZE
#define NETSTATS_NB_SIZE        (8)
#endif

/**
 * @brief   The CIB size for tx correlation
 */
#ifndef NETSTATS_NB_QUEUE_SIZE
#define NETSTATS_NB_QUEUE_SIZE  (4)
#endif

/**
 * @name @ref net_netstats module names
 * @{
 */
#define NETSTATS_LAYER2     (0x01)
#define NETSTATS_IPV6       (0x02)
#define NETSTATS_RPL        (0x03)
#define NETSTATS_ALL        (0xFF)
/** @} */

/**
 * @brief       Global statistics struct
 */
typedef struct {
    uint32_t tx_unicast_count;  /**< packets sent via unicast */
    uint32_t tx_mcast_count;    /**< packets sent via multicast
                                     (including broadcast) */
    uint32_t tx_success;        /**< successful sending operations
                                     (either acknowledged or unconfirmed
                                     sending operation, e.g. multicast) */
    uint32_t tx_failed;         /**< failed sending operations */
    uint32_t tx_bytes;          /**< sent bytes */
    uint32_t rx_count;          /**< received (data) packets */
    uint32_t rx_bytes;          /**< received bytes */
} netstats_t;

/**
 * @brief       Stats per peer struct
 */
typedef struct {
#if IS_USED(MODULE_NETSTATS_NEIGHBOR_TX_TIME) || DOXYGEN
    uint32_t time_tx_avg;   /**< Average frame TX time in µs */
#endif
#if IS_USED(MODULE_NETSTATS_NEIGHBOR_ETX) || DOXYGEN
    uint16_t etx;           /**< ETX of this peer */
#endif
#if IS_USED(MODULE_NETSTATS_NEIGHBOR_COUNT) || DOXYGEN
    uint16_t tx_count;      /**< Number of sent frames to this peer */
    uint16_t tx_fail;       /**< Number of sent frames that did not get ACKed */
    uint16_t rx_count;      /**< Number of received frames */
#endif
    uint16_t last_updated;  /**< seconds timestamp of last update */
    uint16_t last_halved;   /**< seconds timestamp of last halving */
    uint8_t  l2_addr[L2UTIL_ADDR_MAX_LEN]; /**< Link layer address of the neighbor */
    uint8_t  l2_addr_len;   /**< Length of netstats_nb::l2_addr */
    uint8_t  freshness;     /**< Freshness counter */
#if IS_USED(MODULE_NETSTATS_NEIGHBOR_RSSI) || DOXYGEN
    uint8_t  rssi;          /**< Average RSSI of received frames in abs([dBm]) */
#endif
#if IS_USED(MODULE_NETSTATS_NEIGHBOR_LQI) || DOXYGEN
    uint8_t  lqi;           /**< Average LQI of received frames */
#endif
} netstats_nb_t;

/**
 * @brief       L2 Peer Info struct
 */
typedef struct {
    /**
     * @brief CIB for the tx correlation
     */
    cib_t stats_idx;

    /**
     * @brief send/callback mac association array
     */
    netstats_nb_t *stats_queue[NETSTATS_NB_QUEUE_SIZE];

    /**
     * @brief TX timestamp of stats_queue entries
     */
    uint32_t stats_queue_time_tx[NETSTATS_NB_QUEUE_SIZE];

    /**
     * @brief Per neighbor statistics array
     */
    netstats_nb_t pstats[NETSTATS_NB_SIZE];

    /**
     * @brief Neighbor Table access lock
     */
    mutex_t lock;
} netstats_nb_table_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_NETSTATS_H */
