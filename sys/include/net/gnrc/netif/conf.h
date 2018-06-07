/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief   Configuration macros for @ref net_gnrc_netif
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF_CONF_H
#define NET_GNRC_NETIF_CONF_H

#include "net/ieee802154.h"
#include "net/ethernet/hdr.h"
#include "net/gnrc/ipv6/nib/conf.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of network interfaces
 *
 * @note    Intentionally not calling it `GNRC_NETIF_NUMOF` to not require
 *          rewrites throughout the stack.
 */
#ifndef GNRC_NETIF_NUMOF
#define GNRC_NETIF_NUMOF            (1)
#endif

/**
 * @brief   Default priority for network interface threads
 */
#ifndef GNRC_NETIF_PRIO
#define GNRC_NETIF_PRIO            (THREAD_PRIORITY_MAIN - 5)
#endif

/**
 * @brief   Number of multicast addresses needed for @ref net_gnrc_rpl "RPL".
 *
 * @note    Used for calculation of @ref GNRC_NETIF_IPV6_GROUPS_NUMOF
 */
#ifdef MODULE_GNRC_RPL
#define GNRC_NETIF_RPL_ADDR        (1)
#else
#define GNRC_NETIF_RPL_ADDR        (0)
#endif

/**
 * @brief   Number of multicast addresses needed for a @ref net_gnrc_ipv6 "IPv6"
 *          router
 *
 * @note    Used for calculation of @ref GNRC_NETIF_IPV6_GROUPS_NUMOF
 */
#if GNRC_IPV6_NIB_CONF_ROUTER
#define GNRC_NETIF_IPV6_RTR_ADDR   (1)
#else
#define GNRC_NETIF_IPV6_RTR_ADDR   (0)
#endif

/**
 * @brief   Maximum number of unicast and anycast addresses per interface
 *
 * @note    If you change this, please make sure that
 *          @ref GNRC_NETIF_IPV6_GROUPS_NUMOF is also large enough to fit the
 *          addresses' solicited nodes multicast addresses.
 *
 * Default: 2 (1 link-local + 1 global address)
 */
#ifndef GNRC_NETIF_IPV6_ADDRS_NUMOF
#define GNRC_NETIF_IPV6_ADDRS_NUMOF    (2)
#endif

/**
 * @brief   Maximum number of multicast groups per interface
 *
 * Default: 3 (all-nodes + solicited-nodes of link-local and global unicast
 * address) + @ref GNRC_NETIF_RPL_ADDR + @ref GNRC_NETIF_IPV6_RTR_ADDR
 */
#ifndef GNRC_NETIF_IPV6_GROUPS_NUMOF
#define GNRC_NETIF_IPV6_GROUPS_NUMOF   (GNRC_NETIF_IPV6_ADDRS_NUMOF + \
                                        GNRC_NETIF_RPL_ADDR + \
                                        GNRC_NETIF_IPV6_RTR_ADDR + 1)
#endif

/**
 * @brief   Maximum length of the link-layer address.
 *
 * The value for the maximum length of a link-layer address is dependent
 * on the @ref drivers_netdev_api "netdev" adapters compiled in:
 *  - Setting it via `CFLAGS` has the most precedence.
 *  - The default is 8.
 *  - 1, if only @ref drivers_cc110x devices are compiled in.
 *  - @ref ETHERNET_ADDR_LEN, if additionally (or only) ethernet devices are
 *    compiled in.
 *  - @ref IEEE802154_LONG_ADDRESS_LEN, if additionally (or only) IEEE802.15.4
 *    devices are compiled in.
 *
 * @note Implementers note: From longest to shortest extend, if new link-layer
 *       address types are included
 */
#ifndef GNRC_NETIF_L2ADDR_MAXLEN
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
#define GNRC_NETIF_L2ADDR_MAXLEN   (IEEE802154_LONG_ADDRESS_LEN)
#elif   MODULE_NETDEV_ETH
#define GNRC_NETIF_L2ADDR_MAXLEN   (ETHERNET_ADDR_LEN)
#elif   MODULE_CC110X
#define GNRC_NETIF_L2ADDR_MAXLEN   (1U)
#else
#define GNRC_NETIF_L2ADDR_MAXLEN   (GNRC_IPV6_NIB_L2ADDR_MAX_LEN)
#endif
#endif

#ifndef GNRC_NETIF_DEFAULT_HL
#define GNRC_NETIF_DEFAULT_HL      (64U)   /**< default hop limit */
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_CONF_H */
/** @} */
