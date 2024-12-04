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
 * @todo    Implement MLD
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
#include "net/gnrc/netif.h"
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
 * @brief   Reply Router Solicitation event.
 *
 * This message type is for the event of the delayed reply to a Router
 * Solicitaion with a Router Advertisement. The expected message context is a
 * pointer to a valid on-link entry representing the neighbor that sent the
 * Router Solicitation.
 *
 * @note    Only handled with @ref CONFIG_GNRC_IPV6_NIB_ROUTER != 0
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
 * @note    Only handled with @ref CONFIG_GNRC_IPV6_NIB_ROUTER != 0
 */
#define GNRC_IPV6_NIB_SND_MC_RA             (0x4fc6U)

/**
 * @brief   Reachability timeout event.
 *
 * This message type is for the event of a REACHABLE state timeout.
 * The expected message context is a pointer to a valid on-link entry
 * representing the neighbor cache entry that faces a state change.
 *
 * @note    Only handled with @ref CONFIG_GNRC_IPV6_NIB_ARSM != 0
 */
#define GNRC_IPV6_NIB_REACH_TIMEOUT         (0x4fc7U)

/**
 * @brief   Delay timeout event.
 *
 * This message type is for the event of the DELAY state timeout.
 * The expected message context is a pointer to a valid on-link entry
 * representing the neighbor cache entry that faces a state change.
 *
 * @note    Only handled with @ref CONFIG_GNRC_IPV6_NIB_ARSM != 0
 */
#define GNRC_IPV6_NIB_DELAY_TIMEOUT         (0x4fc8U)

/**
 * @brief   Address registration timeout event.
 *
 * This message type is for the event of a 6LoWPAN address registration state
 * timeout. The expected message context is a pointer to a valid on-link entry
 * representing the neighbor which faces a timeout of its address registration.
 *
 * @note    Only handled with @ref CONFIG_GNRC_IPV6_NIB_6LR != 0
 */
#define GNRC_IPV6_NIB_ADDR_REG_TIMEOUT      (0x4fc9U)

/**
 * @brief   Authoritative border router timeout event.
 *
 * This message type is for the event of an Authoritative border router timeout.
 * The expected message context is the NIB-internal state of the authoritative
 * border router.
 *
 * @note    Only handled with @ref CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C != 0
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
 * time. The expected message context is a valid
 * [interface](@ref net_gnrc_netif).
 *
 * @note    Only handled with @ref CONFIG_GNRC_IPV6_NIB_ARSM != 0
 */
#define GNRC_IPV6_NIB_RECALC_REACH_TIME     (0x4fceU)

/**
 * @brief   Reregister address.
 *
 * This message type is for the event of reregistering an IPv6 address to the
 * upstream router. The expected message context is an IPv6 address assigned to
 * one of the nodes interfaces.
 *
 * @note    Only handled with @ref CONFIG_GNRC_IPV6_NIB_6LN != 0
 */
#define GNRC_IPV6_NIB_REREG_ADDRESS         (0x4fcfU)

/**
 * @brief   Route timeout event.
 *
 * This message type is for the event of a route timeout. The expected message
 * context is a valid off-link entry representing the route.
 *
 * @note    Only handled with @ref CONFIG_GNRC_IPV6_NIB_ROUTER != 0
 */
#define GNRC_IPV6_NIB_ROUTE_TIMEOUT         (0x4fd0U)

/**
 * @brief   Perform DAD event.
 *
 * This message type is for performing DAD for a given address. The expected
 * message context is a TENTATIVE IPv6 address.
 *
 * @note    Only handled with @ref CONFIG_GNRC_IPV6_NIB_SLAAC != 0
 */
#define GNRC_IPV6_NIB_DAD                   (0x4fd1U)

/**
 * @brief   Validate a tentative address event.
 *
 * Moves a TENTATIVE address to VALID state. The expected message context is a
 * TENTATIVE IPv6 address.
 *
 * @note    Only handled with @ref CONFIG_GNRC_IPV6_NIB_SLAAC != 0
 */
#define GNRC_IPV6_NIB_VALID_ADDR            (0x4fd2U)

/**
 * @brief   Recursive DNS server timeout
 *
 * This message type is for the event of a recursive DNS server timeout.
 * The expected message context is the [UDP end point](@ref sock_udp_ep_t)
 * representing the DNS server.
 *
 * @note    Only handled with @ref CONFIG_GNRC_IPV6_NIB_DNS != 0
 */
#define GNRC_IPV6_NIB_RDNSS_TIMEOUT         (0x4fd3U)

/**
 * @brief   Interface up event
 */
#define GNRC_IPV6_NIB_IFACE_UP              (0x4fd4U)

/**
 * @brief   Interface down event
 */
#define GNRC_IPV6_NIB_IFACE_DOWN            (0x4fd5U)
/** @} */

/**
 * @brief   Types for gnrc_netif_ipv6_t::route_info_cb
 * @anchor  net_gnrc_ipv6_nib_route_info_type
 */
enum {
    GNRC_IPV6_NIB_ROUTE_INFO_TYPE_UNDEF = 0,    /**< undefined */
    /**
     * @brief   reactive routing query
     *
     * A reactive routing query is issued when a route is unknown to the NIB.
     * A reactive routing protocol can use this call to search for a route in a
     * reactive manner.
     *
     * The `ctx_addr` will be the destination address of the unknown route,
     * `ctx` a pointer to the packet as `gnrc_pktsnip_t` that caused the route
     * look-up (to possibly queue it for later sending).
     */
    GNRC_IPV6_NIB_ROUTE_INFO_TYPE_RRQ,

    /**
     * @brief   route notification
     *
     * A route notification is issued when an already established route is
     * taken. A routing protocol can use this call to update its information on
     * the route.
     *
     * The `ctx_addr` is the prefix of the route, `ctx` is set to a value equal
     * to the length of the prefix in bits.
     */
    GNRC_IPV6_NIB_ROUTE_INFO_TYPE_RN,

    /**
     * @brief   neighbor state change
     *
     * A neighbor state change is issued when ever the NUD state of a neighbor
     * changes. A routing protocol can use this call to update its information
     * on routes via this neighbor.
     *
     * The `ctx_addr` is the address of the neighbor, `ctx` is a value equal
     * to the new NUD state as defined in
     * [the NC info flags](@ref net_gnrc_ipv6_nib_nc_info). If the entry is
     * deleted, `ctx` will be set
     * to @ref GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE (except if it was
     * already in the `UNREACHABLE` state). This does not include cache-outs,
     * since they give no information about the neighbor's reachability (you
     * might however get an INCOMPLETE or STALE notification due to that, as
     * soon as the neighbor enters the neighbor cache again).
     *
     * Be advised to only use `ctx_addr` in the context of the callback, since
     * it might be overwritten, after the callback was left.
     */
    GNRC_IPV6_NIB_ROUTE_INFO_TYPE_NSC,
};

/**
 * @brief   Initialize NIB
 */
void gnrc_ipv6_nib_init(void);

/**
 * @brief   Adds an interface to be managed by the NIB.
 *
 * @pre `netif != NULL`
 *
 * @param[in,out] netif The interface to be managed by the NIB
 */
void gnrc_ipv6_nib_init_iface(gnrc_netif_t *netif);

/**
 * @brief   Call bring-up functions when the interface comes online
 *
 * @pre `netif != NULL`
 *
 * @param[in,out] netif The interface that just got online
 */
void gnrc_ipv6_nib_iface_up(gnrc_netif_t *netif);

/**
 * @brief   Clean up when the interface goes offline
 *
 * @pre `netif != NULL`
 *
 * @param[in,out] netif         The interface that has just got offline
 * @param[in]     send_final_ra Whether to advertise router disappearance
 *                              in a final router advertisement
 */
void gnrc_ipv6_nib_iface_down(gnrc_netif_t *netif, bool send_final_ra);

/**
 * @brief   Gets link-layer address of next hop to a destination address
 *
 * @pre `(dst != NULL) && (nce != NULL)`
 *
 * @param[in] dst       Destination address of a packet.
 * @param[in] netif     Restrict search to this interface. May be `NULL` for any
 *                      interface.
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
                                      gnrc_netif_t *netif, gnrc_pktsnip_t *pkt,
                                      gnrc_ipv6_nib_nc_t *nce);

/**
 * @brief   Handles a received ICMPv6 packet
 *
 * @pre `netif != NULL`
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
 * @param[in] netif         The interface the packet came over.
 * @param[in] ipv6          The IPv6 header of the received packet.
 * @param[in] icmpv6        The ICMPv6 header and payload of the received
 *                          packet.
 * @param[in] icmpv6_len    The number of bytes at @p icmpv6.
 */
void gnrc_ipv6_nib_handle_pkt(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                              const icmpv6_hdr_t *icmpv6, size_t icmpv6_len);

/**
 * @brief   Handles a timer event
 *
 * @param[in] ctx   Context of the timer event.
 * @param[in] type  Type of the timer event (see [timer event
 *                  types](@ref net_gnrc_ipv6_nib_msg))
 */
void gnrc_ipv6_nib_handle_timer_event(void *ctx, uint16_t type);

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER) || defined(DOXYGEN)
/**
 * @brief   Changes the state if an interface advertises itself as a router
 *          or not
 *
 * @param[in] netif     The interface for which the state should be changed.
 * @param[in] enable    `true`, to enable advertising the interface as a router.
 *                      `false`, to disable advertising the interface as a
 *                      router.
 */
void gnrc_ipv6_nib_change_rtr_adv_iface(gnrc_netif_t *netif, bool enable);
#else
/**
 * @brief   Optimization to NOP for non-routers
 */
#define gnrc_ipv6_nib_change_rtr_adv_iface(netif, enable) \
    (void)netif; (void)enable
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_IPV6_NIB_H */
