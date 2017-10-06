/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_nib   Neighbor Information Base for IPv6
 * @ingroup     net_gnrc_ipv6
 * @brief       Neighbor Information Base (NIB) for IPv6
 *
 * @todo    Add detailed description
 * @{
 *
 * @file
 * @brief   NIB definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_IPV6_NIB_H
#define NET_GNRC_IPV6_NIB_H

#include "net/gnrc/ipv6/nib/nc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer event message types
 * @anchor  net_gnrc_ipv6_nib_msg
 * @{
 */
/**
 * @brief   (Re-)transmit unicast Neighbor Solicitation event.
 *
 * This message type is for the event of (re-)transmitting of unicast Neighbor
 * Solicitation. The expected message context is a pointer to a valid on-link
 * entry representing the neighbor to which the Neighbor Solicitation is
 * supposed to be sent.
 */
#define GNRC_IPV6_NIB_SND_UC_NS             (0x4fc0U)

/**
 * @brief   (Re-)transmit multicast Neighbor Solicitation event.
 *
 * This message type is for the event of (re-)transmitting of multicast Neighbor
 * Solicitation. The expected message context is a pointer to a valid on-link
 * entry representing the neighbor to which [solicited
 * nodes](https://tools.ietf.org/html/rfc4291#section-2.7.1) group
 * the Neighbor Solicitation is supposed to be sent.
 */
#define GNRC_IPV6_NIB_SND_MC_NS             (0x4fc1U)

/**
 * @brief   Send delayed Neighbor Advertisement event.
 *
 * This message type is for the event of sending delayed Neighbor
 * Advertisements. The expected message context is a pointer to a valid
 * [packet snip](@ref gnrc_pktsnip_t) in *sending order*, representing the
 * Neighbor Advertisement.
 */
#define GNRC_IPV6_NIB_SND_NA                (0x4fc2U)

/**
 * @brief   Search router event.
 *
 * This message type is for the event of searching a (new) router (which
 * implies sending a multicast Router Solicitation). The expected message
 * context is a pointer to a valid interface behind which the router is
 * searched.
 */
#define GNRC_IPV6_NIB_SEARCH_RTR            (0x4fc3U)

/**
 * @brief   Reconfirm router event.
 *
 * This message type is for the event the reconfirmation of a router (which
 * implies sending a unicast Router Solicitation). The expected message context
 * is a pointer to a valid on-link entry representing the router that is to be
 * confirmed.
 */
#define GNRC_IPV6_NIB_RECONFIRM_RTR         (0x4fc4U)

/**
 * @brief   Reply Router Solicitation event.
 *
 * This message type is for the event of the delayed reply to a Router
 * Solicitaion with a Router Advertisement. The expected message context is a
 * pointer to a valid on-link entry representing the neighbor that sent the
 * Router Solicitation.
 *
 * @note    Only handled with @ref GNRC_IPV6_NIB_CONF_ROUTER != 0
 */
#define GNRC_IPV6_NIB_REPLY_RS              (0x4fc5U)

/**
 * @brief   (Re-)transmit multicast Router Advertisement event.
 *
 * This message type is for the event of (Re)transmit Advertisements
 * event. The expected message context is a pointer to a valid interface over
 * which the Router Advertisement will be sent and by which parameters it will
 * be configured.
 *
 * @note    Only handled with @ref GNRC_IPV6_NIB_CONF_ROUTER != 0
 */
#define GNRC_IPV6_NIB_SND_MC_RA             (0x4fc6U)

/**
 * @brief   Reachability timeout event.
 *
 * This message type is for the event of a REACHABLE state timeout.
 * The expected message context is a pointer to a valid on-link entry
 * representing the neighbor cache entry that faces a state change.
 *
 * @note    Only handled with @ref GNRC_IPV6_NIB_CONF_ARSM != 0
 */
#define GNRC_IPV6_NIB_REACH_TIMEOUT         (0x4fc7U)

/**
 * @brief   Delay timeout event.
 *
 * This message type is for the event of the DELAY state timeout.
 * The expected message context is a pointer to a valid on-link entry
 * representing the neighbor cache entry that faces a state change.
 *
 * @note    Only handled with @ref GNRC_IPV6_NIB_CONF_ARSM != 0
 */
#define GNRC_IPV6_NIB_DELAY_TIMEOUT         (0x4fc8U)

/**
 * @brief   Address registration timeout event.
 *
 * This message type is for the event of a 6LoWPAN address registration state
 * timeout. The expected message context is a pointer to a valid on-link entry
 * representing the neighbor which faces a timeout of its address registration.
 *
 * @note    Only handled with @ref GNRC_IPV6_NIB_CONF_6LR != 0
 */
#define GNRC_IPV6_NIB_ADDR_REG_TIMEOUT      (0x4fc9U)

/**
 * @brief   6LoWPAN context timeout event.
 *
 * This message type is for the event of a 6LoWPAN compression context timeout.
 * The expected message context is the compression context's numerical
 * identifier.
 *
 * @note    Only handled with @ref GNRC_IPV6_NIB_CONF_6LN != 0
 */
#define GNRC_IPV6_NIB_6LO_CTX_TIMEOUT       (0x4fcaU)

/**
 * @brief   Authoritative border router timeout event.
 *
 * This message type is for the event of an Authoritative border router timeout.
 * The expected message context is the NIB-internal state of the authoritative
 * border router.
 *
 * @note    Only handled with @ref GNRC_IPV6_NIB_CONF_MULTIHOP_P6C != 0
 */
#define GNRC_IPV6_NIB_ABR_TIMEOUT           (0x4fcbU)

/**
 * @brief   Prefix timeout event.
 *
 * This message type is for the event of a prefix timeout. The expected message
 * context is a valid off-link entry representing the prefix.
 */
#define GNRC_IPV6_NIB_PFX_TIMEOUT           (0x4fccU)

/**
 * @brief   Router timeout event.
 *
 * This message type is for the event of a router timeout. The expected message
 * context is a valid default router entry representing the router.
 */
#define GNRC_IPV6_NIB_RTR_TIMEOUT           (0x4fcdU)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_NIB_H */
/** @} */
