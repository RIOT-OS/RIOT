/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_GNRC_DHCPV6_CLIENT_SIMPLE_PD_H
#define NET_GNRC_DHCPV6_CLIENT_SIMPLE_PD_H
/**
 * @defgroup    net_dhcpv6_client_simple_pd     gnrc_dhcpv6_client_simple_pd: DHCPv6 client for
 *                                              simple prefix delegation
 * @ingroup     net_dhcpv6_client
 * @brief       DHCPv6 client bootstrapping for prefix deligation with routers &
 *              6LoWPAN border routers
 * @{
 *
 * @file
 * @brief   DHCPv6 client for simple prefix delegation definitions
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

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
 * @brief   Interface type of the upstream interface
 *
 * See @ref netdev_type_t for possible values
 */
#ifndef CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM_TYPE
#define CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM_TYPE NETDEV_ANY
#endif

/**
 * @brief   Interface index of the upstream interface
 *
 * If there are multiple interfaces of the same type, set this to select
 * which one to use for the upstream.
 */
#ifndef CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM_IDX
#define CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM_IDX (0)
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
 * @brief   Initializes the DHCPv6 client for simple prefix delegation
 *
 * @note    Called by `auto_init` when included
 */
void gnrc_dhcpv6_client_simple_pd_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_DHCPV6_CLIENT_SIMPLE_PD_H */
