/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ipv6_if
 * @ingroup     net_ipv6
 * @{
 *
 * @file        ipv6/if.h
 * @brief       IP interfaces
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef __IPV6_IF_H_
#define __IPV6_IF_H_

#include <stdbool.h>

#include "ipv6.h"
#include "kernel_macros.h"
#include "kernel_types.h"
#include "mutex.h"
#include "netdev/base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def IPV6_IF_NUM
 *
 * @brief   Number of IP interfaces on active. Default value is 1.
 */
#ifndef IPV6_IF_NUM
#define IPV6_IF_NUM (1)
#endif  /* MODULE_IPV6_ROUTER */

/**
 * @def IPV6_IF_ADDR_NUM
 *
 * @brief   Number of addresses per interface.
 */
#ifdef MODULE_IPV6_ROUTER
#define IPV6_IF_ADDR_NUM    (5)
#else   /* MODULE_IPV6_ROUTER */
#define IPV6_IF_ADDR_NUM    (4)
#endif  /* MODULE_IPV6_ROUTER */

/**
 * @brief   ID of the loopback interface.
 */
#define IPV6_IF_LOOPBACK_ID (IPV6_IF_NUM)

/**
 * @brief Flags and scope for an address registered to and interface
 *
 * @see <a href="https://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 */
typedef enum __attribute__((packed)) {
    /**
     * @brief   Identifies address as a unicast address.
     *
     * @details Every address, that has not IPV6_IF_ADDR_FLAGS_ANYCAST
     *          or IPV6_IF_ADDR_FLAGS_MULTICAST set can be assumed to be a
     *          unicast address.
     */
    IPV6_IF_ADDR_FLAGS_UNICAST                = 0x00,

    /**
     * @brief   Identifies address as an anycast address.
     *
     * @details IPV6_IF_ADDR_FLAGS_MULTICAST and IPV6_IF_ADDR_FLAGS_ANYCAST
     *          are mutually exclusive.
     */
    IPV6_IF_ADDR_FLAGS_ANYCAST                = 0x01,

    /**
     * @brief   Identifies address as a multicast address.
     *
     * @details IPV6_IF_ADDR_FLAGS_MULTICAST and IPV6_IF_ADDR_FLAGS_ANYCAST
     *          are mutually exclusive.
     */
    IPV6_IF_ADDR_FLAGS_MULTICAST              = 0x02,

    /**
     * @brief   Identifies address as a link-local address.
     */
    IPV6_IF_ADDR_FLAGS_LINK_LOCAL             = 0x04
} ipv6_if_addr_flags_t;

/**
 * @brief   Type to represent an address registered to an interface
 */
typedef struct {
    ipv6_addr_t addr;           /**< The actual address data */

    /**
     * @brief   Flags for the address
     *
     * @details The flags are included since an interface has to keep the
     *          information if an address is an anycast address
     *          (<a href="https://tools.ietf.org/html/rfc4291#section-2.6">RFC
     *          2491, section 2.6</a>). Since this infomation would cost at
     *          least 1 byte anyways this byte could be used for further
     *          information, hence these flags.
     */
    ipv6_if_addr_flags_t flags;
} ipv6_if_addr_t;

/**
 * @brief   Definiton of IP interface type.
 */
typedef struct {
    ipv6_if_addr_t addrs[IPV6_IF_ADDR_NUM]; /**< addresses registered to the interface */
    mutex_t mutex;                          /**< mutex for the interface */
    kernel_pid_t mac_pid;                   /**< MAC layer registered to this interface */
} ipv6_if_t;

/**
 * @brief   Type to represent found addresses searched on all interfaces.
 */
typedef struct {
    int if_id;          /**< The ID of the interface where the address was found */
    ipv6_addr_t *addr;  /**< Pointer to the address on the interface identified by
                         *   *if_id* */
} ipv6_if_addr_pair_t;

extern ipv6_if_t ipv6_ifs[IPV6_IF_NUM];

extern const ipv6_if_t ipv6_if_loopback;

/**
 * @brief   Initializes the module.
 */
void ipv6_if_init(void);

/**
 * @brief   Initializes an interface for a MAC layer identified by *mac_pid*
 *
 * @param[in] mac_pid   A MAC layer's PID
 *
 * @return The new interface's ID on success.
 * @return -EINVAL if *mac_pid* is KERNEL_PID_UNDEF
 * @return -EISCONN if *mac_pid* is already connected to the interface
 * @return -ENOBUFS if no space for a new interface is left.
 */
int ipv6_if_init_if(kernel_pid_t mac_pid);

/**
 * @brief   Resets an interface to its initial state
 *
 * @param[in] if_id An interface's ID
 */
void ipv6_if_reset_if(int if_id);

/**
 * @brief   Checks if an interface was intitialized
 *
 * @param[in] if_id An interface's ID
 *
 * @return  true, if interface is initialized
 * @return  false, if interface is not initialized
 */
static inline bool ipv6_if_initialized(int if_id)
{
    return (if_id == IPV6_IF_LOOPBACK_ID) ||
           ((if_id >= 0) && (if_id < IPV6_IF_NUM) && (ipv6_ifs[if_id].mac_pid != KERNEL_PID_UNDEF));
}

/**
 * @brief   Gets an interface by its ID.
 *
 * @param[in] if_id An interface's ID
 *
 * @return The interface identified by *if_id* on success.
 * @return NULL, if there is no interface identified by *if_id*.
 */
static inline ipv6_if_t *ipv6_if_get_by_id(int if_id)
{
    if (if_id == IPV6_IF_LOOPBACK_ID) {
        return (ipv6_if_t *)(&ipv6_if_loopback);
    }
    else if (ipv6_if_initialized(if_id)) {
        return &(ipv6_ifs[if_id]);
    }
    else {
        return NULL;
    }
}

/**
 * @brief   Gets the ID of the interface that has *mac_pid* registered as its
 *          MAC layer.
 *
 * @param[in] mac_pid   A MAC layer's PID
 *
 * @return The interface's ID that has *mac_pid* registered as its MAC layer on success.
 * @return -ENOENT, if there is no such interface.
 */
int ipv6_if_get_id_by_mac(kernel_pid_t mac_pid);

/**
 * @brief   Gets an interface by the MAC layer it is registered to.
 *
 * @param[in] mac_pid   A MAC layer's PID
 *
 * @return The interface that has *mac_pid* registered as its MAC on success.
 * @return NULL, if there is no such interface.
 */
static inline ipv6_if_t *ipv6_if_get_by_mac(kernel_pid_t mac_pid)
{
    return ipv6_if_get_by_id(ipv6_if_get_id_by_mac(mac_pid));
}

/**
 * @brief   Adds an address to an interface
 *
 * @details Duplicate addresses are ignored
 *
 * @see <a href="https://tools.ietf.org/html/rfc4291#section-2.6">
 *          RFC 4291, section 2.6
 *      </a> on anycast semantics.
 * @see <a href="https://tools.ietf.org/html/rfc4291#section-2.6">
 *          RFC 4291, section 2.5
 *      </a> on address syntax.
 *
 * @param[in] if_id     An interface's ID
 * @param[in] addr      An IPv6 address
 * @param[in] anycast   Identifies *addr* as anycast address if true.
 *
 * @return  0, on success
 * @return  -EINVAL, if *addr* is syntactically no unicast address, but *anycast*
 *          is set or if *addr* is the unspecified or loopback address.
 * @return  -ENOBUFS, if there is no space for *addr* left on the interface
 *          identified by *if_id*.
 * @return  -ENOENT, if there is no interface identified by *if_id*.
 */
int ipv6_if_add_addr(int if_id, const ipv6_addr_t *addr, bool anycast);

/**
 * @brief   Removes an address from an interface
 *
 * @param[in] if_id An interface's ID
 * @param[in] addr  An IPv6 address
 *
 * @return  0, on success
 * @return  -ENOENT, if there is no interface identified by *if_id*.
 */
int ipv6_if_rem_addr(int if_id, const ipv6_addr_t *addr);

/**
 * @brief   Iterates over the addresses registered to an interface
 *
 * @note    The interface's mutex will **not** lock by calling this function.
 *          If you want to use it thread-safe, you **have** to lock it outside the
 *          iteration yourself.
 *
 * @param[in] if_id An interface's ID
 * @param[in] prev  Previous address returned by this function. To initialize
 *                  iteration set this value to NULL
 *
 * @return  Next address registered on this interface.
 * @return  NULL, if iteration has ended, if there is no interface identified
 *          by *if_id*, or *prev* was not registered to this interface.
 */
ipv6_addr_t *ipv6_if_iter_addr(int if_id, const ipv6_addr_t *prev);

/**
 * @brief   Checks if an address is registered to an interface
 *
 * @param[in] if_id An interface's ID
 * @param[in] addr  An IPv6 address
 *
 * @return  Pointer to the address on the interface, if it is is registered
 * @return  NULL, if the address is not registered to this interface or if
 *          there is no interface identified by *if_id*

 */
ipv6_addr_t *ipv6_if_find_addr_on_if(int if_id, const ipv6_addr_t *addr);

/**
 * @brief   Searches an for the first occurence (in order of interface IDs) of
 *          an address on all interface
 *
 * @param[out] res  The result of the search.
 * @param[in] addr  An IPv6 address
 *
 * @return  Copy of *res* on success
 * @return  NULL, if the address is not registered to any interface
 */
ipv6_if_addr_pair_t *ipv6_if_find_addr(ipv6_if_addr_pair_t *res,
                                       const ipv6_addr_t *addr);

/**
 * @brief   Searches for an address with the best prefix match to *addr* on an
 *          interface identified by *if_id*
 *
 * @param[in] if_id An interface's ID
 * @param[in] addr  An IPv6 address
 *
 * @return  Pointer to the found address on the interface, if it is is registered
 * @return  NULL, if there is no address registered to this interface or if
 *          there is no interface identified by *if_id*
 */
ipv6_addr_t *ipv6_if_find_prefix_match_on_if(int if_id, const ipv6_addr_t *addr);

/**
 * @brief   Searches for an address with the best prefix match to *addr* on all
 *          interfaces.
 *
 * @param[out] res  The result of the search.
 * @param[in] addr  An IPv6 address
 *
 * @return  Copy of *res* on success
 * @return  NULL, if there is no address registered to any interface
 */
ipv6_if_addr_pair_t *ipv6_if_find_prefix_match(ipv6_if_addr_pair_t *res,
        const ipv6_addr_t *addr);

/**
 * @brief   Tries to determine best suitable source address attached to
 *          an interface based on the given destination
 *          address.
 *
 * @details The use-case for this function is to find a
 *          suitable address for the source address field of an IPv6
 *          address upon sending.
 *          The major difference to ipv6_if_find_prefix_match_on_if() is that
 *          multicast addresses are left out.
 *
 * @param[in]   if_id   An interface's ID.
 * @param[in]   addr    A destination address for a packet we search the source
 *                      address for.
 *
 * @return  The best source address for the interface, on success
 * @return  NULL, if no suitable address could be found or if there is no
 *          interface identified by *if_id*.
 */
ipv6_addr_t *ipv6_if_get_best_src_addr_on_if(int if_id, const ipv6_addr_t *addr);

/**
 * @brief   Tries to determine best suitable source address attached to
 *          any interface based on the given destination
 *          address.
 *
 * @details The use-case for this function is to find a
 *          suitable address for the source address field of an IPv6
 *          address upon sending.
 *          The major difference to ipv6_if_find_prefix_match() is that
 *          multicast addresses are left out.
 *
 * @param[out]  res     The best source address for this node.
 * @param[in]   addr    A destination address for a packet we search the source
 *                      address for.
 *
 * @return  Copy of *res* on success
 * @return  NULL, if no suitable address could be found on any interface.
 */
ipv6_if_addr_pair_t *ipv6_if_get_best_src_addr(ipv6_if_addr_pair_t *res,
        const ipv6_addr_t *addr);

/**
 * @brief   Checks if an address is an anycast address
 *
 * @note    This function only works if the pointer value of *addr* is received
 *          from the interface via
 *
 *          * @ref ipv6_if_find_addr_on_if(),
 *          * @ref ipv6_if_find_addr(),
 *          * @ref ipv6_if_find_prefix_match_on_if(), or
 *          * @ref ipv6_if_find_prefix_match().
 *
 * @param[in] addr  The address to check
 *
 * @return  true, if address is an anycast address
 * @return  false, if address is not an anycast address
 */
static inline bool ipv6_if_addr_is_anycast(const ipv6_addr_t *addr)
{
    return (bool)(container_of(addr, ipv6_if_addr_t, addr)->flags &
                  IPV6_IF_ADDR_FLAGS_ANYCAST);
}

/**
 * @brief   Checks if an address is an multicast address
 *
 * @note    This function only works if the pointer value of *addr* is received
 *          from the interface via
 *
 *          * @ref ipv6_if_find_addr_on_if(),
 *          * @ref ipv6_if_find_addr(),
 *          * @ref ipv6_if_find_prefix_match_on_if(), or
 *          * @ref ipv6_if_find_prefix_match().
 *
 *          but is a little bit faster than @ref ipv6_addr_is_multicast
 *
 * @param[in] addr  The address to check
 *
 * @return  true, if address is an multicast address
 * @return  false, if address is not an multicast address
 */
static inline bool ipv6_if_addr_is_multicast(const ipv6_addr_t *addr)
{
    return (bool)(container_of(addr, ipv6_if_addr_t, addr)->flags &
                  IPV6_IF_ADDR_FLAGS_MULTICAST);
}

/**
 * @brief   Checks if an address is an unicast address
 *
 * @note    This function only works if the pointer value of *addr* is received
 *          from the interface via
 *
 *          * @ref ipv6_if_find_addr_on_if(),
 *          * @ref ipv6_if_find_addr(),
 *          * @ref ipv6_if_find_prefix_match_on_if(), or
 *          * @ref ipv6_if_find_prefix_match().
 *
 * @param[in] addr  The address to check
 *
 * @return  true, if address is an unicast address
 * @return  false, if address is not an unicast address
 */
static inline bool ipv6_if_addr_is_unicast(const ipv6_addr_t *addr)
{
    return !ipv6_if_addr_is_multicast(addr) && !ipv6_if_addr_is_anycast(addr);
}

/**
 * @brief   Checks if an address is an link-local address
 *
 * @note    This function only works if the pointer value of *addr* is received
 *          from the interface via
 *
 *          * @ref ipv6_if_find_addr_on_if(),
 *          * @ref ipv6_if_find_addr(),
 *          * @ref ipv6_if_find_prefix_match_on_if(), or
 *          * @ref ipv6_if_find_prefix_match().
 *
 *          but is a little bit faster than @ref ipv6_addr_is_link_local()
 *
 * @param[in] addr  The address to check
 *
 * @return  true, if address is an link-local address
 * @return  false, if address is not an link-local address
 */
static inline bool ipv6_if_addr_is_link_local(const ipv6_addr_t *addr)
{
    return (bool)(container_of(addr, ipv6_if_addr_t, addr)->flags &
                  IPV6_IF_ADDR_FLAGS_LINK_LOCAL);
}

#ifdef __cplusplus
}
#endif

#endif /* __IPV6_IF_H_ */
/**
 * @}
 */
