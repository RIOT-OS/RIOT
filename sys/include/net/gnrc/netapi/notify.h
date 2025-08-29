/*
 * Copyright (C) 2025 Elena Frank <elena.frank@tu-dresden.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_gnrc_netapi_notify  Notification events for network APIs.
 * @ingroup     net_gnrc
 * @brief       Network event notification types.
 * @{
 *
 * @file
 * @brief   Network event notification type definitions.
 *
 * @details The purpose of this API is to allow lower layers in the network
 *          stack to inform higher layers of general, protocol-independent
 *          network events.
 *          It can be used through the @ref net_gnrc_netapi to notify any
 *          other network interfaces that have registered themselves through
 *          @ref net_gnrc_netreg for the corresponding @ref net_gnrc_nettype.
 *
 * @note    Notification events are associated with type-dependent event data.
 *          Receivers should not access the data directly, but instead use the
 *          the dedicated API functions to copy the data from the sender's.
 *          If the data is read manually, the receiver is responsible for
 *          calling @ref gnrc_netapi_notify_ack to unblock the sender.
 *
 * @author  Elena Frank <elena.frank@tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/ipv6/addr.h"
#include "cond.h"
#include "mutex.h"

/**
 * @brief   Definition of notification event types in the network stack.
 *
 * @note    Expand at will.
 *          If event data is associated with the type, a helper function must also
 *          be added for parsing the data and calling @ref gnrc_netapi_notify_ack.
 */
typedef enum {
    NETAPI_NOTIFY_L2_CONNECTED,     /**< Connection established on layer 2. */
    NETAPI_NOTIFY_L2_DISCONNECTED,  /**< Connection closed on layer 2. */
    NETAPI_NOTIFY_L3_DISCOVERED,    /**< Discovered node on the network layer. */
    NETAPI_NOTIFY_L3_UNREACHABLE,   /**< Node became unreachable on the network layer. */
} netapi_notify_t;

/**
 * @brief   Data structure to acknowledge netapi notification events.
 */
typedef struct {
    int counter;        /**< ACK counter */
    cond_t cond;        /**< condition variable to signal change in count */
    mutex_t lock;       /**< lock for counter */
} gnrc_netapi_notify_ack_t;

/**
 * @brief   Data structure to be sent for netapi notification events.
 */
typedef struct {
    netapi_notify_t event;          /**< the type of event */
    void *_data;                    /**< associated event data. */
    uint16_t _data_len;             /**< size of the event data */
    gnrc_netapi_notify_ack_t *ack;  /**< acknowledge event */
} gnrc_netapi_notify_t;

/**
 * @brief   L2 connection event data associated with @ref NETAPI_NOTIFY_L2_CONNECTED or
 *          @ref NETAPI_NOTIFY_L2_DISCONNECTED events.
 */
typedef struct {
    uint8_t *l2addr;            /**< L2 address of the node */
    uint8_t l2addr_len;         /**< length of L2 address in byte */
    kernel_pid_t if_pid;        /**< PID of network interface */
} netapi_notify_l2_connection_t;

/**
 * @brief   Acknowledge that a notify event was received and its data read.
 *
 * @param[in] ack           Pointer to the event's acknowledgment structure.
 */
static inline void gnrc_netapi_notify_ack(gnrc_netapi_notify_ack_t *ack)
{
    mutex_lock(&ack->lock);
    ack->counter++;
    /* Signal that the counter changed. */
    cond_signal(&ack->cond);
    mutex_unlock(&ack->lock);
}

/**
 * @brief   Parse the connection event data associated with @ref NETAPI_NOTIFY_L2_CONNECTED
 *          and @ref NETAPI_NOTIFY_L2_DISCONNECTED events.
 *
 * @note    This will call @ref gnrc_netapi_notify_ack.
 *
 * @param[in] notify        Pointer to the received notify event.
 * @param[out] data         Connection data received in the @p notify event.
 *                          data.l2addr_len should be set to the size of the l2addr buffer.
 *
 * @retval                  sizeof(netapi_notify_l2_connection_t) on success.
 * @retval                  -EINVAL if @p notify is of a wrong @ref netapi_notify_t type.
 * @retval                  -ENOBUFS if the length of l2addr in @p data is smaller than the
 *                          received l2addr.
 */
uint8_t gnrc_netapi_notify_copy_l2_connection_data(gnrc_netapi_notify_t *notify,
                                                   netapi_notify_l2_connection_t *data);
/**
 * @brief   Parse the ipv6 address associated with @ref NETAPI_NOTIFY_L3_DISCOVERED and
 *          @ref NETAPI_NOTIFY_L3_UNREACHABLE events.
 *
 * @note    This will call @ref gnrc_netapi_notify_ack.
 *
 * @param[in] notify        Pointer to the received notify event.
 * @param[out] addr         IPv6 address of the remote.
 *
 * @retval                  sizeof(ipv6_addr_t) on success.
 * @retval                  -EINVAL if @p notify is of a wrong @ref netapi_notify_t type.
 */
int gnrc_netapi_notify_copy_l3_address(gnrc_netapi_notify_t *notify, ipv6_addr_t *addr);

#ifdef __cplusplus
}
#endif

/** @} */
