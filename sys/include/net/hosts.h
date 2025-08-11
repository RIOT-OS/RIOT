/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_hosts   Static Hostname definitions
 * @ingroup     net
 * @{
 *
 * @file
 * @brief       Static mappings from hostnames to IP addresses
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "flash_utils.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "xfa.h"

#ifdef __cplusplus
extern "C" {
#else

/**
 * @brief           A single IPv4 host in the list of hostnames.
 *
 * This type is used internally by the @ref HOST_IPV4 macro.
 */
typedef struct {
    FLASH_ATTR const char *name; /**< Name of the host    */
    const ipv4_addr_t addr;      /**< Address of the host */
} hostnames_v4_xfa_t;

/**
 * @brief           A single IPv6 host in the list of hostnames.
 *
 * This type is used internally by the @ref HOST_IPV6 macro.
 */
typedef struct {
    FLASH_ATTR const char *name; /**< Name of the host    */
    const ipv6_addr_t addr;      /**< Address of the host */
} hostnames_v6_xfa_t;

/**
 * @brief   Define IPv4 host
 *
 * @note    This is not available from C++, but a trivial C file can easily
 *          hook up a `extern "C"` function implemented in C++.
 *
 * This macro is a helper for defining a hostname -> address mapping and adding
 * it to the host names XFA (cross file array).
 *
 * Example:
 *
 * ```.c
 * #include "hosts.h"
 * HOST_IPV4(localhost, IPV4_ADDR_LOOPBACK);
 * ```
 */
#define HOST_IPV4(host, ip) \
    XFA_USE_CONST(hostnames_v4_xfa_t, hostnames_v4_xfa); \
    static FLASH_ATTR const char _xfa_ ## host ## _hostname[] = #host; \
    XFA_CONST(hostnames_v4_xfa_t, hostnames_v4_xfa, 0) _xfa_ ## host ## _host = { \
        .name = _xfa_ ## host ## _hostname, \
        .addr = ip, \
    };

/**
 * @brief   Define IPv6 host
 *
 * @note    This is not available from C++, but a trivial C file can easily
 *          hook up a `extern "C"` function implemented in C++.
 *
 * This macro is a helper for defining a hostname -> address mapping and adding
 * it to the host names XFA (cross file array).
 *
 * Example:
 *
 * ```.c
 * #include "hosts.h"
 * HOST_IPV6(localhost, IPV6_ADDR_LOOPBACK);
 * ```
 */
#define HOST_IPV6(host, ip) \
    XFA_USE_CONST(hostnames_v6_xfa_t, hostnames_v6_xfa); \
    static FLASH_ATTR const char _xfa_ ## host ## _hostname[] = #host; \
    XFA_CONST(hostnames_v6_xfa_t, hostnames_v6_xfa, 0) _xfa_ ## host ## _host = { \
        .name = _xfa_ ## host ## _hostname, \
        .addr = ip, \
    };
#endif /* __cplusplus */

/**
 * @brief Get IP address for a host name
 *
 * Host Name <-> IP address mappings can be statically defined with the
 * @see HOST_IPV6 macro.
 *
 * @param[in]   host_name       Host name to resolve into address
 * @param[out]  addr_out        Buffer to write result into, but be able to accommodate
 *                              an address of the selected family
 * @param[in]   family          Either AF_INET (IPv4), AF_INET6 (IPv6) or AF_UNSPEC
 *                              (either IPv4 or IPv6)
 *
 * @return      the size of the resolved address on success
 * @return      -ENOENT if no matching entry could be found
 */
int hosts_query(const char *host_name, void *addr_out, int family);

#ifdef __cplusplus
}
#endif

/** @} */
