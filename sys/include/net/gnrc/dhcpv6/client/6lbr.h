/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_dhcpv6_client_6lbr  DHCPv6 client for 6LoWPAN border routers
 * @ingroup     net_dhcpv6_client
 * @brief       DHCPv6 client bootstrapping for 6LoWPAN border routers
 * @{
 *
 * @file
 * @brief   DHCPv6 client on 6LoWPAN border router definitions
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_DHCPV6_CLIENT_6LBR_H
#define NET_GNRC_DHCPV6_CLIENT_6LBR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Identifier of the upstream interface
 *
 * Leave 0 (default) to let the client pick the first non-6LoWPAN interface it
 * finds
 */
#ifndef CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM
#define CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM     (0)
#endif

/**
 * @brief   6LoWPAN compression context lifetime for configured prefixes in
 *          minutes
 *
 * Must be between 1 and 255
 *
 * @see     [RFC 6775, section 4.2](https://tools.ietf.org/html/rfc6775#section-4.2)
 */
#ifndef CONFIG_GNRC_DHCPV6_CLIENT_6LBR_6LO_CTX_MIN
#define CONFIG_GNRC_DHCPV6_CLIENT_6LBR_6LO_CTX_MIN  (60U)
#endif

/**
 * @brief   Use static routes to upstream router
 *
 * If set the border router will be configured to have a default route via
 * `fe80::1`. The link-local address `fe80::2` will be added so that the
 * upstream router can set a static route for the delegated prefix via that
 * address. It is recommended to increase at least @ref
 * CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF to that end.
 */
#ifdef DOXYGEN
#define CONFIG_GNRC_DHCPV6_CLIENT_6LBR_STATIC_ROUTE
#endif

/**
 * @brief   Initializes the DHCPv6 client for 6LoWPAN border router
 *
 * @note    Called by `auto_init` when included
 */
void gnrc_dhcpv6_client_6lbr_init(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_DHCPV6_CLIENT_6LBR_H */
/** @} */
