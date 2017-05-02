/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_ipv6_nib
 * @internal
 * @{
 *
 * @file
 * @brief   Definitions related to the address resolution state machine (ARSM)
 *          of the NIB
 * @see     @ref GNRC_IPV6_NIB_CONF_ARSM
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef PRIV_NIB_ARSM_H
#define PRIV_NIB_ARSM_H

#include <stdint.h>

#include "net/gnrc/ipv6/nib/conf.h"
#include "net/ndp.h"
#include "net/icmpv6.h"

#include "_nib-internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Sends neighbor solicitation (including ARO if required)
 *
 * @pre `(tgt != NULL) && !ipv6_addr_is_multicast(tgt)`
 * @pre `(netif != NULL) && (dst != NULL)`
 *
 * @param[in] tgt       The target address of the neighbor solicitation.
 *                      May not be NULL and **MUST NOT** be multicast.
 * @param[in] netif     Interface to send over. May not be NULL.
 * @param[in] src       Source address for the neighbor solicitation. Will be
 *                      chosen from the interface according to @p dst, if NULL.
 * @param[in] dst       Destination address for neighbor solicitation. May not
 *                      be NULL.
 */
void _snd_ns(const ipv6_addr_t *tgt, gnrc_ipv6_netif_t *netif,
             const ipv6_addr_t *src, const ipv6_addr_t *dst);

/**
 * @brief   Sends unicast neighbor solicitation and reset corresponding timer
 *          event
 *
 * @note    Neighbor solicitations are used *by* the ARSM, but also by other
 *          mechanisms (e.g. duplicate address detection 6Lo address
 *          resolution). This is why it is defined here, but not exclusively
 *          available when @ref GNRC_IPV6_NIB_CONF_ARSM is set.
 *
 * @param[in] nbr       Neighbor to send neighbor solicitation to.
 * @param[in] reset     Reset probe counter.
 */
void _snd_uc_ns(_nib_onl_entry_t *nbr, bool reset);

/**
 * @brief   Handles SL2AO
 *
 * @note    This is here (but not only available with
 *          @ref GNRC_IPV6_NIB_CONF_ARSM set) since it is closely related
 *          to the ARSM, but ARSM isn't the only mechanism using it (e.g. the
 *          6Lo address registration uses it).
 *
 * @param[in] iface     Interface the SL2AO was sent over.
 * @param[in] ipv6      IPv6 header of the message carrying the SL2AO.
 * @param[in] icmpv6    ICMPv6 header of the message carrying the SL2AO.
 * @param[in] sl2ao     The SL2AO
 */
void _handle_sl2ao(kernel_pid_t iface, const ipv6_hdr_t *ipv6,
                   const icmpv6_hdr_t *icmpv6, const ndp_opt_t *sl2ao);

#if GNRC_IPV6_NIB_CONF_ARSM || defined(DOXYGEN)
/**
 * @brief   Handler for @ref GNRC_IPV6_NIB_SND_UC_NS and
 *          @ref GNRC_IPV6_NIB_SND_UC_NS event handler
 *
 * @param[in] nbr   Neighbor to send the neighbor solicitation to.
 */
void _handle_snd_ns(_nib_onl_entry_t *nbr);

/**
 * @brief   Handler for @ref GNRC_IPV6_NIB_DELAY_TIMEOUT and
 *          @ref GNRC_IPV6_NIB_REACH_TIMEOUT event handler
 *
 * @param[in] nbr   Neighbor to handle the state timeout for to.
 */
void _handle_state_timeout(_nib_onl_entry_t *nbr);

/**
 * @brief   Probes neighbor with neighbor solicitations
 *
 * @param[in] nbr   Neighbor to probe.
 * @param[in] reset Reset probe counter.
 */
void _probe_nbr(_nib_onl_entry_t *nbr, bool reset);

/**
 * @brief   Handles advertised link-layer information
 *
 * This can either be an TL2AO or for a link-layer without addresses just a
 * neighbor advertisement.
 *
 * @param[in] iface     Interface the link-layer information was advertised
 *                      over.
 * @param[in] nce       Neighbor cache entry that is updated by the advertised
 *                      link-layer information.
 * @param[in] icmpv6    The ICMPv6 message (neighbor advertisement or redirect
 *                      message) that carries the link-layer information.
 * @param[in] tl2ao     The TL2AO carrying the link-layer information. May be
 *                      NULL for link-layers without addresses.
 */
void _handle_adv_l2(kernel_pid_t iface, _nib_onl_entry_t *nce,
                    const icmpv6_hdr_t *icmpv6, const ndp_opt_t *tl2ao);

/**
 * @brief   Sets a neighbor cache entry reachable and starts the required
 *          event timers
 *
 * @param[in] iface Interface to the NCE
 * @param[in] nce   The neighbor cache entry to set reachable
 */
void _set_reachable(unsigned iface, _nib_onl_entry_t *nce);

/**
 * @brief   Initializes interface for address registration state machine
 *
 * @param[in] nib_iface An interface
 */
static inline void _init_iface_arsm(_nib_iface_t *nib_iface)
{
    nib_iface->reach_time_base = NDP_REACH_MS;
    nib_iface->retrans_time = NDP_RETRANS_TIMER_MS;
    _nib_iface_recalc_reach_time(nib_iface);
}

/**
 * @brief   Gets neighbor unreachability state of a neighbor
 *
 * @param[in] entry Neighbor cache entry representing the neighbor.
 *
 * @return  Neighbor unreachability state of the @p entry.
 */
static inline uint16_t _get_nud_state(_nib_onl_entry_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK);
}

/**
 * @brief   Sets neighbor unreachablility state of a neighbor
 *
 * @param[in] entry Neighbor cache entry representing the neighbor.
 * @param[in] state Neighbor unreachability state for the neighbor.
 */
static inline void _set_nud_state(_nib_onl_entry_t *entry, uint16_t state)
{
    entry->info &= ~GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK;
    entry->info |= state;
}

#else   /* GNRC_IPV6_NIB_CONF_ARSM || defined(DOXYGEN) */
#define _handle_snd_ns(ctx)                         (void)ctx
#define _handle_state_timeout(ctx)                  (void)ctx
#define _probe_nbr(nbr, reset)                      (void)nbr; (void)reset
#define _init_iface_arsm(netif)                     (void)netif
#define _handle_adv_l2(netif, nce, icmpv6, tl2ao)   (void)netif; (void)nce; \
                                                    (void)icmpv6; (void)tl2ao
#define _set_reachable(netif, nce)                  (void)netif; (void)nce
#define _init_iface_arsm(netif)                     (void)netif

#define _get_nud_state(entry)         (GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED)
#define _set_nud_state(entry, state)  (void)entry; (void)state
#endif  /* GNRC_IPV6_NIB_CONF_ARSM || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

#endif /* PRIV_NIB_ARSM_H */
/** @} */
