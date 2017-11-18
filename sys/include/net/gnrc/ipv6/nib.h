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
 * @todo    Implement multihop DAD
 * @todo    Implement classic SLAAC
 * @{
 *
 * @file
 * @brief   NIB definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_IPV6_NIB_H
#define NET_GNRC_IPV6_NIB_H

#include "net/gnrc/ipv6/nib/abr.h"
#include "net/gnrc/ipv6/nib/ft.h"
#include "net/gnrc/ipv6/nib/nc.h"
#include "net/gnrc/ipv6/nib/pl.h"

#include "net/icmpv6.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/ipv6/nib/nc.h"
#include "net/gnrc/pkt.h"

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

/**
 * @brief   Recalculate reachability timeout time.
 *
 * This message type is for the event of recalculating the reachability timeout
 * time. The expected message context is a valid interface.
 *
 * @note    Only handled with @ref GNRC_IPV6_NIB_CONF_ARSM != 0
 */
#define GNRC_IPV6_NIB_RECALC_REACH_TIME     (0x4fceU)
/** @} */

/**
 * @brief   Initialize NIB
 */
void gnrc_ipv6_nib_init(void);

/**
 * @brief   Adds an interface to be managed by the NIB.
 *
 * @pre `(KERNEL_PID_UNDEF < iface)`
 *
 * @param[in] iface The interface to be managed by the NIB
 */
void gnrc_ipv6_nib_init_iface(kernel_pid_t iface);

/**
 * @brief   Gets link-layer address of next hop to a destination address
 *
 * @pre `(dst != NULL) && (nce != NULL)`
 *
 * @param[in] dst       Destination address of a packet.
 * @param[in] iface     Restrict search to this interface. May be
 *                      `KERNEL_PID_UNDEF` for any interface.
 * @param[in] pkt       The IPv6 packet in sending order for which the next hop
 *                      is searched. Needed for queuing for with reactive
 *                      routing or address resolution. May be `NULL`.
 *                      Will be released properly on error.
 * @param[out] nce      The neighbor cache entry of the next hop to @p dst.
 *
 * @return  0, on success.
 * @return  -ENETUNREACH if there is no route to host.
 * @return  -EHOSTUNREACH if the next hop is not reachable or if @p dst was
 *          link-local, but @p iface was @ref KERNEL_PID_UNDEF (no neighbor
 *          cache entry will be created in this case and no neighbor
 *          solicitation sent).
 */
int gnrc_ipv6_nib_get_next_hop_l2addr(const ipv6_addr_t *dst,
                                      kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                                      gnrc_ipv6_nib_nc_t *nce);

/**
 * @brief   Handles a received ICMPv6 packet
 *
 * @pre `iface != KERNEL_PID_UNDEF`
 * @pre `ipv6 != NULL`
 * @pre `icmpv6 != NULL`
 * @pre `icmpv6_len > sizeof(icmpv6_hdr_t)`
 *
 * @attention   The ICMPv6 checksum is supposed to be checked externally!
 *
 * @note    @p ipv6 is just used for the addresses and hop limit. The next
 *          header field will not be checked for correctness (but should be
 *          @ref PROTNUM_ICMPV6)
 *
 * @see [RFC 4861, section 6.1](https://tools.ietf.org/html/rfc4861#section-6.1)
 * @see [RFC 4861, section 6.2.6](https://tools.ietf.org/html/rfc4861#section-6.2.6)
 * @see [RFC 4861, section 6.3.4](https://tools.ietf.org/html/rfc4861#section-6.3.4)
 * @see [RFC 4861, section 7.1](https://tools.ietf.org/html/rfc4861#section-7.1)
 * @see [RFC 4861, section 7.2.3](https://tools.ietf.org/html/rfc4861#section-7.2.3)
 * @see [RFC 4861, section 7.2.5](https://tools.ietf.org/html/rfc4861#section-7.2.5)
 * @see [RFC 4861, section 8.1](https://tools.ietf.org/html/rfc4861#section-8.1)
 * @see [RFC 4861, section 8.3](https://tools.ietf.org/html/rfc4861#section-8.3)
 * @see [RFC 4862, section 5.4.3](https://tools.ietf.org/html/rfc4862#section-5.4.3)
 * @see [RFC 4862, section 5.4.4](https://tools.ietf.org/html/rfc4862#section-5.4.4)
 * @see [RFC 4862, section 5.5.3](https://tools.ietf.org/html/rfc4862#section-5.5.3)
 * @see [RFC 6775, section 5.5.2](https://tools.ietf.org/html/rfc6775#section-5.5.2)
 * @see [RFC 6775, section 5.4](https://tools.ietf.org/html/rfc6775#section-5.4)
 * @see [RFC 6775, section 6.3](https://tools.ietf.org/html/rfc6775#section-6.3)
 * @see [RFC 6775, section 6.5](https://tools.ietf.org/html/rfc6775#section-6.5)
 * @see [RFC 6775, section 8.1.3](https://tools.ietf.org/html/rfc6775#section-8.1.3)
 * @see [RFC 6775, section 8.2.1](https://tools.ietf.org/html/rfc6775#section-8.2.1)
 * @see [RFC 6775, section 8.2.4](https://tools.ietf.org/html/rfc6775#section-8.2.4)
 * @see [RFC 6775, section 8.2.5](https://tools.ietf.org/html/rfc6775#section-8.2.5)
 *
 * @param[in] iface         The interface the packet came over.
 * @param[in] ipv6          The IPv6 header of the received packet.
 * @param[in] icmpv6        The ICMPv6 header and payload of the received
 *                          packet.
 * @param[in] icmpv6_len    The number of bytes at @p icmpv6.
 */
void gnrc_ipv6_nib_handle_pkt(kernel_pid_t iface, const ipv6_hdr_t *ipv6,
                              const icmpv6_hdr_t *icmpv6, size_t icmpv6_len);

/**
 * @brief   Handles a timer event
 *
 * @param[in] ctx   Context of the timer event.
 * @param[in] type  Type of the timer event (see [timer event
 *                  types](@ref net_gnrc_ipv6_nib_msg))
 */
void gnrc_ipv6_nib_handle_timer_event(void *ctx, uint16_t type);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_NIB_H */
/** @} */
