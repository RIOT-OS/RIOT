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

#include <kernel_defines.h>

#include "net/gnrc/netif.h"
#include "net/l2util.h"
#include "net/netopt.h"

#ifdef MODULE_GNRC_IPV6_NIB
#include "net/gnrc/ipv6/nib/conf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message type to send from @ref net_gnrc_netif_pktq
 */
#define GNRC_NETIF_PKTQ_DEQUEUE_MSG     (0x1233)

/**
 * @brief   Message type for @ref netdev_event_t "netdev events"
 */
#define NETDEV_MSG_TYPE_EVENT           (0x1234)

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

#if IS_USED(MODULE_GNRC_NETIF_IPV6) || DOXYGEN
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
 * @brief   Posts a message to the IPv6 event bus of the interface
 *
 * @param[in] netif     Pointer to the interface
 * @param[in] type      [Type of the event](@ref gnrc_ipv6_event_t)
 * @param[in] ctx       The context of the event
 */
static inline void gnrc_netif_ipv6_bus_post(gnrc_netif_t *netif, int type,
                                            const void *ctx)
{
#ifdef MODULE_GNRC_NETIF_BUS
    msg_bus_post(&netif->bus[GNRC_NETIF_BUS_IPV6], type, ctx);
#else
    (void) netif;
    (void) type;
    (void) ctx;
#endif
}
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) || defined(DOXYGEN) */

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
static inline bool gnrc_netif_is_rtr(const gnrc_netif_t *netif)
{
    if (IS_USED(MODULE_GNRC_IPV6_ROUTER)) {
        return (netif->flags & GNRC_NETIF_FLAGS_IPV6_FORWARDING);
    }
    else {
        return false;
    }
}

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
static inline bool gnrc_netif_is_rtr_adv(const gnrc_netif_t *netif)
{
    if (IS_USED(MODULE_GNRC_IPV6_ROUTER)) {
        return (netif->flags & GNRC_NETIF_FLAGS_IPV6_RTR_ADV);
    }
    else {
        return false;
    }
}

/**
 * @brief   Checks if the device type associated to a @ref gnrc_netif_t
 *          requires 6Lo to run
 *
 * @param[in] netif the network interface
 *
 * @return true if the device requires 6Lo
 * @return false otherwise
 */
bool gnrc_netif_dev_is_6lo(const gnrc_netif_t *netif);

/**
 * @brief   Checks if the interface uses a protocol that requires 6Lo to run
 *
 * @attention   Requires prior locking
 * @note        Assumed to be true, when @ref gnrc_netif_highlander() return true and
 *              @ref net_gnrc_sixlowpan module is included. When the
 *              @ref net_gnrc_sixlowpan module is not included, it is assumed
 *              to be false.
 *              Since `gnrc_sixloenc` makes the interface's 6Lo capabilities
 *              configurable for an Ethernet interface, this does not apply,
 *              when that module is compiled in.
 *
 * @param[in] netif the network interface
 *
 * @return  true, if the interface represents a 6LN
 * @return  false, if the interface does not represent a 6LN
 */
static inline bool gnrc_netif_is_6lo(const gnrc_netif_t *netif)
{
    if ((!gnrc_netif_highlander() &&
       IS_USED(MODULE_GNRC_SIXLOWPAN)) || \
       IS_USED(MODULE_GNRC_SIXLOENC)) {
        return gnrc_netif_dev_is_6lo(netif);
    }
    else if (gnrc_netif_highlander() && IS_USED(MODULE_GNRC_SIXLOWPAN)) {
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief   Checks if the interface represents a 6Lo node (6LN) according to
 *          RFC 6775
 *
 * @attention   Requires prior locking
 * @note        Assumed to be false, when @ref CONFIG_GNRC_IPV6_NIB_6LN is 0.
 *
 * @param[in] netif the network interface
 *
 * @see [RFC 6775, section 2](https://tools.ietf.org/html/rfc6775#section-2)
 *
 * @return  true, if the interface represents a 6LN
 * @return  false, if the interface does not represent a 6LN
 */
static inline bool gnrc_netif_is_6ln(const gnrc_netif_t *netif)
{
    if (IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)) {
        return (netif->flags & GNRC_NETIF_FLAGS_6LN);
    }
    else {
        return false;
    }
}

/**
 * @brief   Checks if the interface represents a 6Lo router (6LR) according to
 *          RFC 6775
 *
 * @attention   Requires prior locking
 * @note        Assumed to be false, when @ref CONFIG_GNRC_IPV6_NIB_6LR == 0
 *
 * @param[in] netif the network interface
 *
 * @see [RFC 6775, section 2](https://tools.ietf.org/html/rfc6775#section-2)
 *
 * @return  true, if the interface represents a 6LR
 * @return  false, if the interface does not represent a 6LR
 */
static inline bool gnrc_netif_is_6lr(const gnrc_netif_t *netif)
{
     /* if flag checkers even evaluate, otherwise just assume their result */
    if (IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LR) &&
            (IS_USED(MODULE_GNRC_IPV6_ROUTER) ||
            (!gnrc_netif_highlander()) ||
            !IS_USED(MODULE_GNRC_SIXLOWPAN))) {
        return gnrc_netif_is_rtr(netif) && gnrc_netif_is_6ln(netif);
    }
    else {
        return false;
    }
}

/**
 * @brief   Checks if the interface represents a 6Lo border router (6LBR)
 *          according to RFC 6775
 *
 * @attention   Requires prior locking
 * @note        Assumed to be false, when @ref CONFIG_GNRC_IPV6_NIB_6LBR == 0.
 *
 * @param[in] netif the network interface
 *
 * @see [RFC 6775, section 2](https://tools.ietf.org/html/rfc6775#section-2)
 *
 * @return  true, if the interface represents a 6LBR
 * @return  false, if the interface does not represent a 6LBR
 */
static inline bool gnrc_netif_is_6lbr(const gnrc_netif_t *netif)
{
    if (IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LBR)) {
        return (netif->flags & GNRC_NETIF_FLAGS_6LO_ABR) &&
               gnrc_netif_is_6lr(netif);
    }
    else {
        return false;
    }
}

/**
 * @name    Device type based function
 *
 * These functions' behavior is based around the gnrc_netif_t::device_type of
 * an interface.
 *
 * @attention   Special care needs to be taken for those functions when porting
 *              a new network device type or link-layer protocol: They might
 *              need adaptions for your port
 * @{
 */
/**
 * @brief   Get the default link-layer address option for the given
 *          gnrc_netif_t::device_type of a network interface
 *
 * @param[in] netif     The network interface to get the default link-layer
 *                      address option for.
 *
 * @return  Either @ref NETOPT_ADDRESS or @ref NETOPT_ADDRESS_LONG.
 */
netopt_t gnrc_netif_get_l2addr_opt(const gnrc_netif_t *netif);

/**
 * @brief   Converts a given hardware address to an EUI-64.
 *
 * @attention When the link-layer of the interface has link-layer addresses, and
 *            `NDEBUG` is not defined, the node fails with an assertion instead
 *            returning `-ENOTSUP`.
 *
 * @param[in] netif     The network interface @p addr came from (either as
 *                      gnrc_netif_t::l2addr or from a packet that came over
 *                      it).
 * @param[in] addr      A hardware address.
 * @param[in] addr_len  Number of bytes in @p addr.
 * @param[out] eui64    The EUI-64 based on gnrc_netif_t::device_type
 *
 * @return  `sizeof(eui64_t)` on success.
 * @return  `-ENOTSUP`, when gnrc_netif_t::device_type of @p netif does not
 *          support IID conversion.
 * @return  `-EINVAL`, when @p addr_len is invalid for the
 *          gnrc_netif_t::device_type of @p netif.
 */
int gnrc_netif_eui64_from_addr(const gnrc_netif_t *netif,
                               const uint8_t *addr, size_t addr_len,
                               eui64_t *eui64);

/**
 * @brief   Converts an interface EUI-64 from an interface's hardware address
 *
 * @param[in] netif     The network interface @p eui64 came from
 * @param[out] eui64    The EUI-64 based on gnrc_netif_t::device_type
 *
 * @note    This wraps around @ref gnrc_netif_eui64_from_addr by using by using
 *          gnrc_netif_t::l2addr and gnrc_netif_t::l2addr_len of @p netif.
 *
 * @return  `sizeof(eui64_t)` on success.
 * @return  `-ENOTSUP`, if interface has no link-layer address or if
 *          gnrc_netif_t::device_type is not supported.
 * @return  `-EINVAL`, when gnrc_netif_t::l2addr_len of @p netif is invalid for
 *          the gnrc_netif_t::device_type of @p netif.
 */
static inline int gnrc_netif_get_eui64(gnrc_netif_t *netif, eui64_t *eui64)
{
#if GNRC_NETIF_L2ADDR_MAXLEN > 0
    if (netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR) {
        return gnrc_netif_eui64_from_addr(netif,
                                          netif->l2addr, netif->l2addr_len,
                                          eui64);
    }
#endif /* GNRC_NETIF_L2ADDR_MAXLEN > 0 */
    (void)netif;
    (void)eui64;
    return -ENOTSUP;
}

/**
 * @brief   Initializes an interface as 6LN according to RFC 6775 and according
 *          to its gnrc_netif_t::device_type
 *
 * @param[in] netif The network interface to initialize as 6LN
 */
void gnrc_netif_init_6ln(gnrc_netif_t *netif);

#if IS_USED(MODULE_GNRC_NETIF_IPV6) || defined(DOXYGEN)
/**
 * @brief   Initialize IPv6 MTU and other packet length related members of
 *          @ref gnrc_netif_t based on gnrc_netif_t::device_type
 *
 * @param[in,out] netif The network interface to initialize the MTU for.
 */
void gnrc_netif_ipv6_init_mtu(gnrc_netif_t *netif);

/**
 * @brief   Converts a given hardware address to an IPv6 IID.
 *
 * @note      The IPv6 IID is derived from the EUI-64 for most link-layers by
 *            flipping the U/L bit.
 * @see       [RFC 2464, section 4](https://tools.ietf.org/html/rfc2464#section-4)
 * @attention When the link-layer of the interface has link-layer addresses, and
 *            `NDEBUG` is not defined, the node fails with an assertion instead
 *            returning `-ENOTSUP`.
 *
 * @param[in] netif     The network interface @p addr came from (either as
 *                      gnrc_netif_t::l2addr or from a packet that came over
 *                      it).
 * @param[in] addr      A hardware address.
 * @param[in] addr_len  Number of bytes in @p addr.
 * @param[out] iid      The IID based on gnrc_netif_t::device_type
 *
 * @return  `sizeof(eui64_t)` on success.
 * @return  `-ENOTSUP`, when gnrc_netif_t::device_type of @p netif does not
 *          support IID conversion.
 * @return  `-EINVAL`, when @p addr_len is invalid for the
 *          gnrc_netif_t::device_type of @p netif.
 */
int gnrc_netif_ipv6_iid_from_addr(const gnrc_netif_t *netif,
                                  const uint8_t *addr, size_t addr_len,
                                  eui64_t *iid);

/**
 * @brief   Converts an IPv6 IID to a hardware address
 *
 * @pre `netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR`
 * @pre @p iid was based on a hardware address
 * @pre The number of bytes available at @p addr is less or equal to
 *      @ref GNRC_NETIF_L2ADDR_MAXLEN.
 *
 * @attention   When `NDEBUG` is not defined, the node fails with an assertion
 *              instead of returning `-ENOTSUP`
 *
 * @param[in] netif     The network interface @p iid came from (either because
 *                      it is paset on gnrc_netif_t::l2addr or from a packet
 *                      that came over it).
 * @param[in] iid       An IID based on gnrc_netif_t::device_type.
 * @param[out] addr     The hardware address. It is assumed that @p iid was
 *                      based on a hardware address and that the available bytes
 *                      in @p addr are less or equal to
 *                      `GNRC_NETIF_L2ADDR_MAXLEN`.
 *
 * @return  Length of resulting @p addr on success.
 * @return  `-ENOTSUP`, when gnrc_netif_t::device_type of @p netif does not
 *          support reverse IID conversion.
 */
static inline int gnrc_netif_ipv6_iid_to_addr(const gnrc_netif_t *netif,
                                              const eui64_t *iid, uint8_t *addr)
{
    assert(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR);
    return l2util_ipv6_iid_to_addr(netif->device_type, iid, addr);
}

/**
 * @brief   Converts an interface IID of an interface's hardware address
 *
 * @param[in] netif     The network interface @p iid came from
 * @param[out] iid      The IID based on gnrc_netif_t::device_type
 *
 * @note    The IPv6 IID is derived from the EUI-64 for most link-layers by
 *          flipping the U/L bit.
 * @see     [RFC 2464, section 4](https://tools.ietf.org/html/rfc2464#section-4)
 * @note    This wraps around @ref gnrc_netif_ipv6_iid_from_addr by using
 *          by using gnrc_netif_t::l2addr and gnrc_netif_t::l2addr_len of
 *          @p netif.
 *
 * @return  `sizeof(eui64_t)` on success.
 * @return  `-ENOTSUP`, if interface has no link-layer address or if
 *          gnrc_netif_t::device_type is not supported.
 * @return  `-EINVAL`, when gnrc_netif_t::l2addr_len of @p netif is invalid for
 *          the gnrc_netif_t::device_type of @p netif.
 */
static inline int gnrc_netif_ipv6_get_iid(gnrc_netif_t *netif, eui64_t *iid)
{
#if GNRC_NETIF_L2ADDR_MAXLEN > 0
    if (netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR) {
        return gnrc_netif_ipv6_iid_from_addr(netif,
                                             netif->l2addr, netif->l2addr_len,
                                             iid);
    }
#endif /* GNRC_NETIF_L2ADDR_MAXLEN > 0 */
    (void)netif;
    (void)iid;
    return -ENOTSUP;
}

/**
 * @brief   Derives the length of the link-layer address in an NDP link-layer
 *          address option from that option's length field and the given device
 *          type.
 *
 * @note    If an RFC exists that specifies how IPv6 operates over a link-layer,
 *          this function usually implements the section "Unicast Address
 *          Mapping".
 *
 * @see [RFC 4861, section 4.6.1](https://tools.ietf.org/html/rfc4861#section-4.6.1)
 *
 * @pre `netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR`
 *
 * @attention   When `NDEBUG` is not defined, the node fails with an assertion
 *              instead of returning `-ENOTSUP`
 *
 * @param[in] netif The network interface @p opt was received on within an NDP
 *                  message.
 * @param[in] opt   An NDP source/target link-layer address option.
 *
 * @return  Length of the link-layer address in @p opt on success
 * @return  `-ENOTSUP`, when implementation does not know how to derive the
 *          length of the link-layer address from @p opt's length field based
 *          on gnrc_netif_t::device_type of @p netif.
 * @return  `-EINVAL` if `opt->len` was an invalid value for the given
 *          gnrc_netif_t::device_type of @p netif.
 */
static inline int gnrc_netif_ndp_addr_len_from_l2ao(gnrc_netif_t *netif,
                                                    const ndp_opt_t *opt)
{
    assert(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR);
    return l2util_ndp_addr_len_from_l2ao(netif->device_type, opt);
}

/**
 * @brief   Converts an IPv6 multicast address to a multicast address
 *          of the respective link layer.
 *
 * @pre There is enough allocated space in @p l2_group for an address for a
 *      device of type @p dev_type (e.g. 6 bytes for an ethernet address).
 *
 * @param[in] dev_type      The network interface @p l2_addr should be generated
 *                          for.
 * @param[in] ipv6_group    An IPv6 multicast address.
 * @param[out] l2_group     A link layer multicast address
 *
 * @return  Length of @p l2_group in bytes
 * @return  `-ENOTSUP` if link layer does not support multicast.
 */
static inline int gnrc_netif_ipv6_group_to_l2_group(gnrc_netif_t *netif,
                                                    const ipv6_addr_t *ipv6_group,
                                                    uint8_t *l2_group)
{
    return l2util_ipv6_group_to_l2_group(netif->device_type, ipv6_group,
                                         l2_group);
}

/**
 * @brief   Configures a prefix on a network interface.
 *
 *          If the interface is a 6LoWPAN interface, this will also
 *          take care of setting up a compression context.
 *
 * @param[in] netif     Network interface the prefix should be added to
 * @param[in] pfx       Prefix to configure
 * @param[in] pfx_len   Length of @p pfx in bits
 * @param[in] valid     Valid lifetime of the prefix in seconds
 * @param[in] pref      Preferred lifetime of the prefix in seconds
 *
 * @return  >= 0, on success
 * @return  -ENOMEM, when no space for new addresses (or its solicited nodes
 *          multicast address) is left on the interface
 */
int gnrc_netif_ipv6_add_prefix(gnrc_netif_t *netif,
                               const ipv6_addr_t *pfx, uint8_t pfx_len,
                               uint32_t valid, uint32_t pref);
#else   /* IS_USED(MODULE_GNRC_NETIF_IPV6) || defined(DOXYGEN) */
#define gnrc_netif_ipv6_init_mtu(netif)                             (void)netif
#define gnrc_netif_ipv6_iid_from_addr(netif, addr, addr_len, iid)   (-ENOTSUP)
#define gnrc_netif_ipv6_iid_to_addr(netif, iid, addr)               (-ENOTSUP)
#define gnrc_netif_ndp_addr_len_from_l2ao(netif, opt)               (-ENOTSUP)
#define gnrc_netif_ipv6_get_iid(netif, iid)                         (-ENOTSUP)
#define gnrc_netif_ipv6_group_to_l2_group(netif, ipv6_group, l2_group)  (-ENOTSUP)
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) || defined(DOXYGEN) */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_INTERNAL_H */
/** @} */
