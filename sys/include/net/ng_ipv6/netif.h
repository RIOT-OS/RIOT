/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ng_ipv6_netif IPv6 network interfaces
 * @ingroup     net_ng_ipv6
 * @brief       IPv6 specific information on @ref net_ng_netif.
 * @{
 *
 * @file
 * @brief       Definitions for IPv6 specific information of network interfaces.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef NG_IPV6_NETIF_H_
#define NG_IPV6_NETIF_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "kernel_macros.h"
#include "kernel_types.h"
#include "mutex.h"
#include "net/ng_ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def NG_IPV6_NETIF_ADDR_NUMOF
 *
 * @brief   Number of IPv6 addresses per interface.
 */
#ifndef NG_IPV6_NETIF_ADDR_NUMOF
#ifdef MODULE_NG_IPV6_ROUTER
#define NG_IPV6_NETIF_ADDR_NUMOF    (5) /* router needs all-routers multicast address */
#else
#define NG_IPV6_NETIF_ADDR_NUMOF    (4)
#endif
#endif

/**
 * @brief   Default MTU
 *
 * @see <a href="https://tools.ietf.org/html/rfc2460#section-5">
 *          RFC 2460, section 5
 *      </a>
 */
#define NG_IPV6_NETIF_DEFAULT_MTU   (1280)

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
#define NG_IPV6_NETIF_DEFAULT_HL    (64)

/**
 * @{
 * @name Flags for a registered IPv6 address.
 * @brief   Needed primarily to identify addresses as either anycast or unicast.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4291#section-2.6">
 *          RFC 4291, section 2.6
 *      </a>
 */
#define NG_IPV6_NETIF_FLAGS_UNICAST     (0x00)  /**< unicast address */
#define NG_IPV6_NETIF_FLAGS_NON_UNICAST (0x01)  /**< non-unicast address */
/**
 * @}
 */

/**
 * @brief   Type to represent an IPv6 address registered to an interface.
 */
typedef struct {
    ng_ipv6_addr_t addr;    /**< The address data */
    uint8_t flags;          /**< flags */
    uint8_t prefix_len;     /**< length of the prefix of the address */
} ng_ipv6_netif_addr_t;

/**
 * @brief   Definition of IPv6 interface type.
 */
typedef struct {
    ng_ipv6_netif_addr_t addrs[NG_IPV6_NETIF_ADDR_NUMOF];   /**< addresses registered
                                                             *   to the interface */
    mutex_t mutex;          /**< mutex for the interface */
    kernel_pid_t pid;       /**< PID of the interface */
    uint16_t mtu;           /**< Maximum Transmission Unit (MTU) of the interface */
    uint8_t cur_hl;         /**< current hop limit for the interface */
} ng_ipv6_netif_t;

/**
 * @brief Initializes the module.
 */
void ng_ipv6_netif_init(void);

/**
 * @brief   Add interface to IPv6.
 *
 * @details This function will be called by @ref ng_netif_add().
 *
 * @param[in] pid   The PID to the interface.
 */
void ng_ipv6_netif_add(kernel_pid_t pid);

/**
 * @brief   Remove interface from IPv6.
 *
 * @details This function will be called by @ref ng_netif_remove().
 *
 * @param[in] pid   The PID to the interface.
 */
void ng_ipv6_netif_remove(kernel_pid_t pid);

/**
 * @brief   Get interface.
 *
 * @param[in] pid   The PID to the interface.
 *
 * @return  The interface describing structure, on success.
 * @return  NULL, if there is no interface with PID @p pid.
 */
ng_ipv6_netif_t *ng_ipv6_netif_get(kernel_pid_t pid);

/**
 * @brief   Adds an address to an interface.
 *
 * @param[in] pid           The PID to the interface.
 * @param[in] addr          An address you want to add to the interface.
 * @param[in] prefix_len    Length of the prefix of the address.
 *                          Must be between 1 and 128.
 * @param[in] anycast       If @p addr should be an anycast address, @p anycast
 *                          must be true. Otherwise set it false.
 *                          If @p addr is a multicast address, @p anycast will be
 *                          ignored.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4291#section-2.6">
 *          RFC 4291, section 2.6
 *      </a>
 *
 * @return  0, on success.
 * @return  -EINVAL, if @p addr is NULL or unspecified address or if
 *          @p prefix length was < 1 or > 128.
 * @return  -ENOENT, if @p pid is no interface.
 * @return  -ENOMEM, if there is no space left to store @p addr.
 */
int ng_ipv6_netif_add_addr(kernel_pid_t pid, const ng_ipv6_addr_t *addr,
                           uint8_t prefix_len, bool anycast);

/**
 * @brief   Remove an address from the interface.
 *
 * @param[in] pid       The PID to the interface.
 * @param[in] addr      An address you want to remove from interface.
 */
void ng_ipv6_netif_remove_addr(kernel_pid_t pid, ng_ipv6_addr_t *addr);

/**
 * @brief   Removes all addresses from the interface.
 *
 * @param[in] pid       The PID to the interface.
 */
void ng_ipv6_netif_reset_addr(kernel_pid_t pid);

/**
 * @brief   Searches for an address on all interfaces.
 *
 * @param[out] out  The reference to the address on the interface.
 * @param[in] addr  The address you want to search for.
 *
 * @return  The PID to the interface the address is registered to.
 * @return  KERNEL_PID_UNDEF, if the address can not be found on any interface.
 */
kernel_pid_t ng_ipv6_netif_find_by_addr(ng_ipv6_addr_t **out,
                                        const ng_ipv6_addr_t *addr);

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
ng_ipv6_addr_t *ng_ipv6_netif_find_addr(kernel_pid_t pid,
                                        const ng_ipv6_addr_t *addr);

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
kernel_pid_t ng_ipv6_netif_find_by_prefix(ng_ipv6_addr_t **out,
        const ng_ipv6_addr_t *prefix);

/**
 * @brief   Searches for the first address matching a prefix best on an
 *          interfaces.
 *
 * @param[in] pid       The PID to the interface.
 * @param[in] prefix    The prefix you want to search for.
 *
 * @return  The reference to the found address on the interface.
 * @return  NULL, if no matching address can be found on the interface.
 * @return  NULL, if @p pid is no interface.
 */
ng_ipv6_addr_t *ng_ipv6_netif_match_prefix(kernel_pid_t pid,
        const ng_ipv6_addr_t *prefix);

/**
 * @brief   Searches for the best address on an interface usable as a
 *          source address for a given destination address.
 *
 * @param[in] pid   The PID to the interface.
 * @param[in] dest  The destination address you want to find a destination
 *                  address for.
 *
 * @return  The reference to the found address on the interface.
 * @return  NULL, if no matching address can be found on the interface.
 * @return  NULL, if @p pid is no interface.
 */
ng_ipv6_addr_t *ng_ipv6_netif_find_best_src_addr(kernel_pid_t pid, const ng_ipv6_addr_t *dest);

/**
 * @brief   Checks if an address is non-unicast.
 *
 * @details This only works with addresses you retrieved via the following
 *          functions:
 *
 * * ng_ipv6_find_addr()
 * * ng_ipv6_find_addr_local()
 * * ng_ipv6_find_prefix_match()
 * * ng_ipv6_find_prefix_match_local()
 * * ng_ipv6_find_best_src_address
 *
 * The behaviour for other addresses is undefined.
 *
 * @param[in] addr  The address you want to check.
 *
 * @return true, if address is anycast or multicast.
 * @return false, if address is unicast.
 */
static inline bool ng_ipv6_netif_addr_is_non_unicast(const ng_ipv6_addr_t *addr)
{
    return (bool)(container_of(addr, ng_ipv6_netif_addr_t, addr)->flags &
                  NG_IPV6_NETIF_FLAGS_NON_UNICAST);

}

#ifdef __cplusplus
}
#endif

#endif /* NETIF_H_ */
/**
 * @}
 */
