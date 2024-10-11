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
 * @brief   Definitions related to 6Lo node (6LN) functionality of the NIB
 * @see     @ref CONFIG_GNRC_IPV6_NIB_6LN
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef PRIV_NIB_6LN_H
#define PRIV_NIB_6LN_H

#include <kernel_defines.h>
#include <stdint.h>

#include "net/gnrc/ipv6/nib/conf.h"
#include "net/sixlowpan/nd.h"
#include "timex.h"
#include "evtimer.h"

#include "_nib-arsm.h"
#include "_nib-internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Additional (local) status to ARO status values for tentative
 *          addresses
 */
#define _ADDR_REG_STATUS_TENTATIVE      (3)

/**
 * @brief   Additional (local) status to ARO status values for return values
 *          to signify that the address was ignored
 */
#define _ADDR_REG_STATUS_IGNORE         (4)

/**
 * @brief   Additional (local) status to ARO status values to signify that ARO
 *          is not available in NA
 *
 * Can be assigned to the variable that receives the return value of
 * @ref _handle_aro(), so that the case that an NA does not contain an ARO
 * (e.g. because upstream router does not support it) can be dealt with.
 */
#define _ADDR_REG_STATUS_UNAVAIL        (255)

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN) || defined(DOXYGEN)
/**
 * @brief   Resolves address statically from destination address using reverse
 *          translation of the IID
 *
 * @param[in] dst   A destination address.
 * @param[in] netif The interface to @p dst.
 * @param[out] nce  Neighbor cache entry to resolve into
 *
 * @return  true when @p nce was set, false when not.
 */
bool _resolve_addr_from_ipv6(const ipv6_addr_t *dst, gnrc_netif_t *netif,
                             gnrc_ipv6_nib_nc_t *nce);

/**
 * @brief   Calculates exponential backoff for RS retransmissions
 *
 * @see [RFC 6775, section 5.3](https://tools.ietf.org/html/rfc6775#section-5.3)
 *
 * @param[in] netif The network interface that the RS will be sent over.
 *
 * @return  The interval in ms to the next RS
 */
static inline uint32_t _get_next_rs_interval(const gnrc_netif_t *netif)
{
    if (gnrc_netif_is_6ln(netif)) {
        if (netif->ipv6.rs_sent < SIXLOWPAN_ND_MAX_RS_NUMOF) {
            return SIXLOWPAN_ND_RS_MSEC_INTERVAL;
        }
        else {
            unsigned exp = netif->ipv6.rs_sent - SIXLOWPAN_ND_MAX_RS_NUMOF;
            uint32_t tmp = SIXLOWPAN_ND_RS_MSEC_INTERVAL +
                           ((1 << exp) * (NDP_RS_MS_INTERVAL));

            if (tmp > (SIXLOWPAN_ND_MAX_RS_SEC_INTERVAL * MS_PER_SEC)) {
                tmp = SIXLOWPAN_ND_MAX_RS_SEC_INTERVAL * MS_PER_SEC;
            }
            return tmp;
        }
    }
    else {
        return NDP_RS_MS_INTERVAL;
    }
}

/**
 * @brief   Handles ARO
 *
 * @param[in] netif     The interface the ARO-carrying message came over.
 * @param[in] ipv6      The IPv6 header of the message carrying the ARO.
 * @param[in] icmpv6    The message carrying the ARO.
 * @param[in] aro       ARO that carries the address registration information.
 * @param[in] sl2ao     SL2AO associated with the ARO.
 * @param[in] nce       Neighbor cache entry the ARO is supposed to change.
 *
 * @return  registration status of the address (including
 *          @ref _ADDR_REG_STATUS_TENTATIVE and @ref _ADDR_REG_STATUS_IGNORE).
 */
uint8_t _handle_aro(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                    const icmpv6_hdr_t *icmpv6,
                    const sixlowpan_nd_opt_ar_t *aro, const ndp_opt_t *sl2ao,
                    _nib_onl_entry_t *nce);

/**
 * @brief   Handler for @ref GNRC_IPV6_NIB_REREG_ADDRESS event handler
 *
 * @param[in] addr  An IPv6 address.
 */
void _handle_rereg_address(const ipv6_addr_t *addr);

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C) || defined(DOXYGEN)
_nib_abr_entry_t *_handle_abro(const sixlowpan_nd_opt_abr_t *abro);
uint32_t _handle_6co(const icmpv6_hdr_t *icmpv6,
                     const sixlowpan_nd_opt_6ctx_t *sixco,
                     _nib_abr_entry_t *abr);
#else   /* CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C || defined(DOXYGEN) */
uint32_t _handle_6co(const icmpv6_hdr_t *icmpv6,
                     const sixlowpan_nd_opt_6ctx_t *sixco);
#endif  /* CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C || defined(DOXYGEN) */
#else   /* CONFIG_GNRC_IPV6_NIB_6LN || defined(DOXYGEN) */
#define _resolve_addr_from_ipv6(dst, netif, nce)    (false)
/* _handle_aro() doesn't make sense without 6LR so don't even use it
 * => throw error in case it is compiled in => don't define it here as NOP macro
 */
#define _get_next_rs_interval(netif)                (NDP_RS_MS_INTERVAL)
#define _handle_rereg_address(netif)                (void)netif
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C) || defined(DOXYGEN)
#define _handle_abro(abro)                          (NULL)
#define _handle_6co(icmpv6, sixco, abr)             (UINT32_MAX)
#else   /* CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C || defined(DOXYGEN) */
#define _handle_6co(icmpv6, sixco)                  (UINT32_MAX)
#endif  /* CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C || defined(DOXYGEN) */
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PRIV_NIB_6LN_H */
