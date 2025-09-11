/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_hosts   Static Hostname definitions
 * @ingroup     net
 * @brief       This enables functionality like `/etc/hosts` for @ref dns_query()
 *
 * This module allows to make static hostname - IP address mappings that will
 * be taken into consideration when using `dns_query()` or a higher level API that
 * makes use of `dns_query()`.
 *
 * To enable this, add the `hosts` module to your `Makefile`:
 *
 * ```
 * USEMODULE += hosts
 * ```
 *
 * # Usage
 * To add a static hostname mapping to your code, just declare it with `HOST_IPV4()`
 * or `HOST_IPV6()` anywhere in a `.c` file
 *
 * ```C
 * #define IPV4_ADDR_EXAMPLE_COM IPV4_ADDR_INIT(192,168,42,23)
 * #define IPV6_ADDR_EXAMPLE_COM {{ 0xfd, 0x00, 0x00, 0x00, \
 *                                  0x00, 0x00, 0x00, 0x00, \
 *                                  0x00, 0x00, 0x00, 0x00, \
 *                                  0x00, 0x00, 0x42, 0x23 }}
 *
 * HOST_IPV4(example.com, IPV4_ADDR_EXAMPLE_COM);
 * HOST_IPV6(example.com, IPV6_ADDR_EXAMPLE_COM);
 * ```
 *
 * The hostname should then be resolved by any tool or function that makes
 * use of `dns_query()`:
 *
 * ```
 * > ping example.com
 * 12 bytes from fd::42:23: icmp_seq=0 ttl=64 time=1.202 ms
 * 12 bytes from fd::42:23: icmp_seq=1 ttl=64 time=1.202 ms
 * 12 bytes from fd::42:23: icmp_seq=2 ttl=64 time=1.202 ms
 *
 * --- example.com PING statistics ---
 * 3 packets transmitted, 3 packets received, 0% packet loss
 * round-trip min/avg/max = 1.202/1.202/1.202 ms
 * ```
 *
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
 * @note    This is not available from C++, but a trivial C++ file can easily
 *          hook up a `extern "C"` function implemented in C.
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
 * @note    This is not available from C++, but a trivial C++ file can easily
 *          hook up a `extern "C"` function implemented in C.
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
 * @ref HOST_IPV6 macro.
 *
 * @param[in]   host_name       Host name to resolve into address
 * @param[out]  addr_out        Buffer to write result into, but be able to accommodate
 *                              an address of the selected family
 * @param[in]   family          Either AF_INET (IPv4), AF_INET6 (IPv6) or AF_UNSPEC
 *                              (either IPv4 or IPv6)
 *
 * @return      the size of the resolved address on success or an error code
 * @retval      -ENOENT if no matching entry could be found
 */
int hosts_query(const char *host_name, void *addr_out, int family);

#ifdef __cplusplus
}
#endif

/** @} */
