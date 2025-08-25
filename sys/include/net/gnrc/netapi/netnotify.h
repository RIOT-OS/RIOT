/*
 * Copyright (C) 2025 Elena Frank <elena.frank@tu-dresden.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_gnrc_netnotify  Notification events for network APIs.
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
 *          the dedicated API functions.
 *          If the data is read directly, the receiver is responsible for setting
 *          the @ref NETNOTIFY_FLAG_ACK flag for the sending thread.
 *
 *
 * @author  Elena Frank <elena.frank@tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/ipv6/addr.h"
#include "sched.h"

/**
 * @brief   Thread flag for acknowledging a received notify even.
 *
 * @note    The flag should only be set after all event data was copied
 *          by receiver and can be freed by the sender.
 */
#define NETNOTIFY_FLAG_ACK         (1u << 10)

/**
 * @brief   Definition of notification event types in the network stack.
 *
 * @note    Expand at will.
 *          If event data is associated with the type, a helper function must also
 *          be added for parsing the data and setting the @ref NETNOTIFY_FLAG_ACK
 *          on the sender.
 */
typedef enum {
    NETNOTIFY_L2_CONNECTED,     /**< Connection established on layer 2. */
    NETNOTIFY_L2_DISCONNECTED,  /**< Connection closed on layer 2. */
    NETNOTIFY_L3_DISCOVERED,    /**< Discovered node on the network layer. */
    NETNOTIFY_L3_UNREACHABLE,   /**< Node became unreachable on the network layer. */
} netnotify_t;

/**
 * @brief   Data structure to be sent for netapi notification events.
 */
typedef struct {
    netnotify_t event;          /**< the type of event */
    void *_data;                /**< associated event data. */
    uint16_t data_len;          /**< size of the event data */
} gnrc_netapi_notify_t;

/**
 * @brief   L2 connection event data associated with @ref NETNOTIFY_L2_CONNECTED or
 *          @ref NETNOTIFY_L2_DISCONNECTED events.
 */
typedef struct {
    uint8_t *l2addr;            /**< L2 address of the node */
    uint8_t l2addr_len;         /**< length of L2 address in byte */
    kernel_pid_t if_pid;        /**< PID of network interface */
} netnotify_l2_connection_t;

/**
 * @brief   Parse the connection event data associated with @ref NETNOTIFY_L2_CONNECTED
 *          and @ref NETNOTIFY_L2_DISCONNECTED events.
 *
 * @note    This will set the @ref NETNOTIFY_FLAG_ACK for the sending thread.
 *
 * @param[in] sender_pid    PID of the netapi sender.
 * @param[in] notify        Pointer to the received notify event.
 * @param[out] l2addr       L2 address of the node on the connection.
 * @param[out] if_pid       PID of the network interface to the node.
 *
 * @return                  Length of @p l2addr on success.
 * @return                  -EINVAL if @p notify is of a wrong @ref netnotify_t type.
 */
uint8_t gnrc_netnotify_get_l2_connection_data(kernel_pid_t sender_pid, gnrc_netapi_notify_t *notify,
                                              uint8_t *l2addr, kernel_pid_t *if_pid);

/**
 * @brief   Parse the ipv6 address associated with @ref NETNOTIFY_L3_DISCOVERED and
 *          @ref NETNOTIFY_L3_UNREACHABLE events.
 *
 * @note    This will set the @ref NETNOTIFY_FLAG_ACK for the sending thread.
 *
 * @param[in] sender_pid    PID of the netapi sender.
 * @param[in] notify        Pointer to the received notify event.
 * @param[out] addr         IPv6 address of the remote.
 *
 * @return                  sizeof(ipv6_addr_t) on success.
 * @return                  -EINVAL if @p notify is of a wrong @ref netnotify_t type.
 */
int gnrc_netnotify_get_l3_address(kernel_pid_t sender_pid, gnrc_netapi_notify_t *notify,
                                  ipv6_addr_t *addr);

#ifdef __cplusplus
}
#endif

/** @} */
