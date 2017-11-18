/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_ipv6_nib
 * @{
 *
 * @file
 * @brief   Definitions related to 6Lo router (6LR) functionality of the NIB
 * @see     @ref GNRC_IPV6_NIB_CONF_6LR
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef PRIV_NIB_6LR_H
#define PRIV_NIB_6LR_H


#include "net/gnrc/ipv6/nib/conf.h"
#include "net/ndp.h"
#include "net/sixlowpan/nd.h"

#include "_nib-arsm.h"
#include "_nib-6ln.h"
#include "_nib-internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#if GNRC_IPV6_NIB_CONF_6LR || defined(DOXYGEN)
/**
 * @brief   Checks if interface represents a 6LR
 *
 * @todo    Use corresponding function in `gnrc_netif2` instead.
 *
 * @param[in] netif A network interface.
 *
 * @return  true, when the @p netif represents a 6LR.
 * @return  false, when the @p netif does not represent a 6LR.
 */
static inline bool _is_6lr(const gnrc_ipv6_netif_t *netif)
{
    return _is_6ln(netif) && (netif->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER);
}

/**
 * @brief   Gets address registration state of a neighbor
 *
 * @param[in] entry Neighbor cache entry representing the neighbor.
 *
 * @return  Address registration state of the @p entry.
 */
static inline uint16_t _get_ar_state(const _nib_onl_entry_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_NC_INFO_AR_STATE_MASK);
}

/**
 * @brief   Sets address registration state of a neighbor
 *
 * @param[in] entry Neighbor cache entry representing the neighbor.
 * @param[in] state Address registration state for the neighbor.
 */
static inline void _set_ar_state(_nib_onl_entry_t *entry, uint16_t state)
{
    entry->info &= ~GNRC_IPV6_NIB_NC_INFO_AR_STATE_MASK;
    entry->info |= state;
}

/**
 * @brief   Checks if the received message is a router solicitation and
 *          the interface represents a 6Lo router
 *
 * @see [RFC 6775](https://tools.ietf.org/html/rfc6775#section-6.3)
 *
 * @param[in] netif     A network interface.
 * @param[in] icmpv6    An ICMPv6 message.
 */
static inline bool _rtr_sol_on_6lr(const gnrc_ipv6_netif_t *netif,
                                   const icmpv6_hdr_t *icmpv6)
{
    return _is_6lr(netif) && (icmpv6->type == ICMPV6_RTR_SOL);
}

/**
 * @brief   Registers an address to the (upstream; in case of multihop DAD)
 *          router
 *
 * @param[in] iface     The interface the ARO-carrying NS came over.
 * @param[in] ipv6      The IPv6 header of the message carrying the ARO.
 * @param[in] icmpv6    The neighbor solicitation carrying the ARO
 *                      (handed over as @ref icmpv6_hdr_t, since it is just
 *                      handed to the SL2AO handler function).
 * @param[in] aro       ARO that carries the address registration information.
 * @param[in] sl2ao     SL2AO associated with the ARO.
 *
 * @return  registration status of the address (including
 *          @ref _ADDR_REG_STATUS_TENTATIVE and @ref _ADDR_REG_STATUS_IGNORE).
 */
uint8_t _reg_addr_upstream(kernel_pid_t iface, const ipv6_hdr_t *ipv6,
                           const icmpv6_hdr_t *icmpv6,
                           const sixlowpan_nd_opt_ar_t *aro,
                           const ndp_opt_t *sl2ao);


/**
 * @brief   Handles and copies ARO from NS to NA
 *
 * @param[in] iface     The interface the ARO-carrying NS came over.
 * @param[in] ipv6      The IPv6 header of the message carrying the original
 *                      ARO.
 * @param[in] nbr_sol   The neighbor solicitation carrying the original ARO
 *                      (handed over as @ref icmpv6_hdr_t, since it is just
 *                      handed to @ref _handle_aro()).
 * @param[in] aro       The original ARO
 * @param[in] sl2ao     SL2AO associated with the ARO.
 *
 * @return  registration status of the address (including
 *          @ref _ADDR_REG_STATUS_TENTATIVE and @ref _ADDR_REG_STATUS_IGNORE).
 */
gnrc_pktsnip_t *_copy_and_handle_aro(kernel_pid_t iface, const ipv6_hdr_t *ipv6,
                                     const ndp_nbr_sol_t *nbr_sol,
                                     const sixlowpan_nd_opt_ar_t *aro,
                                     const ndp_opt_t *sl2ao);
#else   /* GNRC_IPV6_NIB_CONF_6LR || defined(DOXYGEN) */
#define _is_6lr(netif)                  (false)
#define _rtr_sol_on_6lr(netif, icmpv6)  (false)
#define _get_ar_state(nbr)              (_ADDR_REG_STATUS_IGNORE)
#define _set_ar_state(nbr, state)       (void)nbr; (void)state
#define _copy_and_handle_aro(iface, ipv6, icmpv6, aro, sl2ao) \
                                        (NULL)
/* _reg_addr_upstream() doesn't make sense without 6LR so don't even use it
 * => throw error in case it is compiled in => don't define it here as NOP macro
 */
#endif  /* GNRC_IPV6_NIB_CONF_6LR || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

#endif /* PRIV_NIB_6LR_H */
/** @} */
