/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef NET_DHCPV6_RELAY_H
#define NET_DHCPV6_RELAY_H

#include <stdint.h>

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void dhcpv6_relay_auto_init(void);
void dhcpv6_relay_init(event_queue_t *eq, uint16_t listen_netif,
                       uint16_t fwd_netif);

#ifdef __cplusplus
}
#endif

#endif /* NET_DHCPV6_RELAY_H */
/** @} */
