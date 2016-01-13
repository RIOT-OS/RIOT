/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gnrc_ndp  IPv6 Neighbor discovery
 * @ingroup     net_gnrc_icmpv6
 * @brief       GNRC's IPv6 Neighbor Discovery implementation
 * @{
 *
 * @file
 * @brief       Definitions for GNRC's IPv6 Neighbor Discovery
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef GNRC_NDP_H_
#define GNRC_NDP_H_

#include <inttypes.h>
#include <stdlib.h>

#include "byteorder.h"
#include "net/ndp.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/icmpv6.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/ipv6/netif.h"

#include "net/gnrc/ndp/host.h"
#include "net/gnrc/ndp/internal.h"
#include "net/gnrc/ndp/router.h"
#include "net/gnrc/ndp/node.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Message type for router timeouts */
#define GNRC_NDP_MSG_RTR_TIMEOUT                (0x0210)
/** Message type for address timeouts */
#define GNRC_NDP_MSG_ADDR_TIMEOUT               (0x0211)
/** Message type for multicast neighbor solicitation retransmissions */
#define GNRC_NDP_MSG_NBR_SOL_RETRANS            (0x0212)
/** Message type for periodic router advertisements */
#define GNRC_NDP_MSG_RTR_ADV_RETRANS            (0x0213)
/** Message type for delayed router advertisements */
#define GNRC_NDP_MSG_RTR_ADV_DELAY              (0x0214)
/** Message type for delayed router advertisements in a 6LoWPAN
 * 6LoWPAN needs a special handling, because router advertisements are only
 * sent after a short randomized delay, but not periodically. */
#define GNRC_NDP_MSG_RTR_ADV_SIXLOWPAN_DELAY    (0x0215)
/** Message type for periodic router solicitations */
#define GNRC_NDP_MSG_RTR_SOL_RETRANS            (0x0216)
/** Message type for neighbor cache state timeouts */
#define GNRC_NDP_MSG_NC_STATE_TIMEOUT           (0x0217)

/**
 * @name    Host constants
 * @{
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-10">
 *              RFC 4861, section 10
 *          </a>
 */
/**
 * @brief   Upper bound for randomised delay in seconds for initial
 *          router solicitation transmissions
 */
#define GNRC_NDP_MAX_RTR_SOL_DELAY      (1U)

/**
 * @brief   Interval in seconds between initial router solicitation
 *          transmissions
 */
#define GNRC_NDP_MAX_RTR_SOL_INT        (4U)

/**
 * @brief   Maximum number of  initial router solicitation transmissions
 */
#define GNRC_NDP_MAX_RTR_SOL_NUMOF      (3U)
/** @} */

/**
 * @name    Node constants
 * @{
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-10">
 *              RFC 4861, section 10
 *          </a>
 */
/**
 * @brief   Maximum number of unanswered multicast neighbor solicitations
 *          before address resolution is considered failed.
 */
#define GNRC_NDP_MAX_MC_NBR_SOL_NUMOF   (3U)

/**
 * @brief   Maximum number of unanswered unicast neighbor solicitations before
 *          an address is considered unreachable.
 */
#define GNRC_NDP_MAX_UC_NBR_SOL_NUMOF   (3U)

/**
 * @brief   Upper bound of randomized delay in seconds for a solicited
 *          neighbor advertisement transmission for an anycast target.
 */
#define GNRC_NDP_MAX_AC_TGT_DELAY       (1U)

/**
 * @brief   Maximum number of unsolicited neighbor advertisements before on
 *          link-layer address change.
 */
#define GNRC_NDP_MAX_NBR_ADV_NUMOF      (3U)

/**
 * @brief   Base value in mircoseconds for computing randomised
 *          reachable time.
 */
#define GNRC_NDP_REACH_TIME             (30U * SEC_IN_USEC)

/**
 * @brief   Time in mircoseconds between retransmissions of neighbor
 *          solicitations to a neighbor.
 */
#define GNRC_NDP_RETRANS_TIMER          (1U * SEC_IN_USEC)

/**
 * @brief   Delay in seconds for neighbor cache entry between entering
 *          DELAY state and entering PROBE state if no reachability
 *          confirmation has been received.
 */
#define GNRC_NDP_FIRST_PROBE_DELAY      (5U)

/**
 * @brief   Lower bound for randomised reachable time calculation.
 */
#define GNRC_NDP_MIN_RAND               (5U)

/**
 * @brief   Upper bound for randomised reachable time calculation.
 */
#define GNRC_NDP_MAX_RAND               (15U)
/** @} */

/**
 * @name    Router constants
 * @{
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-10">
 *              RFC 4861, section 10
 *          </a>
 */
/**
 * @brief   Initial router advertisement interval in seconds
 */
#define GNRC_NDP_MAX_INIT_RTR_ADV_INT   (16U)

/**
 * @brief   Maximum number of initial router advertisement transmissions
 */
#define GNRC_NDP_MAX_INIT_RTR_ADV_NUMOF (3U)

/**
 * @brief   Maximum number of final router advertisement transmissions
 */
#define GNRC_NDP_MAX_FIN_RTR_ADV_NUMOF  (3U)

/**
 * @brief   Minimum delay in seconds between router advertisement
 *          transmissions
 */
#define GNRC_NDP_MIN_RTR_ADV_DELAY      (3U)

/**
 * @brief   Upper bound for randomised delay in microseconds between router
 *          solicitation reception and responding router advertisement
 *          transmission.
 */
#define GNRC_NDP_MAX_RTR_ADV_DELAY      (500U * MS_IN_USEC)
/** @} */

/**
 * @brief   Handles received neighbor solicitations.
 *
 * @param[in] iface         The receiving interface.
 * @param[in] pkt           The received packet.
 * @param[in] ipv6          The IPv6 header in @p pkt.
 * @param[in] nbr_sol       The neighbor solicitation in @p pkt.
 * @param[in] icmpv6_size   The overall size of the neighbor solicitation.
 */
void gnrc_ndp_nbr_sol_handle(kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                             ipv6_hdr_t *ipv6, ndp_nbr_sol_t *nbr_sol,
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
void gnrc_ndp_nbr_adv_handle(kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                             ipv6_hdr_t *ipv6, ndp_nbr_adv_t *nbr_adv,
                             size_t icmpv6_size);

#if (defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_ROUTER))
/**
 * @brief   Handles received router solicitations.
 *
 * @param[in] iface         The receiving interface.
 * @param[in] pkt           The received packet.
 * @param[in] ipv6          The IPv6 header in @p pkt.
 * @param[in] rtr_sol       The router solicitation in @p pkt.
 * @param[in] icmpv6_size   The overall size of the router solicitation.
 */
void gnrc_ndp_rtr_sol_handle(kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                             ipv6_hdr_t *ipv6, ndp_rtr_sol_t *rtr_sol,
                             size_t icmpv6_size);
#else
/**
 * @brief   A host *must* silently discard all received router solicitations.
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-6.2.6">
 *              RFC 4861, section 6.2.6
 *          </a>
 *
 * This macro is primarily an optimization to not go into the function defined
 * above.
 */
#define gnrc_ndp_rtr_sol_handle(iface, pkt, ipv6, rtr_sol, size)
#endif

/**
 * @brief   Handles received router advertisements
 *
 * @todo    As router check consistency as described in RFC 4861, section 6.2.3
 *
 * @param[in] iface         The receiving interface.
 * @param[in] pkt           The received packet.
 * @param[in] ipv6          The IPv6 header in @p pkt.
 * @param[in] rtr_adv       The router advertisement in @p pkt.
 * @param[in] icmpv6_size   The overall size of the router advertisement.
 */
void gnrc_ndp_rtr_adv_handle(kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                             ipv6_hdr_t *ipv6, ndp_rtr_adv_t *rtr_adv,
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
 *                          is not @ref GNRC_IPV6_NC_STATE_INCOMPLETE or
 *                          @ref GNRC_IPV6_NC_STATE_PROBE.
 */
void gnrc_ndp_retrans_nbr_sol(gnrc_ipv6_nc_t *nc_entry);

/**
 * @brief   Event handler for a neighbor cache state timeout.
 *
 * @param[in]   nc_entry    A neighbor cache entry.
 */
void gnrc_ndp_state_timeout(gnrc_ipv6_nc_t *nc_entry);

/**
 * @brief   NDP interface initialization.
 *
 * @param[in] iface     An IPv6 interface descriptor. Must not be NULL.
 */
void gnrc_ndp_netif_add(gnrc_ipv6_netif_t *iface);

/**
 * @brief   NDP interface removal.
 *
 * @param[in] iface     An IPv6 interface descriptor. Must not be NULL.
 */
void gnrc_ndp_netif_remove(gnrc_ipv6_netif_t *iface);

/**
 * @brief   Get link-layer address and interface for next hop to destination
 *          IPv6 address.
 *
 * @param[out] l2addr           The link-layer for the next hop to @p dst.
 * @param[out] l2addr_len       Length of @p l2addr.
 * @param[in] iface             The interface to search the next hop on.
 *                              May be @ref KERNEL_PID_UNDEF if not specified.
 * @param[in] dst               An IPv6 address to search the next hop for.
 * @param[in] pkt               Packet to send to @p dst. Leave NULL if you
 *                              just want to get the addresses.
 *
 * @return  The PID of the interface, on success.
 * @return  -EHOSTUNREACH, if @p dst is not reachable.
 * @return  -ENOBUFS, if @p l2addr_len was smaller than the resulting @p l2addr
 *          would be long.
 */
kernel_pid_t gnrc_ndp_next_hop_l2addr(uint8_t *l2addr, uint8_t *l2addr_len,
                                      kernel_pid_t iface, ipv6_addr_t *dst,
                                      gnrc_pktsnip_t *pkt);

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
gnrc_pktsnip_t *gnrc_ndp_nbr_sol_build(ipv6_addr_t *tgt, gnrc_pktsnip_t *options);

/**
 * @brief   Builds a neighbor advertisement message for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.4
 *      </a>
 *
 * @param[in] flags     Neighbor advertisement flags:
 *                      @ref NDP_NBR_ADV_FLAGS_R == 1 indicates, that the
 *                      sender is a router,
 *                      @ref NDP_NBR_ADV_FLAGS_S == 1 indicates that the
 *                      advertisement was sent in response to a neighbor
 *                      solicitation,
 *                      @ref NDP_NBR_ADV_FLAGS_O == 1 indicates that the
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
gnrc_pktsnip_t *gnrc_ndp_nbr_adv_build(uint8_t flags, ipv6_addr_t *tgt,
                                       gnrc_pktsnip_t *options);

/**
 * @brief   Builds a router solicitation message for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.1">
 *          RFC 4861, section 4.1
 *      </a>
 *
 * @param[in] options   Options to append to the router solicitation.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, on failure.
 */
gnrc_pktsnip_t *gnrc_ndp_rtr_sol_build(gnrc_pktsnip_t *options);

/**
 * @brief   Builds a router solicitation message for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.1">
 *          RFC 4861, section 4.1
 *      </a>
 *
 * @param[in] options   Options to append to the router solicitation.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, on failure.
 */
gnrc_pktsnip_t *gnrc_ndp_rtr_sol_build(gnrc_pktsnip_t *options);

#if (defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_ROUTER))
/**
 * @brief   Builds a router advertisement message for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>
 *
 * @note    The source address for the packet MUST be the link-local address
 *          of the interface.
 *
 * @param[in] cur_hl        Default hop limit for outgoing IP packets, 0 if
 *                          unspecified by this router.
 * @param[in] flags         Flags as defined above.
 *                          @ref GNRC_NDP_RTR_ADV_FLAGS_M == 1 indicates, that the
 *                          addresses are managed by DHCPv6,
 *                          @ref GNRC_NDP_RTR_ADV_FLAGS_O == 1 indicates that other
 *                          configuration information is available via DHCPv6.
 * @param[in] ltime         Lifetime of the default router in seconds.
 * @param[in] reach_time    Time in milliseconds a node should assume a neighbor
 *                          reachable. 0 means unspecified by the router.
 * @param[in] retrans_timer Time in milliseconds between retransmitted
 *                          neighbor solicitations. 0 means unspecified by
 *                          the router.
 * @param[in] options       Options to append to the router advertisement.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, on failure.
 */
gnrc_pktsnip_t *gnrc_ndp_rtr_adv_build(uint8_t cur_hl, uint8_t flags, uint16_t ltime,
                                       uint32_t reach_time, uint32_t retrans_timer,
                                       gnrc_pktsnip_t *options);
#else
/**
 * @brief   A host *must not* send router advertisements at any time (so why build them?)
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-6.3.4">
 *              RFC 4861, section 6.3.4
 *          </a>
 *
 * This macro is primarily an optimization to not go into the function defined
 * above.
 */
#define gnrc_ndp_rtr_adv_build(cur_hl, flags, ltime, reach_time, retrans_timer, options) (NULL)
#endif

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
gnrc_pktsnip_t *gnrc_ndp_opt_build(uint8_t type, size_t size, gnrc_pktsnip_t *next);

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
gnrc_pktsnip_t *gnrc_ndp_opt_sl2a_build(const uint8_t *l2addr, uint8_t l2addr_len,
                                        gnrc_pktsnip_t *next);

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
gnrc_pktsnip_t *gnrc_ndp_opt_tl2a_build(const uint8_t *l2addr, uint8_t l2addr_len,
                                        gnrc_pktsnip_t *next);

#if (defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_ROUTER))
/**
 * @brief   Builds the prefix information option.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.2">
 *          RFC 4861, section 4.6.2
 *      </a>
 *
 * @note    Must only be used with router advertisemnents. This is not checked
 *          however, since nodes should silently ignore it in other NDP messages.
 *
 * @param[in] prefix_len    The length of @p prefix in bits. Must be between
 *                          0 and 128.
 * @param[in] flags         Flags as defined above.
 *                          @ref NDP_OPT_PI_FLAGS_L == 1 indicates, that
 *                          @p prefix can be used for on-link determination,
 *                          @ref NDP_OPT_PI_FLAGS_A == 1 indicates, that
 *                          @p prefix can be used for stateless address
 *                          configuration.
 * @param[in] valid_ltime   Length of time in seconds that @p prefix is valid.
 *                          UINT32_MAX represents infinity.
 * @param[in] pref_ltime    Length of time in seconds that addresses using
 *                          @p prefix remain prefered. UINT32_MAX represents
 *                          infinity.
 * @param[in] prefix        An IPv6 address or a prefix of an IPv6 address.
 * @param[in] next          More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
gnrc_pktsnip_t *gnrc_ndp_opt_pi_build(uint8_t prefix_len, uint8_t flags,
                                      uint32_t valid_ltime, uint32_t pref_ltime,
                                      ipv6_addr_t *prefix, gnrc_pktsnip_t *next);

/**
 * @brief   Builds the MTU option.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.4">
 *          RFC 4861, section 4.6.4
 *      </a>
 *
 * @note    Must only be used with router advertisemnents. This is not checked
 *          however, since nodes should silently ignore it in other NDP messages.
 *
 * @param[in] mtu           The recommended MTU for the link.
 * @param[in] next          More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
gnrc_pktsnip_t *gnrc_ndp_opt_mtu_build(uint32_t mtu, gnrc_pktsnip_t *next);
#else
/**
 * @brief   A host *must not* send router advertisements at any time (so why build their options?)
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-6.3.4">
 *              RFC 4861, section 6.3.4
 *          </a>
 *
 * This macro is primarily an optimization to not go into the function defined
 * above.
 */
#define gnrc_ndp_opt_pi_build(prefix_len, flags, valid_ltime, pref_ltime, prefix, next) (NULL)

/**
 * @brief   A host *must not* send router advertisements at any time (so why build their options?)
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-6.3.4">
 *              RFC 4861, section 6.3.4
 *          </a>
 *
 * This macro is primarily an optimization to not go into the function defined
 * above.
 */
#define gnrc_ndp_opt_mtu_build(mtu, next)   (NULL)
#endif

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NDP_H_ */
/**
 * @}
 */
