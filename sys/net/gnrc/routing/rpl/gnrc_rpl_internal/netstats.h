/*
 * Copyright (C) 2016 Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     net_gnrc_rpl
 * @{
 *
 * @file
 * @brief       RPL control message statistics functions
 *
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */


#ifdef __cplusplus
extern "C" {
#endif

#include "irq.h"
#include "net/rpl/rpl_netstats.h"

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
    /* other threads (e.g. the shell thread via the rpl shell command) read
     * out the data, so we have to sync accesses */
    unsigned irq_state = irq_disable();
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dio.rx_mcast_count++;
        netstats->dio.rx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dio.rx_ucast_count++;
        netstats->dio.rx_ucast_bytes += len;
    }
    irq_restore(irq_state);
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
    /* other threads (e.g. the shell thread via the rpl shell command) read
     * out the data, so we have to sync accesses */
    unsigned irq_state = irq_disable();
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dio.tx_mcast_count++;
        netstats->dio.tx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dio.tx_ucast_count++;
        netstats->dio.tx_ucast_bytes += len;
    }
    irq_restore(irq_state);
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
    /* other threads (e.g. the shell thread via the rpl shell command) read
     * out the data, so we have to sync accesses */
    unsigned irq_state = irq_disable();
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dis.rx_mcast_count++;
        netstats->dis.rx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dis.rx_ucast_count++;
        netstats->dis.rx_ucast_bytes += len;
    }
    irq_restore(irq_state);
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
    /* other threads (e.g. the shell thread via the rpl shell command) read
     * out the data, so we have to sync accesses */
    unsigned irq_state = irq_disable();
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dis.tx_mcast_count++;
        netstats->dis.tx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dis.tx_ucast_count++;
        netstats->dis.tx_ucast_bytes += len;
    }
    irq_restore(irq_state);
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
    /* other threads (e.g. the shell thread via the rpl shell command) read
     * out the data, so we have to sync accesses */
    unsigned irq_state = irq_disable();
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dao.rx_mcast_count++;
        netstats->dao.rx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dao.rx_ucast_count++;
        netstats->dao.rx_ucast_bytes += len;
    }
    irq_restore(irq_state);
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
    /* other threads (e.g. the shell thread via the rpl shell command) read
     * out the data, so we have to sync accesses */
    unsigned irq_state = irq_disable();
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dao.tx_mcast_count++;
        netstats->dao.tx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dao.tx_ucast_count++;
        netstats->dao.tx_ucast_bytes += len;
    }
    irq_restore(irq_state);
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
    /* other threads (e.g. the shell thread via the rpl shell command) read
     * out the data, so we have to sync accesses */
    unsigned irq_state = irq_disable();
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dao_ack.rx_mcast_count++;
        netstats->dao_ack.rx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dao_ack.rx_ucast_count++;
        netstats->dao_ack.rx_ucast_bytes += len;
    }
    irq_restore(irq_state);
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
    /* other threads (e.g. the shell thread via the rpl shell command) read
     * out the data, so we have to sync accesses */
    unsigned irq_state = irq_disable();
    if (cast == GNRC_RPL_NETSTATS_MULTICAST) {
        netstats->dao_ack.tx_mcast_count++;
        netstats->dao_ack.tx_mcast_bytes += len;
    }
    else if (cast == GNRC_RPL_NETSTATS_UNICAST) {
        netstats->dao_ack.tx_ucast_count++;
        netstats->dao_ack.tx_ucast_bytes += len;
    }
    irq_restore(irq_state);
}

#ifdef __cplusplus
}
#endif

/** @} */
