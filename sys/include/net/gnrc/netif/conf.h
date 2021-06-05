/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_netif_conf GNRC network interface configurations
 * @ingroup     net_gnrc_netif
 * @ingroup     net_gnrc_conf
 * @{
 *
 * @file
 * @brief   Configuration macros for @ref net_gnrc_netif
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF_CONF_H
#define NET_GNRC_NETIF_CONF_H

#include <kernel_defines.h>

#include "net/ieee802154.h"
#include "net/ethernet/hdr.h"
#include "net/gnrc/ipv6/nib/conf.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default priority for network interface threads
 */
#ifndef GNRC_NETIF_PRIO
#define GNRC_NETIF_PRIO            (THREAD_PRIORITY_MAIN - 5)
#endif

/**
 * @brief       Default message queue size for network interface threads (as
 *              exponent of 2^n).
 *
 *              As the queue size ALWAYS needs to be power of two, this option
 *              represents the exponent of 2^n, which will be used as the size
 *              of the queue.
 *
 * @attention   This has influence on the used stack memory of the thread, so
 *              the thread's stack size might need to be adapted if this is
 *              changed.
 */
#ifndef CONFIG_GNRC_NETIF_MSG_QUEUE_SIZE_EXP
#define CONFIG_GNRC_NETIF_MSG_QUEUE_SIZE_EXP  (4U)
#endif

/**
 * @brief       Packet queue pool size for all network interfaces
 *
 * @note        With @ref net_gnrc_sixlowpan_frag the queue should fit at least
 *              all fragments of the minimum MTU.
 * @see         net_gnrc_netif_pktq
 */
#ifndef CONFIG_GNRC_NETIF_PKTQ_POOL_SIZE
#define CONFIG_GNRC_NETIF_PKTQ_POOL_SIZE      (16U)
#endif

/**
 * @brief       Time in microseconds for when to try send a queued packet at the
 *              latest
 *
 * Set to -1 to deactivate dequeing by timer. For this it has to be ensured that
 * none of the notifications by the driver are missed!
 *
 * @see         net_gnrc_netif_pktq
 */
#ifndef CONFIG_GNRC_NETIF_PKTQ_TIMER_US
#define CONFIG_GNRC_NETIF_PKTQ_TIMER_US       (5000U)
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
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
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
#ifndef CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF
#define CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF    (2)
#endif

/**
 * @brief   Maximum number of multicast groups per interface
 *
 * Default: 3 (all-nodes + solicited-nodes of link-local and global unicast
 * address) + @ref GNRC_NETIF_RPL_ADDR + @ref GNRC_NETIF_IPV6_RTR_ADDR
 */
#ifndef GNRC_NETIF_IPV6_GROUPS_NUMOF
#define GNRC_NETIF_IPV6_GROUPS_NUMOF   (CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF + \
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
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) || defined(MODULE_SLIPDEV_L2ADDR)
#define GNRC_NETIF_L2ADDR_MAXLEN   (IEEE802154_LONG_ADDRESS_LEN)
#elif   MODULE_NETDEV_ETH
#define GNRC_NETIF_L2ADDR_MAXLEN   (ETHERNET_ADDR_LEN)
#elif   MODULE_NRF24L01P
#define GNRC_NETIF_L2ADDR_MAXLEN   (5U)
#elif   MODULE_CC110X
#define GNRC_NETIF_L2ADDR_MAXLEN   (1U)
#else
#define GNRC_NETIF_L2ADDR_MAXLEN   (CONFIG_GNRC_IPV6_NIB_L2ADDR_MAX_LEN)
#endif
#endif

#ifndef CONFIG_GNRC_NETIF_DEFAULT_HL
#define CONFIG_GNRC_NETIF_DEFAULT_HL      (64U)   /**< default hop limit */
#endif

/**
 * @brief   Minimum wait time in microseconds after a send operation
 *
 * @experimental
 *
 * This is purely meant as a debugging feature to slow down a radios sending.
 */
#ifndef CONFIG_GNRC_NETIF_MIN_WAIT_AFTER_SEND_US
#define CONFIG_GNRC_NETIF_MIN_WAIT_AFTER_SEND_US   (0U)
#endif
/** @} */

/**
 * @brief   Message queue size for network interface threads
 */
#ifndef GNRC_NETIF_MSG_QUEUE_SIZE
#define GNRC_NETIF_MSG_QUEUE_SIZE   (1 << CONFIG_GNRC_NETIF_MSG_QUEUE_SIZE_EXP)
#endif

/**
 * @brief   Enable the usage of non standard MTU for 6LoWPAN network interfaces
 *
 * @experimental
 *
 * This feature is non compliant with RFC 4944 and RFC 7668 and might not be
 * supported by other implementations.
 */
#ifndef CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU
#define CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU 0
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_CONF_H */
