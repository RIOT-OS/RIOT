/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_netif2 New network interface API
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
#ifndef NET_GNRC_NETIF2_H
#define NET_GNRC_NETIF2_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "kernel_types.h"
#include "msg.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/netif2/conf.h"
#ifdef MODULE_GNRC_SIXLOWPAN
#include "net/gnrc/netif2/6lo.h"
#endif
#include "net/gnrc/netif2/flags.h"
#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/netif2/ipv6.h"
#endif
#ifdef MODULE_GNRC_MAC
#include "net/gnrc/netif2/mac.h"
#endif
#include "net/netdev.h"
#include "rmutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Operations to an interface
 */
typedef struct gnrc_netif2_ops gnrc_netif2_ops_t;

/**
 * @brief   Representation of a network interface
 */
typedef struct {
    const gnrc_netif2_ops_t *ops;           /**< Operations of the network interface */
    netdev_t *dev;                          /**< Network device of the network interface */
    rmutex_t mutex;                         /**< Mutex of the interface */
#if defined(MODULE_GNRC_IPV6) || DOXYGEN
    gnrc_netif2_ipv6_t ipv6;                /**< IPv6 component */
#endif
#if defined(MODULE_GNRC_MAC) || DOXYGEN
    gnrc_netif2_mac_t mac;                  /**< @ref net_gnrc_mac component */
#endif  /* MODULE_GNRC_MAC */
    /**
     * @brief   Flags for the interface
     *
     * @see net_gnrc_netif2_flags
     */
    uint32_t flags;
#if (GNRC_NETIF2_L2ADDR_MAXLEN > 0)
    /**
     * @brief   The link-layer address currently used as the source address
     *          on this interface.
     *
     * @note    Only available if @ref GNRC_NETIF2_L2ADDR_MAXLEN > 0
     */
    uint8_t l2addr[GNRC_NETIF2_L2ADDR_MAXLEN];

    /**
     * @brief   Length in bytes of gnrc_netif2_t::l2addr
     *
     * @note    Only available if @ref GNRC_NETIF2_L2ADDR_MAXLEN > 0
     */
    uint8_t l2addr_len;
#endif
#if defined(MODULE_GNRC_SIXLOWPAN) || DOXYGEN
    gnrc_netif2_6lo_t sixlo;                /**< 6Lo component */
#endif
    uint8_t cur_hl;                         /**< Current hop-limit for out-going packets */
    uint8_t device_type;                    /**< Device type */
    kernel_pid_t pid;                       /**< PID of the network interface's thread */
} gnrc_netif2_t;

/**
 * @see gnrc_netif2_ops_t
 */
struct gnrc_netif2_ops {
    /**
     * @brief   Initializes network interface beyond the default settings
     *
     * @pre `netif != NULL`
     *
     * @param[in] netif The network interface.
     *
     * This is called after the default settings were set, right before the
     * interface's thread starts receiving messages. It is not necessary to lock
     * the interface's mutex gnrc_netif_t::mutex, since the thread will already
     * lock it. Leave NULL if you do not need any special initialization.
     */
    void (*init)(gnrc_netif2_t *netif);

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
     * @return  Any negative error code reported by gnrc_netif2_t::dev.
     */
    int (*send)(gnrc_netif2_t *netif, gnrc_pktsnip_t *pkt);

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
    gnrc_pktsnip_t *(*recv)(gnrc_netif2_t *netif);

    /**
     * @brief   Gets an option from the network interface
     *
     * Use gnrc_netif2_get_from_netdev() to just get options from
     * gnrc_netif2_t::dev.
     *
     * @param[in] netif     The network interface.
     * @param[in] opt       The option parameters.
     *
     * @return  Number of bytes in @p data.
     * @return  -EOVERFLOW, if @p max_len is lesser than the required space.
     * @return  -ENOTSUP, if @p opt is not supported to be set.
     * @return  Any negative error code reported by gnrc_netif2_t::dev.
     */
    int (*get)(gnrc_netif2_t *netif, gnrc_netapi_opt_t *opt);

    /**
     * @brief  Sets an option from the network interface
     *
     * Use gnrc_netif2_set_from_netdev() to just set options from
     * gnrc_netif2_t::dev.
     *
     * @param[in] netif     The network interface.
     * @param[in] opt       The option parameters.
     *
     * @return  Number of bytes written to gnrc_netif2_t::dev.
     * @return  -EOVERFLOW, if @p data_len is greater than the allotted space in
     *          gnrc_netif2_t::dev or gnrc_netif_t.
     * @return  -ENOTSUP, if @p opt is not supported to be set.
     * @return  Any negative error code reported by gnrc_netif2_t::dev.
     */
    int (*set)(gnrc_netif2_t *netif, const gnrc_netapi_opt_t *opt);

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
    void (*msg_handler)(gnrc_netif2_t *netif, msg_t *msg);
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
gnrc_netif2_t *gnrc_netif2_create(char *stack, int stacksize, char priority,
                                  const char *name, netdev_t *dev,
                                  const gnrc_netif2_ops_t *ops);

/**
 * @brief   Get number of network interfaces actually allocated
 *
 * @return  Number of network interfaces actually allocated
 */
unsigned gnrc_netif2_numof(void);

/**
 * @brief   Iterate over all network interfaces.
 *
 * @param[in] prev  previous interface in iteration. NULL to start iteration.
 *
 * @return  The next network interface after @p prev.
 * @return  NULL, if @p prev was the last network interface.
 */
gnrc_netif2_t *gnrc_netif2_iter(const gnrc_netif2_t *prev);

/**
 * @brief   Get network interface by PID
 *
 * @param[in] pid   A PID of a network interface.
 *
 * @return  The network interface on success.
 * @return  NULL, if no network interface with PID exists.
 */
gnrc_netif2_t *gnrc_netif2_get_by_pid(kernel_pid_t pid);

/**
 * @brief   Default operation for gnrc_netif2_ops_t::get()
 *
 * @note    Can also be used to be called *after* a custom operation.
 *
 * @param[in] netif     The network interface.
 * @param[out] opt      The option parameters.
 *
 * @return  Return value of netdev_driver_t::get() of gnrc_netif2_t::dev of
 *          @p netif.
 */
int gnrc_netif2_get_from_netdev(gnrc_netif2_t *netif, gnrc_netapi_opt_t *opt);

/**
 * @brief   Default operation for gnrc_netif2_ops_t::set()
 *
 * @note    Can also be used to be called *after* a custom operation.
 *
 * @param[in] netif     The network interface.
 * @param[in] opt       The option parameters.
 *
 * @return  Return value of netdev_driver_t::set() of gnrc_netif2_t::dev of
 *          @p netif.
 */
int gnrc_netif2_set_from_netdev(gnrc_netif2_t *netif,
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
char *gnrc_netif2_addr_to_str(const uint8_t *addr, size_t addr_len, char *out);

/**
 * @brief   Parses a string of colon-separated hexadecimals to a hardware
 *          address.
 *
 * @details The input format must be like `xx:xx:xx:xx` where `xx` will be the
 *          bytes of @p addr in hexadecimal representation.
 *
 * @pre `(out != NULL)`
 * @pre @p out **MUST** have allocated at least
 *      @ref GNRC_NETIF2_L2ADDR_MAXLEN bytes.
 *
 * @param[in] str       A string of colon-separated hexadecimals.
 * @param[out] out      The resulting hardware address. Must at least have
 *                      @ref GNRC_NETIF2_L2ADDR_MAXLEN bytes allocated.
 *
 * @return  Actual length of @p out on success.
 * @return  0, on failure.
 */
size_t gnrc_netif2_addr_from_str(const char *str, uint8_t *out);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF2_H */
/** @} */
