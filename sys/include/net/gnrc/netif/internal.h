/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  net_gnrc_netif
 * @internal
 * @attention   **FOR GNRC-INTERNAL USE ONLY. PLEASE USE @ref net_gnrc_netapi
 *              "NETAPI" (WITH THE INTERFACE'S PID) FOR EXTERNAL MANIPULATION**
 * @{
 *
 * @file
 * @brief   GNRC-internal network interface definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF_INTERNAL_H
#define NET_GNRC_NETIF_INTERNAL_H

#include "net/gnrc/netif.h"

#ifdef MODULE_GNRC_IPV6_NIB
#include "net/gnrc/ipv6/nib/conf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message type for @ref netdev_event_t "netdev events"
 */
#define NETDEV_MSG_TYPE_EVENT   (0x1234)

/**
 * @brief   Acquires exclusive access to the interface
 *
 * @param[in] netif the network interface
 *
 * @internal
 */
void gnrc_netif_acquire(gnrc_netif_t *netif);

/**
 * @brief   Releases exclusive access to the interface
 *
 * @param[in] netif the network interface
 *
 * @internal
 */
void gnrc_netif_release(gnrc_netif_t *netif);

#if defined(MODULE_GNRC_IPV6) || DOXYGEN
/**
 * @brief   Adds an IPv6 address to the interface
 *
 * @pre `(netif != NULL) && (addr != NULL)`
 * @pre @p addr is not multicast (starts with `ff00::/8`), unspecified (`::`),
 *      or loopback
 * @pre `(pfx_len > 0) && (pfx_len <= 128)`
 *
 * @param[in,out] netif the network interface. Must not be NULL.
 * @param[in] addr      the address to add. If the address is already on the
 *                      interface the function will return 0, but @p flags
 *                      will be ignored. Must not be NULL or be a link-local or
 *                      multicast address.
 * @param[in] pfx_len   length in bits of the prefix of @p addr
 * @param[in] flags     initial flags for the address.
 *                      - Setting the address' state to
 *                        @ref GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_TENTATIVE
 *                        means that this address is assumed to be added due to
 *                        stateless auto-address configuration and actions
 *                        related to that may be performed in the background.
 *                      - Setting the address' state to
 *                        @ref GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID means
 *                        that the address is assumed to be manually configured
 *                        and its prefix will be added to the node's prefix
 *                        list (valid and preferred lifetime will be set to
 *                        infinite, but can be changed using
 *                        @ref gnrc_ipv6_nib_pl_set()).
 *
 *
 * @note    Only available with @ref net_gnrc_ipv6 "gnrc_ipv6".
 *
 * @return  >= 0, on success
 * @return  -ENOMEM, when no space for new addresses (or its solicited nodes
 *          multicast address) is left on the interface
 */
int gnrc_netif_ipv6_addr_add_internal(gnrc_netif_t *netif,
                                      const ipv6_addr_t *addr,
                                      unsigned pfx_len, uint8_t flags);

/**
 * @brief   Removes an IPv6 address from the interface
 *
 * @pre `(netif != NULL) && (addr != NULL)`
 *
 * @param[in,out] netif the network interface
 * @param[in] addr      the address to remove
 *
 * @note    Only available with @ref net_gnrc_ipv6 "gnrc_ipv6".
 */
void gnrc_netif_ipv6_addr_remove_internal(gnrc_netif_t *netif,
                                          const ipv6_addr_t *addr);


/**
 * @brief   Returns the index of @p addr in gnrc_netif_t::ipv6_addrs of @p
 *          netif
 *
 * @pre `(netif != NULL) && (addr != NULL)`
 *
 * Can be used to check if an address is assigned to an interface.
 *
 * @param[in] netif the network interface
 * @param[in] addr  the address to check
 *
 * @note    Only available with @ref net_gnrc_ipv6 "gnrc_ipv6".
 *
 * @return  index of @p addr in gnrc_netif_t::ipv6_addrs of @p netif
 * @return  -1, if @p addr isn't assigned to @p netif
 */
int gnrc_netif_ipv6_addr_idx(gnrc_netif_t *netif,
                             const ipv6_addr_t *addr);

/**
 * @brief   Gets state from address flags
 *
 * @param[in] netif the network interface
 * @param[in] idx   index of the address flags
 *
 * @return  the state of the address at @p idx
 */
static inline uint8_t gnrc_netif_ipv6_addr_get_state(const gnrc_netif_t *netif,
                                                     int idx)
{
    return netif->ipv6.addrs_flags[idx] & GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK;
}

/**
 * @brief   Gets number of duplicate address detection transmissions already
 *          performed for an address
 *
 * @param[in] netif the network interface
 * @param[in] idx   index of the address (and its flags)
 *
 * @return  the number of duplicate address detection transmissions already
 *          performed
 */
static inline uint8_t gnrc_netif_ipv6_addr_dad_trans(const gnrc_netif_t *netif,
                                                     int idx)
{
    return netif->ipv6.addrs_flags[idx] & GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_TENTATIVE;
}

/**
 * @brief   Returns the index of an address in gnrc_netif_t::ipv6_addrs of @p
 *          netif that matches @p addr best
 *
 * @pre `(netif != NULL) && (addr != NULL)`
 *
 * Can be used to check if a prefix is assigned to an interface.
 *
 * @param[in] netif     the network interface
 * @param[in] addr      the prefix to match
 *
 *
 * @note    Only available with @ref net_gnrc_ipv6 "gnrc_ipv6".
 *
 * @return  index of an address in gnrc_netif_t::ipv6_addrs of @p netif that
 *          best matches @p addr.
 * @return  -1, if no address on @p netif matches @p addr
 */
int gnrc_netif_ipv6_addr_match(gnrc_netif_t *netif,
                               const ipv6_addr_t *addr);

/**
 * @brief   Searches for the best address on an interface usable as a source
 *          address for a given destination address.
 *
 * @pre `(netif != NULL) && (dst != NULL)`
 *
 * @param[in] netif     the network interface
 * @param[in] dst       the destination address you want to find a source for.
 * @param[in] ll_only   only link local addresses qualify
 *
 * @see     [RFC 6724](https://tools.ietf.org/html/rfc6724)
 *
 * @note    Only available with @ref net_gnrc_ipv6 "gnrc_ipv6".
 *
 * @todo Rule 4 from RFC 6724 is currently not implemented. Has to updated as
 *       soon as gnrc supports Mobile IP.
 *
 * @todo Rule 6 from RFC 6724 is currently not implemented. Has to updated as
 *       soon as gnrc supports flow labels.
 *
 * @todo Rule 7 from RFC 6724 is currently not implemented. Has to updated as
 *       soon as gnrc supports temporary addresses.
 *
 * @return  The best source address for a packet addressed to @p dst
 * @return  NULL, if no matching address can be found on the interface.
 */
ipv6_addr_t *gnrc_netif_ipv6_addr_best_src(gnrc_netif_t *netif,
                                           const ipv6_addr_t *dst,
                                           bool ll_only);

/**
 * @brief   Gets an interface by an address (incl. multicast groups) assigned
 *          to it.
 *
 * @pre `addr != NULL`
 *
 * @param[in] addr  an IPv6 address
 *
 * @return  The network interface that has @p addr assigned
 * @return  NULL, if no interface has @p addr assigned
 */
gnrc_netif_t *gnrc_netif_get_by_ipv6_addr(const ipv6_addr_t *addr);

/**
 * @brief   Gets an interface by an address matching a given prefix best
 *
 * @param[in] prefix    an IPv6 address or prefix
 *
 * @return  The network interface that has an address assigned, that matches
 *          @p prefix best
 * @return  NULL, if there is no address on any interface that matches @prefix
 */
gnrc_netif_t *gnrc_netif_get_by_prefix(const ipv6_addr_t *prefix);

/**
 * @brief   Joins interface to an IPv6 multicast group
 *
 * @pre `(netif != NULL) && (addr != NULL)`
 * @pre @p addr is a multicast address (starts with `ff00::/8`)
 *
 * @param[in,out] netif the network interface
 * @param[in] addr      the address of the multicast group
 *
 * @note    Only available with @ref net_gnrc_ipv6 "gnrc_ipv6".
 *
 * @return  0, on success
 * @return  -ENOMEM, when no space for new addresses is left on the interface
 */
int gnrc_netif_ipv6_group_join_internal(gnrc_netif_t *netif,
                                        const ipv6_addr_t *addr);

/**
 * @brief   Let interface leave from an IPv6 multicast group
 *
 * @pre `(netif != NULL) && (addr != NULL)`
 *
 * @param[in,out] netif the network interface
 * @param[in] addr      the address of the multicast group
 *
 * @note    Only available with @ref net_gnrc_ipv6 "gnrc_ipv6".
 */
void gnrc_netif_ipv6_group_leave_internal(gnrc_netif_t *netif,
                                          const ipv6_addr_t *addr);

/**
 * @brief   Returns the index of @p addr in gnrc_netif_t::ipv6_groups of @p
 *          netif
 *
 * @pre `(netif != NULL) && (addr != NULL)`
 *
 * Can be used to check if a multicast address is assigned to an interface.
 *
 * @param[in] netif the network interface
 * @param[in] addr  the multicast address to check
 *
 * @note    Only available with @ref net_gnrc_ipv6 "gnrc_ipv6".
 *
 * @return  index of @p addr in gnrc_netif_t::ipv6_groups of @p netif
 * @return  -1, if @p netif is not in group @p addr
 */
int gnrc_netif_ipv6_group_idx(gnrc_netif_t *netif,
                              const ipv6_addr_t *addr);

/**
 * @brief   Gets interface identifier (IID) of an interface's link-layer address
 *
 * @param[in] netif     the network interface
 * @param[out] eui64    the IID
 *
 * @return  0, on success
 * @return  -ENOTSUP, if interface has no link-layer address or if
 *          gnrc_netif_t::device_type is not supported.
 */
int gnrc_netif_ipv6_get_iid(gnrc_netif_t *netif, eui64_t *eui64);
#endif  /* MODULE_GNRC_IPV6 */

/**
 * @brief   Checks if the interface represents a router according to RFC 4861
 *
 * @attention   Requires prior locking
 * @note        Assumed to be false, when `gnrc_ipv6_router` module is not
 *              included.
 *
 * @param[in] netif the network interface
 *
 * @see [RFC 4861, section 2.1](https://tools.ietf.org/html/rfc4861#section-2.1)
 *
 * @return  true, if the interface represents a router
 * @return  false, if the interface does not represent a router
 */
#if defined(MODULE_GNRC_IPV6_ROUTER) || defined(DOXYGEN)
static inline bool gnrc_netif_is_rtr(const gnrc_netif_t *netif)
{
    return (netif->flags & GNRC_NETIF_FLAGS_IPV6_FORWARDING);
}
#else
#define gnrc_netif_is_rtr(netif)                (false)
#endif

/**
 * @brief   Checks if the interface is allowed to send out router advertisements
 *
 * @attention   Requires prior locking
 * @note        Assumed to be false, when `gnrc_ipv6_router` module is not
 *              included.
 *
 * @param[in] netif the network interface
 *
 * @return  true, if the interface is allowed to send out router advertisements
 * @return  false, if the interface is not allowed to send out router
 *          advertisements
 */
#if defined(MODULE_GNRC_IPV6_ROUTER) || defined(DOXYGEN)
static inline bool gnrc_netif_is_rtr_adv(const gnrc_netif_t *netif)
{
    return (netif->flags & GNRC_NETIF_FLAGS_IPV6_RTR_ADV);
}
#else
#define gnrc_netif_is_rtr_adv(netif)            (false)
#endif

/**
 * @brief   Checks if the interface represents a 6Lo node (6LN) according to
 *          RFC 6775
 *
 * @attention   Requires prior locking
 * @note        Assumed to be true, when @ref GNRC_NETIF_NUMOF == 1 and
 *              @ref net_gnrc_sixlowpan module is included (and
 *              @ref GNRC_IPV6_NIB_CONF_6LN is not 0, otherwise assumed to be
 *              false).
 *
 * @param[in] netif the network interface
 *
 * @see [RFC 6775, section 2](https://tools.ietf.org/html/rfc6775#section-2)
 *
 * @return  true, if the interface represents a 6LN
 * @return  false, if the interface does not represent a 6LN
 */
#if (GNRC_NETIF_NUMOF > 1) || !defined(MODULE_GNRC_SIXLOWPAN) || defined(DOXYGEN)
bool gnrc_netif_is_6ln(const gnrc_netif_t *netif);
#elif GNRC_IPV6_NIB_CONF_6LN
#define gnrc_netif_is_6ln(netif)                (true)
#else
#define gnrc_netif_is_6ln(netif)                (false)
#endif

/**
 * @brief   Checks if the interface represents a 6Lo router (6LR) according to
 *          RFC 6775
 *
 * @attention   Requires prior locking
 * @note        Assumed to be false, when @ref GNRC_IPV6_NIB_CONF_6LR == 0
 *
 * @param[in] netif the network interface
 *
 * @see [RFC 6775, section 2](https://tools.ietf.org/html/rfc6775#section-2)
 *
 * @return  true, if the interface represents a 6LR
 * @return  false, if the interface does not represent a 6LR
 */
#if (GNRC_IPV6_NIB_CONF_6LR && \
     /* if flag checkers even evaluate, otherwise just assume their result */ \
     (defined(MODULE_GNRC_IPV6_ROUTER) || \
      (GNRC_NETIF_NUMOF > 1) || !defined(MODULE_GNRC_SIXLOWPAN))) || \
    defined(DOXYGEN)
static inline bool gnrc_netif_is_6lr(const gnrc_netif_t *netif)
{
    return gnrc_netif_is_rtr(netif) && gnrc_netif_is_6ln(netif);
}
#else
#define gnrc_netif_is_6lr(netif)                (false)
#endif

/**
 * @brief   Checks if the interface represents a 6Lo border router (6LBR)
 *          according to RFC 6775
 *
 * @attention   Requires prior locking
 * @note        Assumed to be false, when @ref GNRC_IPV6_NIB_CONF_6LBR == 0.
 *
 * @param[in] netif the network interface
 *
 * @see [RFC 6775, section 2](https://tools.ietf.org/html/rfc6775#section-2)
 *
 * @return  true, if the interface represents a 6LBR
 * @return  false, if the interface does not represent a 6LBR
 */
#if GNRC_IPV6_NIB_CONF_6LBR
static inline bool gnrc_netif_is_6lbr(const gnrc_netif_t *netif)
{
    return (netif->flags & GNRC_NETIF_FLAGS_6LO_ABR) &&
           gnrc_netif_is_6lr(netif);
}
#else
#define gnrc_netif_is_6lbr(netif)               (false)
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_INTERNAL_H */
/** @} */
