/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_netif IPv6 network interfaces
 * @ingroup     net_gnrc_ipv6
 * @brief       IPv6 specific information on @ref net_gnrc_netif.
 * @{
 *
 * @file
 * @brief       Definitions for IPv6 specific information of network interfaces.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef GNRC_IPV6_NETIF_H_
#define GNRC_IPV6_NETIF_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "kernel_macros.h"
#include "kernel_types.h"
#include "mutex.h"
#include "net/ipv6.h"
#include "net/ipv6/addr.h"
#include "vtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def GNRC_IPV6_NETIF_ADDR_NUMOF
 *
 * @brief   Number of IPv6 addresses per interface.
 */
#ifndef GNRC_IPV6_NETIF_ADDR_NUMOF
#ifdef MODULE_GNRC_IPV6_ROUTER
#define GNRC_IPV6_NETIF_ADDR_NUMOF  (7) /* router needs all-routers multicast address */
#else
#define GNRC_IPV6_NETIF_ADDR_NUMOF  (6)
#endif
#endif

/**
 * @brief   Default MTU
 *
 * An interface will choose this MTU if the link-layer's maximum packet size
 * (see @ref NETOPT_MAX_PACKET_SIZE) is lesser than the @ref IPV6_MIN_MTU or if it just not
 * provide it. For RFC-compatible communication it must be at least @ref IPV6_MIN_MTU.
 *
 * @note    If the scenario the node is used in allows for it and the packet size is predictable,
 *          a user might choose to set @ref GNRC_IPV6_NETIF_DEFAULT_MTU to a lesser value than
 *          @ref IPV6_MIN_MTU to optimize for code size (e.g. because it is then possible to omit
 *          @ref net_gnrc_sixlowpan_frag) and memory usage (e.g. because @ref GNRC_PKTBUF_SIZE
 *          can be much smaller).
 */
#ifndef GNRC_IPV6_NETIF_DEFAULT_MTU
#define GNRC_IPV6_NETIF_DEFAULT_MTU             (IPV6_MIN_MTU)
#endif

/**
 * @brief   Default hop limit
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-6.3.2">
 *          RFC 4861, section 6.3.2
 *      </a>
 * @see <a href="http://www.iana.org/assignments/ip-parameters/ip-parameters.xhtml#ip-parameters-2">
 *          IANA, IP TIME TO LIVE PARAMETER
 *      </a>
 */
#define GNRC_IPV6_NETIF_DEFAULT_HL              (64)

/**
 * @name    Default values for router configuration
 * @{
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-6.2.1">
 *          RFC 4861, section 6.2.1
 *      </a>
 */
/**
 * @brief   Maximum time in seconds between sending unsolicited multicast router advertisements.
 */
#define GNRC_IPV6_NETIF_DEFAULT_MAX_ADV_INT     (600U)

/**
 * @brief   Minimum time in seconds between sending unsolicited multicast router advertisements.
 */
#define GNRC_IPV6_NETIF_DEFAULT_MIN_ADV_INT     (200U)

/**
 * @brief   The router lifetime to propagate in router advertisements.
 */
#define GNRC_IPV6_NETIF_DEFAULT_ROUTER_LTIME    (1800U)
/** @} */

/**
 * @{
 * @name Flags for a registered IPv6 address.
 * @brief   Needed primarily to identify addresses as either anycast or unicast.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4291#section-2.6">
 *          RFC 4291, section 2.6
 *      </a>
 */
#define GNRC_IPV6_NETIF_ADDR_FLAGS_UNICAST      (0x00)  /**< unicast address */
#define GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST  (0x01)  /**< non-unicast address */

/**
 * @brief   A prefix information option that propagates the prefix of this
 *          address should set the autonomous flag.
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-6.2.1">
 *              RFC 4861, section 6.2.1
 *          </a>
 */
#define GNRC_IPV6_NETIF_ADDR_FLAGS_NDP_AUTO     (0x40)

/**
 * @brief   A prefix information option that propagates the prefix of this
 *          address should set the on-link flag.
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-6.2.1">
 *              RFC 4861, section 6.2.1
 *          </a>
 */
#define GNRC_IPV6_NETIF_ADDR_FLAGS_NDP_ON_LINK  (0x80)
/**
 * @}
 */

/**
 * @{
 * @name Flags for the interfaces
 */
/**
 * @brief   Interface is 6LoWPAN interface.
 */
#define GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN         (0x0001)

/**
 * @brief   Flag to indicate that routing is enabled on the interface.
 */
#define GNRC_IPV6_NETIF_FLAGS_ROUTER            (0x0002)

/**
 * @brief   Flag to indicate that the interface sends periodic router
 *          advertisements and in response to router solicitations.
 */
#define GNRC_IPV6_NETIF_FLAGS_RTR_ADV           (0x0004)

/**
 * @brief   Flag to indicate that gnrc_ipv6_netif_t::mtu shall be propagated
 *          with the MTU options in router advertisements.
 */
#define GNRC_IPV6_NETIF_FLAGS_ADV_MTU           (0x0008)

/**
 * @brief   Flag to indicate that gnrc_ipv6_netif_t::cur_hl shall be propagated
 *          in router advertisements.
 */
#define GNRC_IPV6_NETIF_FLAGS_ADV_CUR_HL        (0x0010)

/**
 * @brief   Flag to indicate that gnrc_ipv6_netif_t::reach_time shall be propagated
 *          in router advertisements.
 */
#define GNRC_IPV6_NETIF_FLAGS_ADV_REACH_TIME    (0x0020)

/**
 * @brief   Flag to indicate that ng_ipv6_netif_t::retrans_timer shall be propagated
 *          in router advertisements.
 */
#define GNRC_IPV6_NETIF_FLAGS_ADV_RETRANS_TIMER (0x0040)

/**
 * @brief   Flag to indicate if the interface is operating over a wired link
 */
#define GNRC_IPV6_NETIF_FLAGS_IS_WIRED          (0x2000)

/**
 * @brief   Flag to indicate that the interface has other address
 *          configuration.
 */
#define GNRC_IPV6_NETIF_FLAGS_OTHER_CONF        (0x4000)

/**
 * @brief   Flag to indicate that the interface has managed address
 *          configuration (e.g. via DHCPv6).
 */
#define GNRC_IPV6_NETIF_FLAGS_MANAGED           (0x8000)
/**
 * @}
 */

/**
 * @brief   Type to represent an IPv6 address registered to an interface.
 */
typedef struct {
    ipv6_addr_t addr;       /**< The address data */
    uint8_t flags;          /**< flags */
    uint8_t prefix_len;     /**< length of the prefix of the address */
    /**
     * @{
     * @name    Neigbour discovery variables for prefixes
     * @see     <a href="https://tools.ietf.org/html/rfc4861#section-6.2.1">
     *              RFC 4861, section 6.2.1
     *          </a>
     */
    /**
     * @brief   The time in seconds this address is valid. If it is UINT32_MAX
     *          the lifetime is infinite.
     */
    uint32_t valid;

    /**
     * @brief   The length of time that this address remains preferred.
     *          If it is UINT32_MAX the lifetime is infinite.
     *          It **must** be < gnrc_ipv6_netif_addr_t::valid.
     */
    uint32_t preferred;
    /**
     * @brief   Validity timeout timer.
     */
    vtimer_t valid_timeout;
    /**
     * @}
     */
} gnrc_ipv6_netif_addr_t;

/**
 * @brief   Definition of IPv6 interface type.
 */
typedef struct {
    /**
     * @brief addresses registered to the interface
     */
    gnrc_ipv6_netif_addr_t addrs[GNRC_IPV6_NETIF_ADDR_NUMOF];
    mutex_t mutex;          /**< mutex for the interface */
    kernel_pid_t pid;       /**< PID of the interface */
    uint16_t flags;         /**< flags for 6LoWPAN and Neighbor Discovery */
    uint16_t mtu;           /**< Maximum Transmission Unit (MTU) of the interface */
    uint8_t cur_hl;         /**< current hop limit for the interface */
#ifdef MODULE_GNRC_NDP_HOST
    /**
     * @brief   Counter for send router solicitations.
     */
    uint8_t rtr_sol_count;
#endif
#ifdef MODULE_GNRC_NDP_ROUTER
    /**
     * @brief   Counter for initial router advertisements.
     */
    uint8_t rtr_adv_count;

    /**
     * @brief   Maximum time in seconds between sending unsolicited multicast
     *          router advertisements. Must be between 4 and 1800 seconds.
     *          The default value is @ref GNRC_IPV6_NETIF_DEFAULT_MAX_ADV_INT.
     */
    uint16_t max_adv_int;

    /**
     * @brief   Minimum time in seconds between sending unsolicited multicast
     *          router advertisements. Must be between 3 and
     *          3/4 * ng_ipv6_netif_t::max_adv_int seconds.
     *          The default value is @ref GNRC_IPV6_NETIF_DEFAULT_MIN_ADV_INT.
     */
    uint16_t min_adv_int;

    /**
     * @brief   The router lifetime to propagate in router advertisements.
     *          Must be either 0 or between ng_ipv6_netif_t::max_adv_int and
     *          9000 seconds. 0 means this router is not to be used as a default
     *          router. The default value is @ref GNRC_IPV6_NETIF_DEFAULT_ROUTER_LTIME.
     */
    uint16_t adv_ltime;
#endif
    /**
     * @brief   Base value in microseconds for computing random
     *          gnrc_ipv6_netif_t::reach_time.
     *          The default value is @ref GNRC_NDP_REACH_TIME.
     */
    uint32_t reach_time_base;

    /**
     * @brief   The time a neighbor is considered reachable after receiving
     *          a reachability confirmation.
     *          Should be uniformly distributed between @ref GNRC_NDP_MIN_RAND
     *          and GNRC_NDP_MAX_RAND multiplied with
     *          gnrc_ipv6_netif_t::reach_time_base microseconds devided by 10.
     *          Can't be greater than 1 hour.
     */
    timex_t reach_time;

    /**
     * @brief   Time between retransmissions of neighbor solicitations to a
     *          neighbor.
     *          The default value is @ref GNRC_NDP_RETRANS_TIMER.
     */
    timex_t retrans_timer;
    vtimer_t rtr_sol_timer; /**< Timer for periodic router solicitations */
#ifdef MODULE_GNRC_NDP_ROUTER
    vtimer_t rtr_adv_timer; /**< Timer for periodic router advertisements */
#endif
} gnrc_ipv6_netif_t;

/**
 * @brief Initializes the module.
 */
void gnrc_ipv6_netif_init(void);

/**
 * @brief   Add interface to IPv6.
 *
 * @details This function will be called by @ref gnrc_netif_add().
 *
 * @param[in] pid   The PID to the interface.
 */
void gnrc_ipv6_netif_add(kernel_pid_t pid);

/**
 * @brief   Remove interface from IPv6.
 *
 * @details This function will be called by @ref gnrc_netif_remove().
 *
 * @param[in] pid   The PID to the interface.
 */
void gnrc_ipv6_netif_remove(kernel_pid_t pid);

/**
 * @brief   Get interface.
 *
 * @param[in] pid   The PID to the interface.
 *
 * @return  The interface describing structure, on success.
 * @return  NULL, if there is no interface with PID @p pid.
 */
gnrc_ipv6_netif_t *gnrc_ipv6_netif_get(kernel_pid_t pid);

#if (defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_ROUTER))
/**
 * @brief   Set interface to router mode.
 *
 * @details This sets/unsets the GNRC_IPV6_NETIF_FLAGS_ROUTER and initializes
 *          or ceases router behavior for neighbor discovery.
 *
 * @param[in] netif     The interface.
 * @param[in] enable    Status for the GNRC_IPV6_NETIF_FLAGS_ROUTER flag.
 */
void gnrc_ipv6_netif_set_router(gnrc_ipv6_netif_t *netif, bool enable);

/**
 * @brief   Set interface to router advertisement mode.
 *
 * @details If GNRC_IPV6_NETIF_FLAGS_ROUTER is set this sets/unsets the
 *          GNRC_IPV6_NETIF_FLAGS_RTR_ADV and initializes or ceases router
 *          advertising behavior for neighbor discovery.
 *
 * @param[in] netif     The interface.
 * @param[in] enable    Status for the GNRC_IPV6_NETIF_FLAGS_RTR flag.
 */
void gnrc_ipv6_netif_set_rtr_adv(gnrc_ipv6_netif_t *netif, bool enable);
#else
/* dummy macros to be able to "call" these functions when none of the relevant modules
 * is implemented */
#define gnrc_ipv6_netif_set_router(netif, enable)
#define gnrc_ipv6_netif_set_rtr_adv(netif, enable)
#endif

/**
 * @brief   Adds an address to an interface.
 *
 * @param[in] pid           The PID to the interface.
 * @param[in] addr          An address you want to add to the interface.
 * @param[in] prefix_len    Length of the prefix of the address.
 *                          Must be between 1 and 128.
 * @param[in] flags         Flags for the address entry
 *                          If @p addr should be an anycast address, @p flags
 *                          must have @ref GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST
 *                          set. Otherwise leave it unset.
 *                          If @p addr is a multicast address, the status of
 *                          @ref GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST will be
 *                          ignored and set in either case.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4291#section-2.6">
 *          RFC 4291, section 2.6
 *      </a>
 *
 * @return  The address on the interface, on success.
 * @return  NULL, on failure
 */
ipv6_addr_t *gnrc_ipv6_netif_add_addr(kernel_pid_t pid, const ipv6_addr_t *addr, uint8_t prefix_len,
                                      uint8_t flags);

/**
 * @brief   Remove an address from the interface.
 *
 * @param[in] pid       The PID to the interface. If @p pid is KERNEL_PID_UNDEF
 *                      it will be removed from all interfaces.
 * @param[in] addr      An address you want to remove from interface.
 */
void gnrc_ipv6_netif_remove_addr(kernel_pid_t pid, ipv6_addr_t *addr);

/**
 * @brief   Removes all addresses from the interface.
 *
 * @param[in] pid       The PID to the interface.
 */
void gnrc_ipv6_netif_reset_addr(kernel_pid_t pid);

/**
 * @brief   Searches for an address on all interfaces.
 *
 * @param[out] out  The reference to the address on the interface.
 * @param[in] addr  The address you want to search for.
 *
 * @return  The PID to the interface the address is registered to.
 * @return  KERNEL_PID_UNDEF, if the address can not be found on any interface.
 */
kernel_pid_t gnrc_ipv6_netif_find_by_addr(ipv6_addr_t **out, const ipv6_addr_t *addr);

/**
 * @brief   Searches for an address on an interface.
 *
 * @param[in] pid   The PID to the interface.
 * @param[in] addr  The address you want to search for.
 *
 * @return  The reference to the address on the interface.
 * @return  NULL, if the address can not be found on the interface.
 * @return  NULL, if @p pid is no interface.
 */
ipv6_addr_t *gnrc_ipv6_netif_find_addr(kernel_pid_t pid, const ipv6_addr_t *addr);

/**
 * @brief   Searches for the first address matching a prefix best on all
 *          interfaces.
 *
 * @param[out] out      The reference to the found address on the interface.
 * @param[in] prefix    The prefix you want to search for.
 *
 * @return  The PID to the interface the address is registered to.
 * @return  KERNEL_PID_UNDEF, if no matching address can not be found on any
 *          interface.
 */
kernel_pid_t gnrc_ipv6_netif_find_by_prefix(ipv6_addr_t **out, const ipv6_addr_t *prefix);

/**
 * @brief   Searches for the first address matching a prefix best on an
 *          interface.
 *
 * @param[in] pid       The PID to the interface.
 * @param[in] prefix    The prefix you want to search for.
 *
 * @return  The reference to the found address on the interface.
 * @return  NULL, if no matching address can be found on the interface.
 * @return  NULL, if @p pid is no interface.
 */
ipv6_addr_t *gnrc_ipv6_netif_match_prefix(kernel_pid_t pid, const ipv6_addr_t *prefix);

/**
 * @brief   Searches for the best address on an interface usable as a
 *          source address for a given destination address.
 *
 * @param[in] pid   The PID to the interface.
 * @param[in] dest  The destination address you want to find a destination
 *                  address for.
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
 * @return  The reference to the found address on the interface.
 * @return  NULL, if no matching address can be found on the interface.
 * @return  NULL, if @p pid is no interface.
 */
ipv6_addr_t *gnrc_ipv6_netif_find_best_src_addr(kernel_pid_t pid, const ipv6_addr_t *dest);

/**
 * @brief   Get interface specific meta-information on an address
 *
 * @details This only works with addresses you retrieved via the following
 *          functions:
 *
 * * gnrc_ipv6_netif_add_addr()
 * * gnrc_ipv6_find_addr()
 * * gnrc_ipv6_find_addr_local()
 * * gnrc_ipv6_find_prefix_match()
 * * gnrc_ipv6_find_prefix_match_local()
 * * gnrc_ipv6_find_best_src_address()
 *
 * The behaviour for other addresses is undefined.
 *
 * @param[in] addr  The address you want to get the meta-information for.
 *
 * @return  Interface specific meta-information on @p addr
 */
static inline gnrc_ipv6_netif_addr_t *gnrc_ipv6_netif_addr_get(const ipv6_addr_t *addr)
{
    return container_of(addr, gnrc_ipv6_netif_addr_t, addr);
}

/**
 * @brief   Checks if an address is non-unicast.
 *
 * @details This only works with addresses you retrieved via the following
 *          functions:
 *
 * * gnrc_ipv6_netif_add_addr()
 * * gnrc_ipv6_find_addr()
 * * gnrc_ipv6_find_addr_local()
 * * gnrc_ipv6_find_prefix_match()
 * * gnrc_ipv6_find_prefix_match_local()
 * * gnrc_ipv6_find_best_src_address()
 *
 * The behaviour for other addresses is undefined.
 *
 * @param[in] addr  The address you want to check.
 *
 * @return true, if address is anycast or multicast.
 * @return false, if address is unicast.
 */
static inline bool gnrc_ipv6_netif_addr_is_non_unicast(const ipv6_addr_t *addr)
{
    return (bool)(container_of(addr, gnrc_ipv6_netif_addr_t, addr)->flags &
                  GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST);

}

/**
 * @brief   Initializes an interface with device-dependent values.
 *
 * @note    Must be called after all interfaces were initialized and must not
 *          be called in an interface's thread (will otherwise hang up).
 */
void gnrc_ipv6_netif_init_by_dev(void);

#ifdef __cplusplus
}
#endif

#endif /* NETIF_H_ */
/**
 * @}
 */
