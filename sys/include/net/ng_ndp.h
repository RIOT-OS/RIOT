/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ng_ndp  IPv6 Neighbor discovery
 * @ingroup     net_ng_icmpv6
 * @brief       IPv6 Neighbor Discovery Implementation
 * @{
 *
 * @file
 * @brief       Neighbor Discovery definitions
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <inttypes.h>

#include "byteorder.h"
#include "net/ng_pkt.h"
#include "net/ng_icmpv6.h"
#include "net/ipv6/addr.h"
#include "net/ng_ipv6/nc.h"
#include "net/ng_ipv6/netif.h"

#include "net/ng_ndp/node.h"
#include "net/ng_ndp/types.h"

#ifndef NG_NDP_H_
#define NG_NDP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define NG_NDP_MSG_RTR_TIMEOUT      (0x0211)    /**< Message type for router timeouts */
#define NG_NDP_MSG_ADDR_TIMEOUT     (0x0212)    /**< Message type for address timeouts */
#define NG_NDP_MSG_NBR_SOL_RETRANS  (0x0213)    /**< Message type for multicast
                                                 *   neighbor solicitation retransmissions */
#define NG_NDP_MSG_NC_STATE_TIMEOUT (0x0214)    /**< Message type for neighbor cache state timeouts */

/**
 * @{
 * @name    Node constants
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-10">
 *              RFC 4861, section 10
 *          </a>
 */
/**
 * @brief   Maximum number of unanswered multicast neighbor solicitations
 *          before address resolution is considered failed.
 */
#define NG_NDP_MAX_MC_NBR_SOL_NUMOF (3U)

/**
 * @brief   Maximum number of unanswered unicast neighbor solicitations before
 *          an address is considered unreachable.
 */
#define NG_NDP_MAX_UC_NBR_SOL_NUMOF (3U)

/**
 * @brief   Upper bound of randomized delay in seconds for a solicited
 *          neighbor advertisement transmission for an anycast target.
 */
#define NG_NDP_MAX_AC_TGT_DELAY     (1U)

/**
 * @brief   Maximum number of unsolicited neighbor advertisements before on
 *          link-layer address change.
 */
#define NG_NDP_MAX_NBR_ADV_NUMOF    (3U)

/**
 * @brief   Base value in mircoseconds for computing randomised
 *          reachable time.
 */
#define NG_NDP_REACH_TIME           (30U * SEC_IN_USEC)

/**
 * @brief   Time in mircoseconds between retransmissions of neighbor
 *          solicitations to a neighbor.
 */
#define NG_NDP_RETRANS_TIMER        (1U * SEC_IN_USEC)

/**
 * @brief   Delay in seconds for neighbor cache entry between entering
 *          DELAY state and entering PROBE state if no reachability
 *          confirmation has been received.
 */
#define NG_NDP_FIRST_PROBE_DELAY    (5U)

/**
 * @brief   Lower bound for randomised reachable time calculation.
 */
#define NG_NDP_MIN_RAND             (5U)

/**
 * @brief   Upper bound for randomised reachable time calculation.
 */
#define NG_NDP_MAX_RAND             (15U)
/**
 * @}
 */

/**
 * @brief   Handles received neighbor solicitations.
 *
 * @param[in] iface         The receiving interface.
 * @param[in] pkt           The received packet.
 * @param[in] ipv6          The IPv6 header in @p pkt.
 * @param[in] nbr_sol       The neighbor solicitation in @p pkt.
 * @param[in] icmpv6_size   The overall size of the neighbor solicitation.
 */
void ng_ndp_nbr_sol_handle(kernel_pid_t iface, ng_pktsnip_t *pkt,
                           ipv6_hdr_t *ipv6, ng_ndp_nbr_sol_t *nbr_sol,
                           size_t icmpv6_size);

/**
 * @brief   Handles received neighbor advertisements.
 *
 * @param[in] iface         The receiving interface.
 * @param[in] pkt           The received packet.
 * @param[in] ipv6          The IPv6 header in @p pkt.
 * @param[in] nbr_adv       The neighbor advertisement in @p pkt.
 * @param[in] icmpv6_size   The overall size of the neighbor advertisement.
 */
void ng_ndp_nbr_adv_handle(kernel_pid_t iface, ng_pktsnip_t *pkt,
                           ipv6_hdr_t *ipv6, ng_ndp_nbr_adv_t *nbr_adv,
                           size_t icmpv6_size);

/**
 * @brief   Retransmits a multicast neighbor solicitation for an incomplete or
 *          probing neighbor cache entry @p nc_entry,
 *          if nc_entry::probes_remaining > 0.
 *
 * @details If nc_entry::probes_remaining > 0 it will be decremented. If it
 *          reaches 0 it the entry @p nc_entry will be removed from the
 *          neighbor cache.
 *
 * @param[in]   nc_entry    A neighbor cache entry. Will be ignored if its state
 *                          is not @ref NG_IPV6_NC_STATE_INCOMPLETE or
 *                          @ref NG_IPV6_NC_STATE_PROBE.
 */
void ng_ndp_retrans_nbr_sol(ng_ipv6_nc_t *nc_entry);

/**
 * @brief   Event handler for a neighbor cache state timeout.
 *
 * @param[in]   nc_entry    A neighbor cache entry.
 */
void ng_ndp_state_timeout(ng_ipv6_nc_t *nc_entry);

/**
 * @brief   NDP interface initialization.
 *
 * @param[in] iface     An IPv6 interface descriptor. Must not be NULL.
 */
void ng_ndp_netif_add(ng_ipv6_netif_t *iface);

/**
 * @brief   NDP interface removal.
 *
 * @param[in] iface     An IPv6 interface descriptor. Must not be NULL.
 */
void ng_ndp_netif_remove(ng_ipv6_netif_t *iface);

/**
 * @brief   Builds a neighbor solicitation message for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.3">
 *          RFC 4861, section 4.3
 *      </a>
 *
 * @param[in] tgt       The target address.
 * @param[in] options   Options to append to the router solicitation.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, on failure.
 */
ng_pktsnip_t *ng_ndp_nbr_sol_build(ipv6_addr_t *tgt, ng_pktsnip_t *options);

/**
 * @brief   Builds a neighbor advertisement message for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.4
 *      </a>
 *
 * @param[in] flags     Flags as defined above.
 *                      @ref NG_NDP_NBR_ADV_FLAGS_R == 1 indicates, that the
 *                      sender is a router,
 *                      @ref NG_NDP_NBR_ADV_FLAGS_S == 1 indicates that the
 *                      advertisement was sent in response to a neighbor
 *                      solicitation,
 *                      @ref NG_NDP_NBR_ADV_FLAGS_O == 1 indicates that the
 *                      advertisement should override an existing cache entry
 *                      and update the cached link-layer address.
 * @param[in] tgt       For solicited advertisements, the Target Address field
 *                      in the neighbor solicitaton.
 *                      For and unsolicited advertisement, the address whose
 *                      link-layer address has changed.
 *                      MUST NOT be multicast.
 * @param[in] options   Options to append to the neighbor advertisement.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, on failure.
 */
ng_pktsnip_t *ng_ndp_nbr_adv_build(uint8_t flags, ipv6_addr_t *tgt,
                                   ng_pktsnip_t *options);

/**
 * @brief   Builds a generic NDP option.
 *
 * @param[in] type  Type of the option.
 * @param[in] size  Size in byte of the option (will be rounded up to the next
 *                  multiple of 8).
 * @param[in] next  More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
ng_pktsnip_t *ng_ndp_opt_build(uint8_t type, size_t size, ng_pktsnip_t *next);

/**
 * @brief   Builds the source link-layer address option.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.1">
 *          RFC 4861, section 4.6.1
 *      </a>
 *
 * @note    Must only be used with neighbor solicitations, router solicitations,
 *          and router advertisements. This is not checked however, since
 *          hosts should silently ignore it in other NDP messages.
 *
 * @param[in] l2addr        A link-layer address of variable length.
 * @param[in] l2addr_len    Length of @p l2addr.
 * @param[in] next          More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
ng_pktsnip_t *ng_ndp_opt_sl2a_build(const uint8_t *l2addr, uint8_t l2addr_len,
                                    ng_pktsnip_t *next);

/**
 * @brief   Builds the target link-layer address option.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.1">
 *          RFC 4861, section 4.6.1
 *      </a>
 *
 * @note    Must only be used with neighbor advertisemnents and redirect packets.
 *          This is not checked however, since hosts should silently ignore it
 *          in other NDP messages.
 *
 * @param[in] l2addr        A link-layer address of variable length.
 * @param[in] l2addr_len    Length of @p l2addr.
 * @param[in] next          More options in the packet. NULL, if there are none.
 *
 * @return  The pkt snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
ng_pktsnip_t *ng_ndp_opt_tl2a_build(const uint8_t *l2addr, uint8_t l2addr_len,
                                    ng_pktsnip_t *next);

#ifdef __cplusplus
}
#endif

#endif /* NG_NDP_H_ */
/**
 * @}
 */
