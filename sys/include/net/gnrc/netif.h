/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_netif Network interface API
 * @ingroup     net_gnrc
 * @brief       Abstraction layer for GNRC's network interfaces
 *
 * Network interfaces in the context of GNRC are threads for protocols that are
 * below the network layer.
 *
 * @{
 *
 * @file
 * @brief   Definition for GNRC's network interfaces
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF_H
#define NET_GNRC_NETIF_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "kernel_types.h"
#include "msg.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/netif/conf.h"
#ifdef MODULE_GNRC_SIXLOWPAN
#include "net/gnrc/netif/6lo.h"
#endif
#if defined(MODULE_GNRC_NETIF_DEDUP) && (GNRC_NETIF_L2ADDR_MAXLEN > 0)
#include "net/gnrc/netif/dedup.h"
#endif
#include "net/gnrc/netif/flags.h"
#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/netif/ipv6.h"
#endif
#ifdef MODULE_GNRC_MAC
#include "net/gnrc/netif/mac.h"
#endif
#include "net/ndp.h"
#include "net/netdev.h"
#include "net/netopt.h"
#ifdef MODULE_NETSTATS_L2
#include "net/netstats.h"
#endif
#include "rmutex.h"
#include "net/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Operations to an interface
 */
typedef struct gnrc_netif_ops gnrc_netif_ops_t;

/**
 * @brief   Representation of a network interface
 */
typedef struct {
    netif_t netif;                          /**< network interface descriptor */
    const gnrc_netif_ops_t *ops;            /**< Operations of the network interface */
    netdev_t *dev;                          /**< Network device of the network interface */
    rmutex_t mutex;                         /**< Mutex of the interface */
#ifdef MODULE_NETSTATS_L2
    netstats_t stats;                       /**< transceiver's statistics */
#endif
#if defined(MODULE_GNRC_IPV6) || DOXYGEN
    gnrc_netif_ipv6_t ipv6;                 /**< IPv6 component */
#endif
#if defined(MODULE_GNRC_MAC) || DOXYGEN
    gnrc_netif_mac_t mac;                  /**< @ref net_gnrc_mac component */
#endif  /* MODULE_GNRC_MAC */
    /**
     * @brief   Flags for the interface
     *
     * @see net_gnrc_netif_flags
     */
    uint32_t flags;
#if (GNRC_NETIF_L2ADDR_MAXLEN > 0) || DOXYGEN
    /**
     * @brief   The link-layer address currently used as the source address
     *          on this interface.
     *
     * @note    Only available if @ref GNRC_NETIF_L2ADDR_MAXLEN > 0
     */
    uint8_t l2addr[GNRC_NETIF_L2ADDR_MAXLEN];

    /**
     * @brief   Length in bytes of gnrc_netif_t::l2addr
     *
     * @note    Only available if @ref GNRC_NETIF_L2ADDR_MAXLEN > 0
     */
    uint8_t l2addr_len;
#if defined(MODULE_GNRC_NETIF_DEDUP) || DOXYGEN
    /**
     * @brief   Last received packet information
     *
     * @note    Only available with @ref net_gnrc_netif_dedup.
     */
    gnrc_netif_dedup_t last_pkt;
#endif
#endif
#if defined(MODULE_GNRC_SIXLOWPAN) || DOXYGEN
    gnrc_netif_6lo_t sixlo;                 /**< 6Lo component */
#endif
    uint8_t cur_hl;                         /**< Current hop-limit for out-going packets */
    uint8_t device_type;                    /**< Device type */
    kernel_pid_t pid;                       /**< PID of the network interface's thread */
} gnrc_netif_t;

/**
 * @see gnrc_netif_ops_t
 */
struct gnrc_netif_ops {
    /**
     * @brief   Initializes network interface beyond the default settings
     *
     * @pre `netif != NULL`
     *
     * @param[in] netif The network interface.
     *
     * This is called after the network device's initial configuration, right
     * before the interface's thread starts receiving messages. It is not
     * necessary to lock the interface's mutex gnrc_netif_t::mutex, since it is
     * already locked. Set to @ref gnrc_netif_default_init() if you do not need
     * any special initialization. If you do need special initialization, it is
     * recommended to call @ref gnrc_netif_default_init() at the start of the
     * custom initialization function set here.
     */
    void (*init)(gnrc_netif_t *netif);

    /**
     * @brief   Send a @ref net_gnrc_pkt "packet" over the network interface
     *
     * @pre `netif != NULL && pkt != NULL`
     *
     * @note The function re-formats the content of @p pkt to a format expected
     *       by the netdev_driver_t::send() method of gnrc_netif_t::dev and
     *       releases the packet before returning (so no additional release
     *       should be required after calling this method).
     *
     * @param[in] netif The network interface.
     * @param[in] pkt   A packet to send.
     *
     * @return  The number of bytes actually sent on success
     * @return  -EBADMSG, if the @ref net_gnrc_netif_hdr in @p pkt is missing
     *          or is in an unexpected format.
     * @return  -ENOTSUP, if sending @p pkt in the given format isn't supported
     *          (e.g. empty payload with Ethernet).
     * @return  Any negative error code reported by gnrc_netif_t::dev.
     */
    int (*send)(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);

    /**
     * @brief   Receives a @ref net_gnrc_pkt "packet" from the network interface
     *
     * @pre `netif != NULL`
     *
     * @note The function takes the bytes received via netdev_driver_t::recv()
     *       from gnrc_netif_t::dev and re-formats it to a
     *       @ref net_gnrc_pkt "packet" containing a @ref net_gnrc_netif_hdr
     *       and a payload header in receive order.
     *
     * @param[in] netif The network interface.
     *
     * @return  The packet received. Contains the payload (with the type marked
     *          accordingly) and a @ref net_gnrc_netif_hdr in receive order.
     * @return  NULL, if @ref net_gnrc_pktbuf was full.
     */
    gnrc_pktsnip_t *(*recv)(gnrc_netif_t *netif);

    /**
     * @brief   Gets an option from the network interface
     *
     * Use gnrc_netif_get_from_netdev() to just get options from
     * gnrc_netif_t::dev.
     *
     * @param[in] netif     The network interface.
     * @param[in] opt       The option parameters.
     *
     * @return  Number of bytes in @p data.
     * @return  -EOVERFLOW, if @p max_len is lesser than the required space.
     * @return  -ENOTSUP, if @p opt is not supported to be set.
     * @return  Any negative error code reported by gnrc_netif_t::dev.
     */
    int (*get)(gnrc_netif_t *netif, gnrc_netapi_opt_t *opt);

    /**
     * @brief  Sets an option from the network interface
     *
     * Use gnrc_netif_set_from_netdev() to just set options from
     * gnrc_netif_t::dev.
     *
     * @param[in] netif     The network interface.
     * @param[in] opt       The option parameters.
     *
     * @return  Number of bytes written to gnrc_netif_t::dev.
     * @return  -EOVERFLOW, if @p data_len is greater than the allotted space in
     *          gnrc_netif_t::dev or gnrc_netif_t.
     * @return  -ENOTSUP, if @p opt is not supported to be set.
     * @return  Any negative error code reported by gnrc_netif_t::dev.
     */
    int (*set)(gnrc_netif_t *netif, const gnrc_netapi_opt_t *opt);

    /**
     * @brief   Message handler for network interface
     *
     * This message handler is used, when the network interface needs to handle
     * message types beyond the ones defined in @ref net_gnrc_netapi "netapi".
     * Leave NULL if this is not the case.
     *
     * @param[in] netif The network interface.
     * @param[in] msg   Message to be handled.
     */
    void (*msg_handler)(gnrc_netif_t *netif, msg_t *msg);
};

/**
 * @brief   Creates a network interface
 *
 * @param[in] stack     The stack for the network interface's thread.
 * @param[in] stacksize Size of @p stack.
 * @param[in] priority  Priority for the network interface's thread.
 * @param[in] name      Name for the network interface. May be NULL.
 * @param[in] dev       Device for the interface.
 * @param[in] ops       Operations for the network interface.
 *
 * @note If @ref DEVELHELP is defined netif_params_t::name is used as the
 *       name of the network interface's thread.
 *
 * @attention   Fails and crashes (assertion error with @ref DEVELHELP or
 *              segmentation fault without) if `GNRC_NETIF_NUMOF` is lower than
 *              the number of calls to this function.
 *
 * @return  The network interface on success.
 */
gnrc_netif_t *gnrc_netif_create(char *stack, int stacksize, char priority,
                                const char *name, netdev_t *dev,
                                const gnrc_netif_ops_t *ops);

/**
 * @brief   Get number of network interfaces actually allocated
 *
 * @return  Number of network interfaces actually allocated
 */
unsigned gnrc_netif_numof(void);

/**
 * @brief   Iterate over all network interfaces.
 *
 * @param[in] prev  previous interface in iteration. NULL to start iteration.
 *
 * @return  The next network interface after @p prev.
 * @return  NULL, if @p prev was the last network interface.
 */
gnrc_netif_t *gnrc_netif_iter(const gnrc_netif_t *prev);

/**
 * @brief   Get network interface by PID
 *
 * @param[in] pid   A PID of a network interface.
 *
 * @return  The network interface on success.
 * @return  NULL, if no network interface with PID exists.
 */
gnrc_netif_t *gnrc_netif_get_by_pid(kernel_pid_t pid);

/**
 * @brief   Gets the (unicast on anycast) IPv6 address of an interface (if IPv6
 *          is supported)
 *
 * @pre `netif != NULL`
 * @pre `addrs != NULL`
 * @pre `max_len >= sizeof(ipv6_addr_t)`
 *
 * @param[in] netif     The interface. May not be `NULL`.
 * @param[out] addrs    Up to the first `max_len / sizeof(ipv6_addr_t)`
 *                      addresses assigned to @p netif. May not be `NULL`
 * @param[in] max_len   Number of *bytes* available in @p addrs. Must be at
 *                      least `sizeof(ipv6_addr_t)`. It is recommended to use
 *                      @p GNRC_NETIF_IPV6_ADDRS_NUMOF `* sizeof(ipv6_addr_t)
 *                      here (and have @p addrs of the according length).
 *
 * @return  Number of addresses in @p addrs times `sizeof(ipv6_addr_t)` on
 *          success (including 0).
 * @return  -ENOTSUP, if @p netif doesn't support IPv6.
 */
static inline int gnrc_netif_ipv6_addrs_get(const gnrc_netif_t *netif,
                                            ipv6_addr_t *addrs,
                                            size_t max_len)
{
    assert(netif != NULL);
    assert(addrs != NULL);
    assert(max_len >= sizeof(ipv6_addr_t));
    return gnrc_netapi_get(netif->pid, NETOPT_IPV6_ADDR, 0, addrs, max_len);
}

/**
 * @brief   Adds an (unicast or anycast) IPv6 address to an interface (if IPv6
 *          is supported)
 *
 * @pre `netif != NULL`
 * @pre `addr != NULL`
 * @pre `(pfx_len > 0) && (pfx_len <= 128)`
 *
 * @param[in] netif     The interface. May not be `NULL`.
 * @param[in] addr      The address to add to @p netif. May not be `NULL`.
 * @param[in] pfx_len   The prefix length of @p addr. Must be greater than 0 and
 *                      lesser than or equal to 128.
 * @param[in] flags     [Flags](@ref net_gnrc_netif_ipv6_addrs_flags) for
 *                      @p addr. Set @ref GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID
 *                      to skip duplicate address detection (when activated).
 *
 * @return  sizeof(ipv6_addr_t) on success.
 * @return  -ENOMEM, if no space is left on @p netif to add @p addr or its
 *          corresponding solicited-nodes multicast address.
 * @return  -ENOTSUP, if @p netif doesn't support IPv6.
 */
static inline int gnrc_netif_ipv6_addr_add(const gnrc_netif_t *netif,
                                           ipv6_addr_t *addr, unsigned pfx_len,
                                           uint8_t flags)
{
    assert(netif != NULL);
    assert(addr != NULL);
    assert((pfx_len > 0) && (pfx_len <= 128));
    return gnrc_netapi_set(netif->pid, NETOPT_IPV6_ADDR,
                           ((pfx_len << 8U) | flags), addr,
                           sizeof(ipv6_addr_t));
}

/**
 * @brief   Removes a (unicast or anycast) IPv6 address from an interface (if
 *          IPv6 is supported)
 *
 * @pre `netif != NULL`
 * @pre `addr != NULL`
 *
 * @param[in] netif     The interface. May not be `NULL`.
 * @param[in] addr      The address to remove from @p netif. May not be `NULL`.
 *
 * @return  sizeof(ipv6_addr_t) on success.
 * @return  -ENOTSUP, if @p netif doesn't support IPv6.
 */
static inline int gnrc_netif_ipv6_addr_remove(const gnrc_netif_t *netif,
                                              ipv6_addr_t *addr)
{
    assert(netif != NULL);
    assert(addr != NULL);
    return gnrc_netapi_set(netif->pid, NETOPT_IPV6_ADDR_REMOVE,
                           0, addr, sizeof(ipv6_addr_t));
}

/**
 * @brief   Gets the IPv6 multicast groups an interface is joined to (if IPv6 is
 *          supported)
 *
 * @pre `netif != NULL`
 * @pre `groups != NULL`
 * @pre `max_len >= sizeof(ipv6_addr_t)`
 *
 * @param[in] netif     The interface. May not be `NULL`.
 * @param[out] groups   Up to the first `max_len / 8` multicast groups @p netif
 *                      is joined to. May not be `NULL`
 * @param[in] max_len   Number of *bytes* available in @p groups. Must be at
 *                      least `sizeof(ipv6_addr_t)`. It is recommended to use
 *                      @p GNRC_NETIF_IPV6_GROUPS_NUMOF `* sizeof(ipv6_addr_t)
 *                      here (and have @p groups of the according length).
 *
 * @return  Number of addresses in @p groups times `sizeof(ipv6_addr_t)` on
 *          success (including 0).
 * @return  -ENOTSUP, if @p netif doesn't support IPv6.
 */
static inline int gnrc_netif_ipv6_groups_get(const gnrc_netif_t *netif,
                                             ipv6_addr_t *groups,
                                             size_t max_len)
{
    assert(netif != NULL);
    assert(groups != NULL);
    assert(max_len >= sizeof(ipv6_addr_t));
    return gnrc_netapi_get(netif->pid, NETOPT_IPV6_GROUP, 0, groups, max_len);
}

/**
 * @brief   Joins an IPv6 multicast group on an interface (if IPv6 is supported)
 *
 * @pre `netif != NULL`
 * @pre `group != NULL`
 *
 * @param[in] netif     The interface.
 * @param[in] group     The address of the multicast group to join on @p netif.
 *                      May not be `NULL`.
 *
 * @return  sizeof(ipv6_addr_t) on success.
 * @return  -ENOMEM, if no space is left on @p netif to add @p group.
 * @return  -ENOTSUP, if @p netif doesn't support IPv6.
 */
static inline int gnrc_netif_ipv6_group_join(const gnrc_netif_t *netif,
                                             ipv6_addr_t *group)
{
    assert(netif != NULL);
    assert(group != NULL);
    return gnrc_netapi_set(netif->pid, NETOPT_IPV6_GROUP, 0, group,
                           sizeof(ipv6_addr_t));
}

/**
 * @brief   Leaves an IPv6 multicast group on an interface (if IPv6 is supported)
 *
 * @pre `netif != NULL`
 * @pre `group != NULL`
 *
 * @param[in] netif     The interface.
 * @param[in] group     The address of the multicast group to leave on @p netif.
 *                      May not be `NULL`.
 *
 * @return  sizeof(ipv6_addr_t) on success.
 * @return  -ENOTSUP, if @p netif doesn't support IPv6.
 */
static inline int gnrc_netif_ipv6_group_leave(const gnrc_netif_t *netif,
                                              ipv6_addr_t *group)
{
    assert(netif != NULL);
    assert(group != NULL);
    return gnrc_netapi_set(netif->pid, NETOPT_IPV6_GROUP_LEAVE, 0, group,
                           sizeof(ipv6_addr_t));
}

/**
 * @brief   Default operation for gnrc_netif_ops_t::init()
 *
 * @note    Can also be used to be called *before* a custom operation.
 *
 * @param[in] netif     The network interface.
 */
void gnrc_netif_default_init(gnrc_netif_t *netif);

/**
 * @brief   Default operation for gnrc_netif_ops_t::get()
 *
 * @note    Can also be used to be called *after* a custom operation.
 *
 * @param[in] netif     The network interface.
 * @param[out] opt      The option parameters.
 *
 * @return  Return value of netdev_driver_t::get() of gnrc_netif_t::dev of
 *          @p netif.
 */
int gnrc_netif_get_from_netdev(gnrc_netif_t *netif, gnrc_netapi_opt_t *opt);

/**
 * @brief   Default operation for gnrc_netif_ops_t::set()
 *
 * @note    Can also be used to be called *after* a custom operation.
 *
 * @param[in] netif     The network interface.
 * @param[in] opt       The option parameters.
 *
 * @return  Return value of netdev_driver_t::set() of gnrc_netif_t::dev of
 *          @p netif.
 */
int gnrc_netif_set_from_netdev(gnrc_netif_t *netif,
                               const gnrc_netapi_opt_t *opt);

/**
 * @brief   Converts a hardware address to a human readable string.
 *
 * @details The format will be like `xx:xx:xx:xx` where `xx` are the bytes
 *          of @p addr in hexadecimal representation.
 *
 * @pre `(out != NULL) && ((addr != NULL) || (addr_len == 0))`
 * @pre @p out **MUST** have allocated at least 3 * @p addr_len bytes.
 *
 * @param[in] addr      A hardware address.
 * @param[in] addr_len  Length of @p addr.
 * @param[out] out      A string to store the output in. Must at least have
 *                      3 * @p addr_len bytes allocated.
 *
 * @return  @p out.
 */
char *gnrc_netif_addr_to_str(const uint8_t *addr, size_t addr_len, char *out);

/**
 * @brief   Parses a string of colon-separated hexadecimals to a hardware
 *          address.
 *
 * @details The input format must be like `xx:xx:xx:xx` where `xx` will be the
 *          bytes of @p addr in hexadecimal representation.
 *
 * @pre `(out != NULL)`
 * @pre @p out **MUST** have allocated at least
 *      @ref GNRC_NETIF_L2ADDR_MAXLEN bytes.
 *
 * @param[in] str       A string of colon-separated hexadecimals.
 * @param[out] out      The resulting hardware address. Must at least have
 *                      @ref GNRC_NETIF_L2ADDR_MAXLEN bytes allocated.
 *
 * @return  Actual length of @p out on success.
 * @return  0, on failure.
 */
size_t gnrc_netif_addr_from_str(const char *str, uint8_t *out);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_H */
/** @} */
