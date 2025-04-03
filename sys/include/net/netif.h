/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_NETIF_H
#define NET_NETIF_H
/**
 * @defgroup    net_netif   Network interfaces
 * @ingroup     net
 * @brief       Common network interface API
 *
 * This allows access to network interfaces regardless of the network stack
 * implementation. The network stack must provide
 *
 * - A definition for @p netif_get_name
 * - A definition for @p netif_get_opt
 * - A definition for @p netif_set_opt
 *
 * The network stack should also register each interface via @p netif_register.
 *
 * @{
 *
 * @file
 * @brief   Common network interface API definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "list.h"
#include "net/netopt.h"
#include "net/ipv6.h"

#ifdef MODULE_NETSTATS_NEIGHBOR
#include "cib.h"
#include "net/netstats.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_netif_conf Network interfaces compile configurations
 * @ingroup  config
 * @{
 */
/**
 * @brief    Maximum length for an interface name
 */
#ifndef CONFIG_NETIF_NAMELENMAX
#define CONFIG_NETIF_NAMELENMAX    (8U)
#endif
/** @} */

/**
 * @brief Network interface descriptor.
 *
 * @note All network interfaces should inherit from this structure.
 */
typedef struct {
    list_node_t node;               /**< Pointer to the next interface */
#ifdef MODULE_NETSTATS_NEIGHBOR
    netstats_nb_table_t neighbors;  /**< Structure containing all L2 neighbors */
#endif
} netif_t;

/**
 * @brief   Iterator for the interfaces
 *
 * Returns interface after @p last. To start use `last == NULL`.
 *
 * @param[in] last  The previous interface. Use `NULL` to start
 *                  iteration.
 *
 * @return next network interface.
 * @return NULL, if there is no interface after @p last
 */
netif_t *netif_iter(const netif_t *last);

/**
 * @brief   Gets name of an interface
 *
 * @pre `name != NULL`
 * @pre name holds at least @ref CONFIG_NETIF_NAMELENMAX characters
 *
 * @note    Supposed to be implemented by the networking module. `name` must be
 *          zero-terminated in the result!
 *
 * @param[in] netif A network interface.
 * @param[out] name The name of the interface. Must not be `NULL`. Must at least
 *                  hold @ref CONFIG_NETIF_NAMELENMAX bytes.
 *
 * @return  length of @p name on success
 */

int netif_get_name(const netif_t *netif, char *name);

/**
 * @brief   Gets the numeric identifier of an interface
 *
 * @param[in] netif A network interface.
 *
 * @return  The numeric identifier of an interface
 * @return  -1 if @p netif is not registered
 */
int16_t netif_get_id(const netif_t *netif);

/**
 * @brief   Gets interface by name, from a buffer
 *
 * @pre `name != NULL`
 *
 * @param[in] name          The name of an interface as an array of chars. Must not be `NULL`.
 * @param[in] name_len      Number of characters in @p name.
 *
 * @return  Pointer to the interface that matches the name
 * @retval  NULL if no interface is named @p name.
 */
netif_t *netif_get_by_name_buffer(const char *name, size_t name_len);

/**
 * @brief   Gets interface by name
 *
 * @pre `name != NULL`
 *
 *
 * @param[in] name  The name of an interface as a zero-terminated. Must not be
 *                  `NULL`.
 *
 * @return  The interface on success.
 * @return  NULL if no interface is named @p name.
 */
static inline netif_t *netif_get_by_name(const char *name)
{
    return netif_get_by_name_buffer(name, strlen(name));
}

/**
 * @brief   Gets interface by a numeric identifier.
 *
 * @param[in] id  A numeric identifier.
 *
 * @return  The interface on success.
 * @return  NULL if no interface with identifier @p id.
 */
netif_t *netif_get_by_id(int16_t id);

/**
 * @brief   Gets option from an interface
 *
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in]   netif   A network interface.
 * @param[in]   opt     Option type.
 * @param[in]   context (Optional) context to the given option
 * @param[out]  value   Pointer to store the option's value in.
 * @param[in]   max_len Maximal amount of byte that fit into @p value.
 *
 * @return  Number of bytes written to @p value.
 * @return  `< 0` on error, 0 on success.
 */
int netif_get_opt(const netif_t *netif, netopt_t opt, uint16_t context,
                  void *value, size_t max_len);

/**
 * @brief   Sets option to an interface
 *
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in] netif     A network interface.
 * @param[in] opt       Option type.
 * @param[in] context   (Optional) context to the given option
 * @param[in] value     Pointer to store the option's value in.
 * @param[in] value_len The length of @p value.
 *
 * @return Number of bytes used from @p value.
 * @return `< 0` on error, 0 on success.
 */
int netif_set_opt(const netif_t *netif, netopt_t opt, uint16_t context,
                  void *value, size_t value_len);

/**
 * @brief   Registers a network interface in the global interface list.
 *
 * @note    This functions should be called when initializing an interface.
 *
 * @param[in] netif     Interface to be registered
 *
 * @return  0 on success
 * @return  -EINVAL if @p netif is NULL.
 */
int netif_register(netif_t *netif);

/**
 * @brief   Get IPv6 address(es) of the given interface
 * @param[in]   netif       Interface to get the IPv6 address(es) from
 * @param[out]  dest        Array of IPv6 addresses to write to
 * @param[in]   numof       Size of @p dest in array elements (not in bytes!)
 * @retval  -1              Failed
 * @return      Number of addresses written to @p dest
 */
static inline ssize_t netif_get_ipv6(netif_t *netif, ipv6_addr_t *dest,
                                     size_t numof)
{
    int res = netif_get_opt(netif, NETOPT_IPV6_ADDR, 0, dest, sizeof(*dest) * numof);
    if (res < 0) {
        /* standard says at ssize_t's value range is [-1, SSIZE_MAX], so do
         * not rely on smaller numbers that -1 being passed through correctly */
        return -1;
    }

    return res / sizeof(*dest);
}

/**
 * @brief   Get IPv6 address(es) of **all** interfaces
 * @param[out]  dest        Array of IPv6 addresses to write to
 * @param[in]   numof       Size of @p dest in array elements (not in bytes!)
 * @retval  -1              Failed
 * @return      Number of addresses written to @p dest
 */
ssize_t netifs_get_ipv6(ipv6_addr_t *dest, size_t numof);

/**
 * @brief   Print the IPv6 address(es) of the given interface
 * @param[in]   netif       Interface to print the IPv6 address(es) of
 * @param[in]   separator   Separator to print between the IPv6 addresses
 *
 * Usage:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *     // print IPv6 addrs of netif as JSON
 *     printf("{\"IPv6 addresses\": [\"");
 *     netif_print_ipv6(netif, "\", \"");
 *     puts("\"]}");
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void netif_print_ipv6(netif_t *netif, const char *separator);

/**
 * @brief   Print the IPv6 address(es) of **all** interface
 * @param[in]   separator   Separator to print between the IPv6 addresses
 *
 * Usage:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *     // print all IPv6 addrs as JSON
 *     printf("{\"IPv6 addresses\": [\"");
 *     netifs_print_ipv6("\", \"");
 *     puts("\"]}");
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void netifs_print_ipv6(const char *separator);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_NETIF_H */
