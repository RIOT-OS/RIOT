/*
 * Copyright (C) 2025 Elena Frank <elena.frank@tu-dresden.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sched.h"

/**
 * @brief   Definition of notification event types in the network stack.
 *
 * @note    Expand at will.
 */
typedef enum {
    NETNOTIFY_L2_CONNECTED,
    NETNOTIFY_L2_DISCONNECTED,
    NETNOTIFY_L3_DISCOVERED,
    NETNOTIFY_L3_UNREACHABLE,
} netnotify_t;

/**
 * @brief   L2 (dis-)connection event data.
 */
typedef struct netnotify_l2_connection {
    uint8_t *l2addr;            /**< L2 address of the node */
    uint8_t l2addr_len;         /**< length of L2 address in byte */
    kernel_pid_t if_pid;        /**< PID of network interface */
} netnotify_l2_connection_t;


#ifdef __cplusplus
}
#endif

/** @} */
