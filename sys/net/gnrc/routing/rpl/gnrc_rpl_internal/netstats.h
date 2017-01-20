/*
 * Copyright (C) 2016 Cenk Gündoğan <mail@cgundogan.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_rpl
 * @{
 *
 * @file
 * @brief       RPL control message statistics functions
 *
 * @author      Cenk Gündoğan <mail@cgundogan.de>
 */

#ifndef RPL_NETSTATS_H
#define RPL_NETSTATS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "net/rpl/rpl_netstats.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define GNRC_RPL_NETSTATS_MULTICAST (0)
#define GNRC_RPL_NETSTATS_UNICAST   (1)

/**
 * @brief   Increase statistics for received DIO
 *
 * @param[in]   netstats    Pointer to netstats_rpl_t
 * @param[in]   len         Length in bytes of an ICMPv6 packet to add to statistics
 * @param[in]   cast        GNRC_RPL_NETSTATS_MULTICAST or GNRC_RPL_NETSTATS_UNICAST
 */
static inline void gnrc_rpl_netstats_rx_DIO(netstats_rpl_t *netstats, size_t len, int cast)
{
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dio_rx_mcast_count++;
        netstats->dio_rx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dio_rx_ucast_count++;
        netstats->dio_rx_ucast_bytes += len;
    }
}

/**
 * @brief   Increase statistics for sent DIO
 *
 * @param[in]   netstats    Pointer to netstats_rpl_t
 * @param[in]   len         Length in bytes of an ICMPv6 packet to add to statistics
 * @param[in]   cast        GNRC_RPL_NETSTATS_MULTICAST or GNRC_RPL_NETSTATS_UNICAST
 */
static inline void gnrc_rpl_netstats_tx_DIO(netstats_rpl_t *netstats, size_t len, int cast)
{
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dio_tx_mcast_count++;
        netstats->dio_tx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dio_tx_ucast_count++;
        netstats->dio_tx_ucast_bytes += len;
    }
}

/**
 * @brief   Increase statistics for received DIS
 *
 * @param[in]   netstats    Pointer to netstats_rpl_t
 * @param[in]   len         Length in bytes of an ICMPv6 packet to add to statistics
 * @param[in]   cast        GNRC_RPL_NETSTATS_MULTICAST or GNRC_RPL_NETSTATS_UNICAST
 */
static inline void gnrc_rpl_netstats_rx_DIS(netstats_rpl_t *netstats, size_t len, int cast)
{
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dis_rx_mcast_count++;
        netstats->dis_rx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dis_rx_ucast_count++;
        netstats->dis_rx_ucast_bytes += len;
    }
}

/**
 * @brief   Increase statistics for sent DIS
 *
 * @param[in]   netstats    Pointer to netstats_rpl_t
 * @param[in]   len         Length in bytes of an ICMPv6 packet to add to statistics
 * @param[in]   cast        GNRC_RPL_NETSTATS_MULTICAST or GNRC_RPL_NETSTATS_UNICAST
 */
static inline void gnrc_rpl_netstats_tx_DIS(netstats_rpl_t *netstats, size_t len, int cast)
{
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dis_tx_mcast_count++;
        netstats->dis_tx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dis_tx_ucast_count++;
        netstats->dis_tx_ucast_bytes += len;
    }
}

/**
 * @brief   Increase statistics for received DAO
 *
 * @param[in]   netstats    Pointer to netstats_rpl_t
 * @param[in]   len         Length in bytes of an ICMPv6 packet to add to statistics
 * @param[in]   cast        GNRC_RPL_NETSTATS_MULTICAST or GNRC_RPL_NETSTATS_UNICAST
 */
static inline void gnrc_rpl_netstats_rx_DAO(netstats_rpl_t *netstats, size_t len, int cast)
{
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dao_rx_mcast_count++;
        netstats->dao_rx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dao_rx_ucast_count++;
        netstats->dao_rx_ucast_bytes += len;
    }
}

/**
 * @brief   Increase statistics for sent DIO
 *
 * @param[in]   netstats    Pointer to netstats_rpl_t
 * @param[in]   len         Length in bytes of an ICMPv6 packet to add to statistics
 * @param[in]   cast        GNRC_RPL_NETSTATS_MULTICAST or GNRC_RPL_NETSTATS_UNICAST
 */
static inline void gnrc_rpl_netstats_tx_DAO(netstats_rpl_t *netstats, size_t len, int cast)
{
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dao_tx_mcast_count++;
        netstats->dao_tx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dao_tx_ucast_count++;
        netstats->dao_tx_ucast_bytes += len;
    }
}

/**
 * @brief   Increase statistics for received DAO-ACK
 *
 * @param[in]   netstats    Pointer to netstats_rpl_t
 * @param[in]   len         Length in bytes of an ICMPv6 packet to add to statistics
 * @param[in]   cast        GNRC_RPL_NETSTATS_MULTICAST or GNRC_RPL_NETSTATS_UNICAST
 */
static inline void gnrc_rpl_netstats_rx_DAO_ACK(netstats_rpl_t *netstats, size_t len, int cast)
{
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dao_ack_rx_mcast_count++;
        netstats->dao_ack_rx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dao_ack_rx_ucast_count++;
        netstats->dao_ack_rx_ucast_bytes += len;
    }
}

/**
 * @brief   Increase statistics for sent DAO-ACK
 *
 * @param[in]   netstats    Pointer to netstats_rpl_t
 * @param[in]   len         Length in bytes of an ICMPv6 packet to add to statistics
 * @param[in]   cast        GNRC_RPL_NETSTATS_MULTICAST or GNRC_RPL_NETSTATS_UNICAST
 */
static inline void gnrc_rpl_netstats_tx_DAO_ACK(netstats_rpl_t *netstats, size_t len, int cast)
{
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dao_ack_tx_mcast_count++;
        netstats->dao_ack_tx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dao_ack_tx_ucast_count++;
        netstats->dao_ack_tx_ucast_bytes += len;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* RPL_NETSTATS_H */
/** @} */
