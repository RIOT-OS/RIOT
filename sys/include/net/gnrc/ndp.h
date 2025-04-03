/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_GNRC_NDP_H
#define NET_GNRC_NDP_H
/**
 * @defgroup    net_gnrc_ndp   IPv6 neighbor discovery (v2)
 * @ingroup     net_gnrc_ipv6
 * @brief       Provides build and send functions for neighbor discovery packets
 * @{
 *
 * @file
 * @brief       GNRC-specific neighbor discovery definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>

#include "net/gnrc/pkt.h"
#include "net/gnrc/netif.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   @ref net_gnrc_nettype to send NDP packets to
 */
#ifndef GNRC_NETTYPE_NDP
# if    defined(MODULE_GNRC_IPV6) || DOXYGEN
#  define GNRC_NETTYPE_NDP (GNRC_NETTYPE_IPV6)      /* usual configuration */
# else
#  define GNRC_NETTYPE_NDP (GNRC_NETTYPE_UNDEF)     /* for testing */
# endif
#endif  /* GNRC_NETTYPE_NDP */

/**
 * @brief   Builds a neighbor solicitation message for sending.
 *
 * @pre `(tgt != NULL) && !ipv6_addr_is_multicast(tgt)`
 *
 * @see [RFC 4861, section 4.3](https://tools.ietf.org/html/rfc4861#section-4.3)
 *
 * @param[in] tgt       The target address of the neighbor solicitation.
 *                      Must not be NULL or a multicast address.
 * @param[in] options   Options to append to the neighbor solicitation.
 *                      May be NULL for none.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, if packet buffer is full.
 */
gnrc_pktsnip_t *gnrc_ndp_nbr_sol_build(const ipv6_addr_t *tgt,
                                       gnrc_pktsnip_t *options);

/**
 * @brief   Builds a neighbor advertisement message for sending.
 *
 * @pre `(tgt != NULL) && !ipv6_addr_is_multicast(tgt)`
 *
 * @see [RFC 4861, section 4.4](https://tools.ietf.org/html/rfc4861#section-4.4")
 *
 * @param[in] tgt       For solicited advertisements, the Target Address field
 *                      in the neighbor solicitaton.
 *                      For and unsolicited advertisement, the address whose
 *                      link-layer address has changed.
 *                      Must not be NULL or a multicast address.
 * @param[in] flags     Neighbor advertisement flags:
 *                      - @ref NDP_NBR_ADV_FLAGS_R == 1 indicates, that the
 *                        sender is a router,
 *                      - @ref NDP_NBR_ADV_FLAGS_S == 1 indicates that the
 *                        advertisement was sent in response to a neighbor
 *                        solicitation,
 *                      - @ref NDP_NBR_ADV_FLAGS_O == 1 indicates that the
 *                        advertisement should override an existing cache entry
 *                      and update the cached link-layer address.
 * @param[in] options   Options to append to the neighbor advertisement.
 *                      May be NULL for none.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, if packet buffer is full.
 */
gnrc_pktsnip_t *gnrc_ndp_nbr_adv_build(const ipv6_addr_t *tgt, uint8_t flags,
                                       gnrc_pktsnip_t *options);

/**
 * @brief   Builds a router solicitation message for sending.
 *
 * @see `[RFC 4861, section 4.1](https://tools.ietf.org/html/rfc4861#section-4.1")
 *
 * @param[in] options   Options to append to the router solicitation.
 *                      May be NULL for none.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, if packet buffer is full.
 */
gnrc_pktsnip_t *gnrc_ndp_rtr_sol_build(gnrc_pktsnip_t *options);

/**
 * @brief   Builds a router advertisement message for sending.
 *
 * @see `[RFC 4861, section 4.2](https://tools.ietf.org/html/rfc4861#section-4.2")
 *
 * @note    The source address for the packet MUST be the link-local address
 *          of the interface.
 *
 * @param[in] cur_hl        Default hop limit for outgoing IP packets, 0 if
 *                          unspecified by this router.
 * @param[in] flags         Flags as defined in net/ndp.h.
 *                          - @ref NDP_RTR_ADV_FLAGS_M == 1 indicates, that the
 *                            addresses are managed by DHCPv6,
 *                          - @ref NDP_RTR_ADV_FLAGS_O == 1 indicates that other
 *                            configuration information is available via DHCPv6.
 * @param[in] ltime         Lifetime of the default router in seconds.
 * @param[in] reach_time    Time in milliseconds a node should assume a neighbor
 *                          reachable. 0 means unspecified by the router.
 * @param[in] retrans_timer Time in milliseconds between retransmitted
 *                          neighbor solicitations. 0 means unspecified by
 *                          the router.
 * @param[in] options       Options to append to the router advertisement.
 *                          May be NULL for none.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, if packet buffer is full.
 */
gnrc_pktsnip_t *gnrc_ndp_rtr_adv_build(uint8_t cur_hl, uint8_t flags,
                                       uint16_t ltime, uint32_t reach_time,
                                       uint32_t retrans_timer,
                                       gnrc_pktsnip_t *options);

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
gnrc_pktsnip_t *gnrc_ndp_opt_build(uint8_t type, size_t size,
                                   gnrc_pktsnip_t *next);

/**
 * @brief   Builds the source link-layer address option.
 *
 * @pre `(l2addr != NULL) && (l2addr_len != 0)`
 *
 * @see [RFC 4861, section 4.6.1](https://tools.ietf.org/html/rfc4861#section-4.6.1)
 *
 * @note    Should only be used with neighbor solicitations, router solicitations,
 *          and router advertisements. This is not checked however, since
 *          hosts should silently ignore it in other NDP messages.
 *
 * @param[in] l2addr        A link-layer address of variable length.
 *                          Must not be NULL.
 * @param[in] l2addr_len    Length of @p l2addr. Must not be 0.
 * @param[in] next          More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
gnrc_pktsnip_t *gnrc_ndp_opt_sl2a_build(const uint8_t *l2addr,
                                        uint8_t l2addr_len,
                                        gnrc_pktsnip_t *next);

/**
 * @brief   Builds the target link-layer address option.
 *
 * @pre `(l2addr != NULL) && (l2addr_len != 0)`
 *
 * @see [RFC 4861, section 4.6.1](https://tools.ietf.org/html/rfc4861#section-4.6.1)
 *
 * @note    Should only be used with neighbor advertisemnents and redirect packets.
 *          This is not checked however, since hosts should silently ignore it
 *          in other NDP messages.
 *
 * @param[in] l2addr        A link-layer address of variable length.
 *                          Must not be NULL.
 * @param[in] l2addr_len    Length of @p l2addr. Must not be 0.
 * @param[in] next          More options in the packet. NULL, if there are none.
 *
 * @return  The pkt snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
gnrc_pktsnip_t *gnrc_ndp_opt_tl2a_build(const uint8_t *l2addr,
                                        uint8_t l2addr_len,
                                        gnrc_pktsnip_t *next);

/**
 * @brief   Builds the prefix information option.
 *
 * @pre `prefix != NULL`
 * @pre `!ipv6_addr_is_link_local(prefix) && !ipv6_addr_is_multicast(prefix)`
 * @pre `prefix_len <= 128`
 *
 * @see [RFC 4861, section 4.6.2](https://tools.ietf.org/html/rfc4861#section-4.6.2)
 *
 * @note    Should only be used with router advertisemnents. This is not checked
 *          however, since nodes should silently ignore it in other NDP messages.
 *
 * @param[in] prefix        An IPv6 address or a prefix of an IPv6 address.
 *                          Must not be NULL or be a link-local or
 *                          multicast address.
 * @param[in] prefix_len    The length of @p prefix in bits. Must be between
 *                          0 and 128.
 * @param[in] valid_ltime   Length of time in seconds that @p prefix is valid.
 *                          UINT32_MAX represents infinity.
 * @param[in] pref_ltime    Length of time in seconds that addresses using
 *                          @p prefix remain preferred. UINT32_MAX represents
 *                          infinity.
 * @param[in] flags         Flags as defined in net/ndp.h.
 *                          - @ref NDP_OPT_PI_FLAGS_L == 1 indicates, that
 *                            @p prefix can be used for on-link determination,
 *                          - @ref NDP_OPT_PI_FLAGS_A == 1 indicates, that
 *                            @p prefix can be used for stateless address
 *                          configuration.
 * @param[in] next          More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
gnrc_pktsnip_t *gnrc_ndp_opt_pi_build(const ipv6_addr_t *prefix,
                                      uint8_t prefix_len,
                                      uint32_t valid_ltime, uint32_t pref_ltime,
                                      uint8_t flags, gnrc_pktsnip_t *next);
/**
 * @brief   Builds the route information option.
 *
 * @pre `prefix != NULL`
 * @pre `!ipv6_addr_is_link_local(prefix) && !ipv6_addr_is_multicast(prefix)`
 * @pre `prefix_len <= 128`
 *
 * @see [RFC 4191, section 2.3](https://tools.ietf.org/html/rfc4191#section-2.3)
 *
 * @note    Should only be used with router advertisemnents. This is not checked
 *          however, since nodes should silently ignore it in other NDP messages.
 *
 * @param[in] prefix        An IPv6 address or a prefix of an IPv6 address.
 *                          Must not be NULL or be a link-local or
 *                          multicast address.
 * @param[in] prefix_len    The length of @p prefix in bits. Must be between
 *                          0 and 128.
 * @param[in] route_ltime   Length of time in seconds that @p prefix is valid.
 *                          UINT32_MAX represents infinity.
 * @param[in] flags         Flags as defined in net/ndp.h.
 * @param[in] next          More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
gnrc_pktsnip_t *gnrc_ndp_opt_ri_build(const ipv6_addr_t *prefix,
                                      uint8_t prefix_len,
                                      uint32_t route_ltime,
                                      uint8_t flags, gnrc_pktsnip_t *next);

/**
 * @brief   Builds the MTU option.
 *
 * @see [RFC 4861, section 4.6.4](https://tools.ietf.org/html/rfc4861#section-4.6.4)
 *
 * @note    Should only be used with router advertisemnents. This is not checked
 *          however, since nodes should silently ignore it in other NDP messages.
 *
 * @param[in] mtu           The recommended MTU for the link.
 * @param[in] next          More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
gnrc_pktsnip_t *gnrc_ndp_opt_mtu_build(uint32_t mtu, gnrc_pktsnip_t *next);

/**
 * @brief   Builts the recursive DNS server option
 *
 * @see [RFC 8106, section 5.1](https://tools.ietf.org/html/rfc8106#section-5.1)
 * @pre `addrs != NULL`
 * @pre `addrs_num > 0`
 *
 * @note    Should only be used with router advertisemnents. This is not checked
 *          however, since nodes should silently ignore it in other NDP messages.
 *
 * @param[in] lifetime      The lifetime of the recursive DNS servers
 * @param[in] addrs         The addresses of the recursive DNS servers
 * @param[in] addrs_num     The number of addresses in @p addrs
 * @param[in] next          More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  @p next, if RDNSS is not supported
 * @return  NULL, if packet buffer is full
 */
gnrc_pktsnip_t *gnrc_ndp_opt_rdnss_build(uint32_t lifetime, ipv6_addr_t *addrs,
                                         unsigned addrs_num,
                                         gnrc_pktsnip_t *next);

/**
 * @brief   Send pre-compiled neighbor solicitation depending on a given network
 *          interface.
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
 * @param[in] ext_opts  External options for the neighbor advertisement.
 *                      Leave NULL for none.
 *                      **Warning:** these are not tested if they are suitable
 *                      for a neighbor solicitation so be sure to check that.
 *                      **Will be released** in an error case.
 */
void gnrc_ndp_nbr_sol_send(const ipv6_addr_t *tgt, gnrc_netif_t *netif,
                           const ipv6_addr_t *src, const ipv6_addr_t *dst,
                           gnrc_pktsnip_t *ext_opts);

/**
 * @brief   Send pre-compiled neighbor advertisement depending on a given
 *          network interface.
 *
 * @pre `(tgt != NULL) && !ipv6_addr_is_multicast(tgt)`
 * @pre `(netif != NULL) && (dst != NULL)`
 *
 * If @p netif is a forwarding interface and router advertisements are
 * activated the @ref NDP_NBR_ADV_FLAGS_R is set in the neighbor advertisement.
 * If @p dst is @ref IPV6_ADDR_UNSPECIFIED it will be replaced with
 * @ref IPV6_ADDR_ALL_NODES_LINK_LOCAL and* @p supply_tl2a is set to true
 * implicitly. Otherwise, the @ref NDP_NBR_ADV_FLAGS_S will be set. If @p tgt
 * is an anycast address on @p netif the @ref NDP_NBR_ADV_FLAGS_O flag will be
 * set.
 *
 * The source address of the IPv6 packet will be left unspecified, so the
 * @ref net_gnrc_ipv6 "IPv6 module" selects a fitting IPv6 address.
 *
 * @param[in] tgt           Target address for the neighbor advertisement. May
 *                          not be NULL and **MUST NOT** be multicast.
 * @param[in] netif         Interface to send over. Must not be NULL.
 * @param[in] dst           Destination address for neighbor advertisement. May
 *                          not be NULL. Is set to
 *                          @ref IPV6_ADDR_ALL_NODES_LINK_LOCAL when equal to
 *                          @ref IPV6_ADDR_UNSPECIFIED (to allow for simple
 *                          reply mechanisms to neighbor solicitations). This
 *                          also implies that @p supply_tl2a **must** be true
 *                          and the parameter will be reset accordingly. If
 *                          @p dst is not @ref IPV6_ADDR_UNSPECIFIED, the
 *                          @ref NDP_NBR_ADV_FLAGS_S flag will be set
 *                          implicitly.
 * @param[in] supply_tl2a   Add target link-layer address option to neighbor
 *                          advertisement if link-layer has addresses.
 *                          If @p dst is @ref IPV6_ADDR_UNSPECIFIED, it will be
 *                          set to true.
 * @param[in] ext_opts      External options for the neighbor advertisement.
 *                          Leave NULL for none.
 *                          **Warning:** these are not tested if they are
 *                          suitable for a neighbor advertisement so be sure to
 *                          check that.
 *                          **Will be released** in an error case.
 */
void gnrc_ndp_nbr_adv_send(const ipv6_addr_t *tgt, gnrc_netif_t *netif,
                           const ipv6_addr_t *dst, bool supply_tl2a,
                           gnrc_pktsnip_t *ext_opts);

/**
 * @brief   Send pre-compiled router solicitation depending on a given
 *          network interface.
 *
 * @pre `(netif != NULL)`
 *
 * @param[in] netif Interface to send over. Must not be NULL.
 * @param[in] dst   Destination for the router solicitation. ff02::2 if NULL.
 */
void gnrc_ndp_rtr_sol_send(gnrc_netif_t *netif, const ipv6_addr_t *dst);

/**
 * @brief   Send pre-compiled router advertisement depending on a given network
 *          interface.
 *
 * @pre `(netif != NULL)`
 *
 * This function does not add the PIOs to the router, since they are highly
 * dependent on external set-ups (e.g. if multihop prefix distribution is used).
 * Provide them via @p ext_opts
 *
 * @param[in] netif     Interface to send over. Must not be NULL.
 * @param[in] src       Source address for the router advertisement. May be
 *                      NULL to be determined by source address selection
 *                      (:: if @p netif has no address).
 * @param[in] dst       Destination address for router advertisement.
 *                      ff02::1 if NULL.
 * @param[in] fin       This is part of the router's final batch of router
 *                      advertisements before ceising to be a router (set's
 *                      router lifetime field to 0).
 * @param[in] ext_opts  External options for the neighbor advertisement.
 *                      Leave NULL for none.
 *                      **Warning:** these are not tested if they are suitable
 *                      for a neighbor advertisement so be sure to check that.
 *                      **Will be released** in an error case.
 */
void gnrc_ndp_rtr_adv_send(gnrc_netif_t *netif, const ipv6_addr_t *src,
                           const ipv6_addr_t *dst, bool fin,
                           gnrc_pktsnip_t *ext_opts);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_NDP_H */
