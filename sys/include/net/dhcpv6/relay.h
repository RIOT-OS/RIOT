/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup net_dhcpv6_relay   DHCPv6 relay agent
 * @ingroup  net_dhcpv6
 * @brief    DHCPv6 relay agent implementation
 * @{
 *
 * @file
 * @brief   DHCPv6 client definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup net_dhcpv6_conf
 * @{
 */
/**
 * @brief   Maximum hop count in a relay-forward message (HOP_COUNT_LIMIT)
 *
 * @see [RFC 8415, section 7.6](https://tools.ietf.org/html/rfc8415#section-7.6)
 */
#ifndef CONFIG_DHCPV6_RELAY_HOP_LIMIT
#define CONFIG_DHCPV6_RELAY_HOP_LIMIT   (8U)
#endif

#ifndef CONFIG_DHCPV6_RELAY_BUFLEN
#define CONFIG_DHCPV6_RELAY_BUFLEN      (256U)   /**< default length for send and receive buffer */
#endif
/** @} */

/**
 * @brief   Auto-initializes the relay agent in its own thread or event thread
 *          when available
 *
 * @note    Only used with `auto_init_dhcpv6_relay`.
 */
void dhcpv6_relay_auto_init(void);

/**
 * @brief   Initializes the relay agent
 *
 * @pre `event_queue->waiter != NULL` (event queue is initialized)
 *
 * @param[in] event_queue   Event queue to use with the relay agent. Needs to
 *                          be initialized in the handler thread.
 * @param[in] listen_netif  The network interface the relay agent listens on for
 *                          incoming client or relay forward messages from other
 *                          relay agents.
 * @param[in] fwd_netif     The network interface the relay agent relays
 *                          messages upstreams and listens for relay replies
 *                          on.
 */
void dhcpv6_relay_init(event_queue_t *event_queue, uint16_t listen_netif,
                       uint16_t fwd_netif);

#ifdef __cplusplus
}
#endif

/** @} */
