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
 * @brief       One block of RPL statistics
 */
typedef struct {
    uint32_t rx_ucast_count;            /**< unicast packets received */
    uint32_t rx_ucast_bytes;            /**< unicast bytes received */
    uint32_t rx_mcast_count;            /**< multicast packets received */
    uint32_t rx_mcast_bytes;            /**< multicast bytes received */
    uint32_t tx_ucast_count;            /**< unicast packets sent */
    uint32_t tx_ucast_bytes;            /**< unicast bytes sent */
    uint32_t tx_mcast_count;            /**< multicast packets sent */
    uint32_t tx_mcast_bytes;            /**< multicast bytes sent*/

} netstats_rpl_block_t;

/**
 * @brief       RPL statistics struct
 */
typedef struct {
    netstats_rpl_block_t dio;           /**< DIO statistics */
    netstats_rpl_block_t dis;           /**< DIS statistics */
    netstats_rpl_block_t dao;           /**< DAO statistics */
    netstats_rpl_block_t dao_ack;       /**< DAO-ACK statistics */
} netstats_rpl_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_RPL_RPL_NETSTATS_H */
/** @} */
