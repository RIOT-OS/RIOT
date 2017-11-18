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
 * @see     @ref GNRC_IPV6_NIB_CONF_6LN
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef PRIV_NIB_6LN_H
#define PRIV_NIB_6LN_H

#include <stdint.h>

#include "net/gnrc/ipv6/nib/conf.h"
#include "net/sixlowpan/nd.h"

#include "_nib-arsm.h"
#include "_nib-internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#if GNRC_IPV6_NIB_CONF_6LN || defined(DOXYGEN)
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
 * @brief   Checks if interface represents a 6LN
 *
 * @todo    Use corresponding function in `gnrc_netif2` instead.
 *
 * @param[in] netif A network interface.
 *
 * @return  true, when the @p netif represents a 6LN.
 * @return  false, when the @p netif does not represent a 6LN.
 */
static inline bool _is_6ln(const gnrc_ipv6_netif_t *netif)
{
    return (netif->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN);
}

/**
 * @brief   Resolves address statically from destination address using reverse
 *          translation of the IID
 *
 * @param[in] dst   A destination address.
 * @param[in] iface The interface to @p dst.
 * @param[out] nce  Neighbor cache entry to resolve into
 *
 * @return  true when @p nce was set, false when not.
 */
bool _resolve_addr_from_ipv6(const ipv6_addr_t *dst, kernel_pid_t iface,
                             gnrc_ipv6_nib_nc_t *nce);

/**
 * @brief   Handles ARO
 *
 * @param[in] iface     The interface the ARO-carrying message came over.
 * @param[in] ipv6      The IPv6 header of the message carrying the ARO.
 * @param[in] icmpv6    The message carrying the ARO.
 * @param[in] aro       ARO that carries the address registration information.
 * @param[in] sl2ao     SL2AO associated with the ARO.
 * @param[in] nce       Neighbor cache entry the ARO is supposed to change.
 *
 * @return  registration status of the address (including
 *          @ref _ADDR_REG_STATUS_TENTATIVE and @ref _ADDR_REG_STATUS_IGNORE).
 */
uint8_t _handle_aro(kernel_pid_t iface, const ipv6_hdr_t *ipv6,
                    const icmpv6_hdr_t *icmpv6,
                    const sixlowpan_nd_opt_ar_t *aro, const ndp_opt_t *sl2ao,
                    _nib_onl_entry_t *nce);
#else   /* GNRC_IPV6_NIB_CONF_6LN || defined(DOXYGEN) */
#define _is_6ln(netif)                              (false)
#define _resolve_addr_from_ipv6(dst, iface, nce)    (false)
/* _handle_aro() doesn't make sense without 6LR so don't even use it
 * => throw error in case it is compiled in => don't define it here as NOP macro
 */
#endif  /* GNRC_IPV6_NIB_CONF_6LN || defined(DOXYGEN) */


#ifdef __cplusplus
}
#endif

#endif /* PRIV_NIB_6LN_H */
/** @} */
