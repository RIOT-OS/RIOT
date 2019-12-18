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
 * @brief   IPv4 defintions for @ref net_gnrc_netif
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF_IPV4_H
#define NET_GNRC_NETIF_IPV4_H

#include <assert.h>

#include "evtimer_msg.h"
#include "net/ipv4/addr.h"

#include "net/gnrc/netapi.h"
#include "net/gnrc/netif/conf.h"
#ifdef MODULE_NETSTATS_IPV4
#include "net/netstats.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    IPv6 unicast and anycast address flags
 * @anchor  net_gnrc_netif_ipv4_addrs_flags
 * @{
 */
/**
 * @brief   Mask for the address' state
 */
#define GNRC_NETIF_IPV4_ADDRS_FLAGS_STATE_MASK             (0x1fU)

/**
 * @brief   Tentative states (with encoded DAD retransmissions)
 *
 * The retransmissions of DAD transmits can be decoded from this state by
 * applying it as a mask to the [flags](gnrc_netif_ipv4_t::addrs_flags) of the
 * address.
 */
#define GNRC_NETIF_IPV4_ADDRS_FLAGS_STATE_TENTATIVE        (0x07U)

/**
 * @brief   Deprecated address state (still valid, but not preferred)
 */
#define GNRC_NETIF_IPV4_ADDRS_FLAGS_STATE_DEPRECATED       (0x08U)

/**
 * @brief   Valid address state
 */
#define GNRC_NETIF_IPV4_ADDRS_FLAGS_STATE_VALID            (0x10U)

/**
 * @brief   Address is an anycast address
 */
#define GNRC_NETIF_IPV4_ADDRS_FLAGS_ANYCAST                (0x20U)
/** @} */

/**
 * @brief   IPv6 component for @ref gnrc_netif_t
 *
 * @note only available with @ref net_gnrc_ipv6.
 */
typedef struct {
    /**
     * @brief   Flags for gnrc_netif_t::ipv4_addrs
     *
     * @see net_gnrc_netif_ipv4_addrs_flags
     *
     * @note    Only available with module @ref net_gnrc_ipv4 "gnrc_ipv4".
     */
    uint8_t addrs_flags[GNRC_NETIF_IPV4_ADDRS_NUMOF];

    /**
     * @brief   IPv6 unicast and anycast addresses of the interface
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    ipv4_addr_t addrs[GNRC_NETIF_IPV4_ADDRS_NUMOF];
    ipv4_addr_t addrs_mask[GNRC_NETIF_IPV4_ADDRS_NUMOF];

    /**
     * @brief   IPv6 multicast groups of the interface
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    ipv4_addr_t groups[GNRC_NETIF_IPV4_GROUPS_NUMOF];
#ifdef MODULE_NETSTATS_IPV4
    /**
     * @brief IPv6 packet statistics
     *
     * @note    Only available with module `netstats_ipv6`.
     */
    netstats_t stats;
#endif


    /**
     * @brief   IPv6 auto-address configuration mode
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6"
     */
    uint8_t aac_mode;

    /**
     * @brief   Maximum transmission unit (MTU) for IPv6 packets
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    uint16_t mtu;
} gnrc_netif_ipv4_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_IPV6_H */
/** @} */
