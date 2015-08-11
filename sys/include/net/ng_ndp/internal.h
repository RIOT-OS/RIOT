/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_ndp_internal Internal functions for neighbor discovery.
 * @ingroup     net_ng_ndp
 * @brief       Internal functions for neighbor discovery.
 * @internal
 * @note        Only for use with a neighbor discovery implementations.
 * @{
 *
 * @file
 * @brief   Internal neighbor discovery functions for neighbor discovery.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef INTERNAL_H_
#define INTERNAL_H_

#include "net/ng_ipv6/addr.h"
#include "net/ng_ndp/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get best match from default router list.
 *
 * @internal
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-6.3.6">
 *          RFC 4861, section 6.3.6
 *      </a>
 *
 * @return  Address to a default router.
 * @return  NULL, if the default router list is empty.
 */
ng_ipv6_addr_t *ng_ndp_internal_default_router(void);

/**
 * @brief   Sets state of a neighbor cache entry and triggers required actions.
 *
 * @internal
 *
 * @param[in] nc_entry  A neighbor cache entry.
 * @param[in] state     The new state for the neighbor cache entry.
 */
void ng_ndp_internal_set_state(ng_ipv6_nc_t *nc_entry, uint8_t state);

/**
 * @brief   Send precompiled neighbor solicitation.
 *
 * @internal
 *
 * @param[in] iface Interface to send over. May not be KERNEL_PID_UNDEF.
 * @param[in] tgt   Target address for the neighbor solicitation. May not be
 *                  NULL.
 * @param[in] dst   Destination address for neighbor solicitation. May not be
 *                  NULL.
 */
void ng_ndp_internal_send_nbr_sol(kernel_pid_t iface, ng_ipv6_addr_t *tgt,
                                  ng_ipv6_addr_t *dst);

/**
 * @brief   Send precompiled neighbor advertisement.
 *
 * @internal
 *
 * @param[in] iface         Interface to send over. May not be KERNEL_PID_UNDEF.
 * @param[in] tgt           Target address for the neighbor solicitation. May
 *                          not be NULL.
 * @param[in] dst           Destination address for neighbor solicitation. May
 *                          not be NULL.
 * @param[in] supply_tl2a   Add target link-layer address option to neighbor
 *                          advertisement if link-layer has addresses.
 */
void ng_ndp_internal_send_nbr_adv(kernel_pid_t iface, ng_ipv6_addr_t *tgt,
                                  ng_ipv6_addr_t *dst, bool supply_tl2a);

/**
 * @brief   Handles a SL2A option.
 *
 * @param[in] iface         Interface the option was received on.
 * @param[in] pkt           Packet the option was received in.
 * @param[in] ipv6          IPv6 header of @p pkt
 * @param[in] icmpv6_type   ICMPv6 type of the message carrying the option.
 * @param[in] sl2a_opt      The SL2A option.
 *
 * @return  true, on success.
 * @return  false, if SL2A was not valid.
 */
bool ng_ndp_internal_sl2a_opt_handle(kernel_pid_t iface, ng_pktsnip_t *pkt,
                                     ng_ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                                     ng_ndp_opt_t *sl2a_opt);

/**
 * @brief   Handles a TL2A option.
 *
 * @param[in] iface         Interface the option was received on.
 * @param[in] pkt           Packet the option was received in.
 * @param[in] ipv6          IPv6 header of @p pkt
 * @param[in] icmpv6_type   ICMPv6 type of the message carrying the option.
 * @param[in] tl2a_opt      The TL2A option.
 * @param[out] l2addr       The L2 address carried in the TL2A option.
 *
 * @return  length of the L2 address, on success.
 * @return  -EINVAL, if TL2A was not valid.
 */
int ng_ndp_internal_tl2a_opt_handle(ng_pktsnip_t *pkt, ng_ipv6_hdr_t *ipv6,
                                    uint8_t icmpv6_type, ng_ndp_opt_t *tl2a_opt,
                                    uint8_t *l2addr);

#ifdef __cplusplus
}
#endif

#endif /* INTERNAL_H_ */
/** @} */
