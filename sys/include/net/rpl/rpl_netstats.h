/*
 * Copyright (C) 2016 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netstats_rpl Packet statistics for RPL
 * @ingroup     net_netstats
 * @brief       Packet statistics for RPL
 * @{
 *
 * @file
 * @brief       Definition of RPL related packet statistics
 *
 * @author      Cenk Gündoğan <mail@cgundogan.de>
 */

#include <stdint.h>

#ifndef NET_RPL_RPL_NETSTATS_H
#define NET_RPL_RPL_NETSTATS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       RPL statistics struct
 */
typedef struct {
    /* DIO */
    uint32_t dio_rx_ucast_count;        /**< unicast dio received in packets */
    uint32_t dio_rx_ucast_bytes;        /**< unicast dio received in bytes */
    uint32_t dio_rx_mcast_count;        /**< multicast dio received in packets */
    uint32_t dio_rx_mcast_bytes;        /**< multicast dio received in bytes */
    uint32_t dio_tx_ucast_count;        /**< unicast dio sent in packets */
    uint32_t dio_tx_ucast_bytes;        /**< unicast dio sent in bytes */
    uint32_t dio_tx_mcast_count;        /**< multicast dio sent in packets */
    uint32_t dio_tx_mcast_bytes;        /**< multicast dio sent in bytes*/
    /* DIS */
    uint32_t dis_rx_ucast_count;        /**< unicast dis received in packets */
    uint32_t dis_rx_ucast_bytes;        /**< unicast dis received in bytes */
    uint32_t dis_rx_mcast_count;        /**< multicast dis received in packets */
    uint32_t dis_rx_mcast_bytes;        /**< multicast dis received in bytes */
    uint32_t dis_tx_ucast_count;        /**< unicast dis sent in packets */
    uint32_t dis_tx_ucast_bytes;        /**< unicast dis sent in bytes */
    uint32_t dis_tx_mcast_count;        /**< multicast dis sent in packets */
    uint32_t dis_tx_mcast_bytes;        /**< multicast dis sent in bytes*/
    /* DAO */
    uint32_t dao_rx_ucast_count;        /**< unicast dao received in packets */
    uint32_t dao_rx_ucast_bytes;        /**< unicast dao received in bytes */
    uint32_t dao_rx_mcast_count;        /**< multicast dao received in packets */
    uint32_t dao_rx_mcast_bytes;        /**< multicast dao received in bytes */
    uint32_t dao_tx_ucast_count;        /**< unicast dao sent in packets */
    uint32_t dao_tx_ucast_bytes;        /**< unicast dao sent in bytes */
    uint32_t dao_tx_mcast_count;        /**< multicast dao sent in packets */
    uint32_t dao_tx_mcast_bytes;        /**< multicast dao sent in bytes*/
    /* DAO-ACK */
    uint32_t dao_ack_rx_ucast_count;    /**< unicast dao_ack received in packets */
    uint32_t dao_ack_rx_ucast_bytes;    /**< unicast dao_ack received in bytes */
    uint32_t dao_ack_rx_mcast_count;    /**< multicast dao_ack received in packets */
    uint32_t dao_ack_rx_mcast_bytes;    /**< multicast dao_ack received in bytes */
    uint32_t dao_ack_tx_ucast_count;    /**< unicast dao_ack sent in packets */
    uint32_t dao_ack_tx_ucast_bytes;    /**< unicast dao_ack sent in bytes */
    uint32_t dao_ack_tx_mcast_count;    /**< multicast dao_ack sent in packets */
    uint32_t dao_ack_tx_mcast_bytes;    /**< multicast dao_ack sent in bytes*/
} netstats_rpl_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_RPL_RPL_NETSTATS_H */
/** @} */
