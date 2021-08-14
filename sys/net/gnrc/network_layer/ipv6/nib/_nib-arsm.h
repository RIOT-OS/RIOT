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
 * @see     @ref CONFIG_GNRC_IPV6_NIB_ARSM
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef PRIV_NIB_ARSM_H
#define PRIV_NIB_ARSM_H

#include <kernel_defines.h>
#include <stdint.h>

#include "net/gnrc/ipv6/nib/conf.h"
#include "net/gnrc/netif.h"
#include "net/ndp.h"
#include "net/icmpv6.h"
#include "net/ipv6/hdr.h"

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
 *                      Must not be NULL or a multicast address.
 * @param[in] netif     Interface to send over. Must not be NULL.
 * @param[in] src       Source address for the neighbor solicitation. Will be
 *                      chosen from the interface according to @p dst, if NULL.
 * @param[in] dst       Destination address for neighbor solicitation. Must not
 *                      be NULL.
 */
void _snd_ns(const ipv6_addr_t *tgt, gnrc_netif_t *netif,
             const ipv6_addr_t *src, const ipv6_addr_t *dst);

/**
 * @brief   Sends unicast neighbor solicitation and reset corresponding timer
 *          event
 *
 * @note    Neighbor solicitations are used *by* the ARSM, but also by other
 *          mechanisms (e.g. duplicate address detection 6Lo address
 *          resolution). This is why it is defined here, but not exclusively
 *          available when @ref CONFIG_GNRC_IPV6_NIB_ARSM is set.
 *
 * @param[in] nbr       Neighbor to send neighbor solicitation to.
 * @param[in] reset     Reset probe counter.
 */
void _snd_uc_ns(_nib_onl_entry_t *nbr, bool reset);

/**
 * @brief   Handles SL2AO
 *
 * @note    This is here (but not only available with
 *          @ref CONFIG_GNRC_IPV6_NIB_ARSM set) since it is closely related
 *          to the ARSM, but ARSM isn't the only mechanism using it (e.g. the
 *          6Lo address registration uses it).
 *
 * @param[in] netif     Interface the SL2AO was sent over.
 * @param[in] ipv6      IPv6 header of the message carrying the SL2AO.
 * @param[in] icmpv6    ICMPv6 header of the message carrying the SL2AO.
 * @param[in] sl2ao     The SL2AO
 */
void _handle_sl2ao(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                   const icmpv6_hdr_t *icmpv6, const ndp_opt_t *sl2ao);

/**
 * @brief   Calculates truncated exponential back-off for retransmission timer
 *          for neighbor solicitations based on a randomized factor
 *
 * The truncation is at @ref NDP_MAX_RETRANS_TIMER_MS. as suggested in
 * [RFC 7048, section 3](https://tools.ietf.org/html/rfc7048#section-3).
 *
 * @param[in] ns_sent       Neighbor solicitations sent up until now. Must be
 *                          lesser than or equal to @ref NDP_MAX_NS_NUMOF.
 * @param[in] retrans_timer Currently configured retransmission timer in ms.
 * @param[in] factor        An equally distributed factor between
 *                          @ref NDP_MIN_RANDOM_FACTOR and (exclusive)
 *                          @ref NDP_MAX_RANDOM_FACTOR.
 *
 * @pre (NDP_MIN_RANDOM_FACTOR <= factor < NDP_MAX_RANDOM_FACTOR)
 * @pre (ns_sent <= NDP_MAX_NS_NUMOF)
 *
 * @return  exponential back-off of the retransmission timer
 */
static inline uint32_t _exp_backoff_retrans_timer_factor(uint8_t ns_sent,
                                                         uint32_t retrans_timer,
                                                         uint32_t factor)
{
    assert(NDP_MIN_RANDOM_FACTOR <= factor);
    assert(factor < NDP_MAX_RANDOM_FACTOR);
    assert(ns_sent <= NDP_MAX_NS_NUMOF);
    /* backoff according to  https://tools.ietf.org/html/rfc7048 with
     * BACKOFF_MULTIPLE == 2 */
    uint32_t res = (uint32_t)(((uint64_t)(((uint32_t) 1) << ns_sent) *
                               retrans_timer * factor) / US_PER_MS);
    /* random factors were statically multiplied with 1000 */
    if (res > NDP_MAX_RETRANS_TIMER_MS) {
        res = NDP_MAX_RETRANS_TIMER_MS;
    }
    return res;
}

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM) || defined(DOXYGEN)
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
 * @param[in] netif     Interface the link-layer information was advertised
 *                      over.
 * @param[in] nce       Neighbor cache entry that is updated by the advertised
 *                      link-layer information.
 * @param[in] icmpv6    The ICMPv6 message (neighbor advertisement or redirect
 *                      message) that carries the link-layer information.
 * @param[in] tl2ao     The TL2AO carrying the link-layer information. May be
 *                      NULL for link-layers without addresses.
 */
void _handle_adv_l2(gnrc_netif_t *netif, _nib_onl_entry_t *nce,
                    const icmpv6_hdr_t *icmpv6, const ndp_opt_t *tl2ao);

/**
 * @brief   Recalculates the (randomized) reachable time of on a network
 *          interface.
 *
 * @see [RFC 4861, section 6.3.4](https://tools.ietf.org/html/rfc4861#section-6.3.4)
 *
 * @param[in] netif Interface to set reachable time for.
 */
void _recalc_reach_time(gnrc_netif_ipv6_t *netif);

/**
 * @brief   Sets a neighbor cache entry reachable and starts the required
 *          event timers
 *
 * @param[in] netif Interface to the NCE
 * @param[in] nce   The neighbor cache entry to set reachable
 */
void _set_reachable(gnrc_netif_t *netif, _nib_onl_entry_t *nce);

/**
 * @brief   Initializes interface for address registration state machine
 *
 * @param[in] netif An interface
 */
static inline void _init_iface_arsm(gnrc_netif_t *netif)
{
    netif->ipv6.reach_time_base = NDP_REACH_MS;
    _recalc_reach_time(&netif->ipv6);
}

/**
 * @brief   Gets neighbor unreachability state of a neighbor
 *
 * @param[in] nbr   Neighbor cache entry representing the neighbor.
 *
 * @return  Neighbor unreachability state of the @p nbr.
 */
static inline uint16_t _get_nud_state(_nib_onl_entry_t *nbr)
{
    return (nbr->info & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK);
}

/**
 * @brief   Sets neighbor unreachablility state of a neighbor
 *
 * @param[in] netif The network interface (to signal routing protocol using
 *                  gnrc_netif_t::ipv6::route_info_cb())
 * @param[in] nbr   Neighbor cache entry representing the neighbor.
 * @param[in] state Neighbor unreachability state for the neighbor.
 */
void _set_nud_state(gnrc_netif_t *netif, _nib_onl_entry_t *nbr,
                    uint16_t state);

/**
 * @brief   Checks if a node is in a reachable state
 *
 * A node is reachable if it is not in NUD state UNREACHABLE or INCOMPLETE
 *
 * @param[in] entry A node.
 *
 * @return  true, if @p entry is in a reachable state.
 * @return  false, if @p entry is not in a reachable state.
 */
bool _is_reachable(_nib_onl_entry_t *entry);
#else   /* CONFIG_GNRC_IPV6_NIB_ARSM || defined(DOXYGEN) */
#define _handle_snd_ns(ctx)                         (void)ctx
#define _handle_state_timeout(ctx)                  (void)ctx
#define _probe_nbr(nbr, reset)                      (void)nbr; (void)reset
#define _init_iface_arsm(netif)                     (void)netif
#define _handle_adv_l2(netif, nce, icmpv6, tl2ao)   (void)netif; (void)nce; \
                                                    (void)icmpv6; (void)tl2ao
#define _recalc_reach_time(netif)                   (void)netif
#define _set_reachable(netif, nce)                  (void)netif; (void)nce
#define _init_iface_arsm(netif)                     (void)netif

#define _get_nud_state(nbr)                 (GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED)
#define _set_nud_state(netif, nce, state)   (void)netif; (void)nbr; (void)state
#define _is_reachable(entry)                (true)
#endif  /* CONFIG_GNRC_IPV6_NIB_ARSM || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

#endif /* PRIV_NIB_ARSM_H */
/** @} */
