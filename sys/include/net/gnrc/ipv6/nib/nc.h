/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_nib_nc   Neighbor Cache
 * @ingroup     net_gnrc_ipv6_nib
 * @brief       Neighbor cache component of neighbor information base
 * @{
 *
 * @file
 * @brief       Neighbor cache definitions
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_IPV6_NIB_NC_H
#define NET_GNRC_IPV6_NIB_NC_H

#include <stdbool.h>
#include <stdint.h>

#include "net/eui64.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/ipv6/nib/conf.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Values for gnrc_ipv6_nib_nc_t::info
 * @anchor  net_gnrc_ipv6_nib_nc_info
 * @name    Info values
 * @{
 */
/**
 * @brief   Mask for neighbor unreachability detection (NUD) states
 *
 * @see [RFC 4861, section 7.3.2](https://tools.ietf.org/html/rfc4861#section-7.3.2)
 * @see [RFC 7048](https://tools.ietf.org/html/rfc7048)
 */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK            (0x0007)

/**
 * @brief   not managed by NUD
 */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED       (0x0000)

/**
 * @brief   entry is not reachable
 */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE     (0x0001)

/**
 * @brief   address resolution is currently performed
 */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE      (0x0002)

/**
 * @brief   address might not be reachable
 */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE           (0x0003)

/**
 * @brief   NUD will be performed in a moment
 */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY           (0x0004)

/**
 * @brief   NUD is performed
 */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE           (0x0005)

/**
 * @brief   entry is reachable
 */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE       (0x0006)

/**
 * @brief   gnrc_ipv6_nib_t::next_hop is router
 *
 * This flag indicates that gnrc_ipv6_nib_t::next_hop is a router, but it does
 * not necessarily indicate that it is in the default router list! A neighbor
 * that has this flag unset however **must not** appear in the default router
 * list.
 *
 * @see [RFC 4861, Appendix D](https://tools.ietf.org/html/rfc4861#page-91)
 */
#define GNRC_IPV6_NIB_NC_INFO_IS_ROUTER                 (0x0008)

/**
 * @brief   Mask for interface identifier
 */
#define GNRC_IPV6_NIB_NC_INFO_IFACE_MASK                (0x01f0)

/**
 * @brief   Shift position of interface identifier
 */
#define GNRC_IPV6_NIB_NC_INFO_IFACE_POS                 (4)

/**
 * @brief Mask for 6LoWPAN address registration (6Lo-AR) states
 *
 * @see [RFC 6775, section 3.5](https://tools.ietf.org/html/rfc6775#section-3.5)
 */
#define GNRC_IPV6_NIB_NC_INFO_AR_STATE_MASK             (0x0600)

/**
 * @brief   not managed by 6Lo-AR (address can be removed when memory is low
 */
#define GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC               (0x0000)

/**
 * @brief   address registration still pending at upstream router
 */
#define GNRC_IPV6_NIB_NC_INFO_AR_STATE_TENTATIVE        (0x0200)

/**
 * @brief   address is registered
 */
#define GNRC_IPV6_NIB_NC_INFO_AR_STATE_REGISTERED       (0x0600)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_NIB_NC_H */
/** @} */
