/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ndp_internal Internal functions for neighbor discovery.
 * @ingroup     net_gnrc_ndp
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
#ifndef GNRC_NDP_INTERNAL_H_
#define GNRC_NDP_INTERNAL_H_

#include "kernel_types.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/ndp.h"

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
ipv6_addr_t *gnrc_ndp_internal_default_router(void);

/**
 * @brief   Sets state of a neighbor cache entry and triggers required actions.
 *
 * @internal
 *
 * @param[in] nc_entry  A neighbor cache entry.
 * @param[in] state     The new state for the neighbor cache entry.
 */
void gnrc_ndp_internal_set_state(gnrc_ipv6_nc_t *nc_entry, uint8_t state);

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
void gnrc_ndp_internal_send_nbr_sol(kernel_pid_t iface, ipv6_addr_t *tgt,
                                    ipv6_addr_t *dst);

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
 * @param[in] ext_opts      External options for the neighbor advertisement. Leave NULL for none.
 *                          **Warning:** these are not tested if they are suitable for a
 *                          neighbor advertisement so be sure to check that.
 *                          **Will be released** in an error case.
 */
void gnrc_ndp_internal_send_nbr_adv(kernel_pid_t iface, ipv6_addr_t *tgt, ipv6_addr_t *dst,
                                    bool supply_tl2a, gnrc_pktsnip_t *ext_opts);

/**
 * @brief   Send precompiled router solicitation to @p dst.
 *
 * @internal
 *
 * @param[in] iface Interface to send over. May not be KERNEL_PID_UNDEF.
 * @param[in] dst   Destination for the router solicitation. ff02::2 if NULL.
 */
void gnrc_ndp_internal_send_rtr_sol(kernel_pid_t iface, ipv6_addr_t *dst);

#if (defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_ROUTER))
/**
 * @brief   Handles received router solicitations.
 *
 * @param[in] iface         Interface to send over. May not be KERNEL_PID_UNDEF.
 * @param[in] src           Source address for the router advertisement. May be NULL to be determined
 *                          by source address selection (:: if no @p iface has no address).
 * @param[in] dst           Destination address for router advertisement.
 *                          @ref IPV6_ADDR_ALL_NODES_LINK_LOCAL if NULL.
 * @param[in] fin           This is part of the router's final batch of router advertisements
 *                          before ceising to be a router (set's router lifetime field to 0).
 */
void gnrc_ndp_internal_send_rtr_adv(kernel_pid_t iface, ipv6_addr_t *src,
                                    ipv6_addr_t *dst, bool fin);
#else
/**
 * @brief   A host *must not* send router advertisements at any time.
 *
 * This macro is primarily an optimization to not go into the function defined
 * above.
 */
#define gnrc_ndp_internal_send_rtr_adv(iface, dst, fin)
#endif

/**
 * @brief   Handles a SL2A option.
 *
 * @param[in] pkt           Packet the option was received in.
 * @param[in] ipv6          IPv6 header of @p pkt
 * @param[in] icmpv6_type   ICMPv6 type of the message carrying the option.
 * @param[in] tl2a_opt      The TL2A option.
 * @param[out] l2addr       The L2 address carried in the SL2A option.
 *
 * @return  length of the L2 address, on success.
 * @return  -EINVAL, if SL2A was not valid.
 * @return  -ENOTSUP, if node should silently ignore the option.
 */
int gnrc_ndp_internal_sl2a_opt_handle(gnrc_pktsnip_t *pkt, ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                                      ndp_opt_t *sl2a_opt, uint8_t *l2addr);

/**
 * @brief   Handles a TL2A option.
 *
 * @param[in] pkt           Packet the option was received in.
 * @param[in] ipv6          IPv6 header of @p pkt
 * @param[in] icmpv6_type   ICMPv6 type of the message carrying the option.
 * @param[in] tl2a_opt      The TL2A option.
 * @param[out] l2addr       The L2 address carried in the TL2A option.
 *
 * @return  length of the L2 address, on success.
 * @return  -EINVAL, if TL2A was not valid.
 * @return  -ENOTSUP, if node should silently ignore the option.
 */
int gnrc_ndp_internal_tl2a_opt_handle(gnrc_pktsnip_t *pkt, ipv6_hdr_t *ipv6,
                                      uint8_t icmpv6_type, ndp_opt_t *tl2a_opt,
                                      uint8_t *l2addr);

/**
 * @brief   Handles a MTU option.
 *
 * @internal
 *
 * @param[in] iface         Interface the MTU option was received on.
 * @param[in] icmpv6_type   ICMPv6 type of the message carrying the option.
 * @param[in] mtu_opt       A MTU option.
 *
 * @return  true, on success (or if the node should silently ignore the option).
 * @return  false, if MTU option was not valid.
 */
bool gnrc_ndp_internal_mtu_opt_handle(kernel_pid_t iface, uint8_t icmpv6_type,
                                      ndp_opt_mtu_t *mtu_opt);

/**
 * @brief   Handles a PI option.
 *
 * @internal
 *
 * @param[in] iface         Interface the PI option was received on.
 * @param[in] icmpv6_type   ICMPv6 type of the message carrying the option.
 * @param[in] pi_opt        A PI option.
 *
 * @return  true, on success (or if the node should silently ignore the option).
 * @return  false, if PIO was not valid.
 */
bool gnrc_ndp_internal_pi_opt_handle(kernel_pid_t iface, uint8_t icmpv6_type,
                                     ndp_opt_pi_t *pi_opt);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NDP_INTERNAL_H_ */
/** @} */
