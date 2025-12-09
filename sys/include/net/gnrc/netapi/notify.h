/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_gnrc_netapi_notify  Notification events for network APIs.
 * @ingroup     net_gnrc_netapi
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

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "net/gnrc/netif/conf.h"
#include "sema_inv.h"

/**
 * @brief   Definition of notification event types in the network stack.
 *
 * @note    Expand at will.
 *          If event data is associated with the type, a helper function must also
 *          be added for parsing the data and calling @ref gnrc_netapi_notify_ack.
 */
typedef enum {
    NETAPI_NOTIFY_L2_NEIGH_CONNECTED,       /**< Connection established on layer 2. */
    NETAPI_NOTIFY_L2_NEIGH_DISCONNECTED,    /**< Connection closed on layer 2. */
    NETAPI_NOTIFY_L3_DISCOVERED,            /**< Discovered node on the network layer. */
    NETAPI_NOTIFY_L3_UNREACHABLE,           /**< Node became unreachable on the network layer. */
} netapi_notify_t;

/**
 * @brief   Data structure to be sent for netapi notification events.
 */
typedef struct {
    netapi_notify_t event;          /**< the type of event */
    void *_data;                    /**< associated event data. */
    uint16_t _data_len;             /**< size of the event data */
    sema_inv_t ack;                 /**< inverse semaphore for collecting ack's */
} gnrc_netapi_notify_t;

/**
 * @brief   L2 connection event data associated with @ref NETAPI_NOTIFY_L2_NEIGH_CONNECTED or
 *          @ref NETAPI_NOTIFY_L2_NEIGH_DISCONNECTED events.
 */
typedef struct {
    uint8_t l2addr[GNRC_NETIF_L2ADDR_MAXLEN];   /**< L2 address of the node */
    uint8_t l2addr_len;                         /**< length of L2 address in byte */
    kernel_pid_t if_pid;                        /**< PID of network interface */
} netapi_notify_l2_connection_t;

/**
 * @brief   Acknowledge that a notify event was received and its data read.
 *
 * @param[in] ack           Pointer to semaphore that is used to collect ack's.
 */
static inline void gnrc_netapi_notify_ack(sema_inv_t *ack)
{
    sema_inv_post(ack);
}

/**
 * @brief   Copy the connection event data associated with a @ref gnrc_netapi_notify_t event.
 *
 * @note    This will call @ref gnrc_netapi_notify_ack.
 *
 * @param[in] notify        Pointer to the received notify event.
 * @param[in] data_len      Size of the expected data type.
 * @param[out] data         Connection data received in the @p notify event.
 *
 * @retval                  Size of the data type on success.
 * @retval                  -EINVAL if the data in @p notify is invalid or doesn't match the expected
 *                          data length.
 */
int gnrc_netapi_notify_copy_event_data(gnrc_netapi_notify_t *notify, uint8_t data_len, void *data);

#ifdef __cplusplus
}
#endif

/** @} */
