/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <errno.h>
#include <string.h>
#include "net/af.h"
#include "net/hosts.h"

#ifdef MODULE_IPV4_ADDR
/* define IPv4 hostnames cross file array */
XFA_INIT_CONST(hostnames_v4_xfa_t, hostnames_v4_xfa);
/* always add localhost */
HOST_IPV4(localhost, IPV4_ADDR_LOOPBACK);

static int _resolve_v4(const char *domain_name, ipv4_addr_t *dst)
{
    unsigned n = XFA_LEN(hostnames_v4_xfa_t, hostnames_v4_xfa);
    for (unsigned i = 0; i < n; i++) {
        const volatile hostnames_v4_xfa_t *entry = &hostnames_v4_xfa[i];
        if (flash_strcmp(domain_name, entry->name) != 0) {
            continue;
        }
        const ipv4_addr_t *addr = (ipv4_addr_t *)&entry->addr;
        *dst = *addr;
        return sizeof(*dst);
    }

    return -ENOENT;
}
#else
static int _resolve_v4(const char *domain_name, ipv4_addr_t *dst)
{
    (void)domain_name;
    (void)dst;
    return -ENOENT;
}
#endif

#ifdef MODULE_IPV6_ADDR
/* define IPv6 hostnames cross file array */
XFA_INIT_CONST(hostnames_v6_xfa_t, hostnames_v6_xfa);
/* always add localhost */
HOST_IPV6(localhost, IPV6_ADDR_LOOPBACK);

static int _resolve_v6(const char *domain_name, ipv6_addr_t *dst)
{
    unsigned n = XFA_LEN(hostnames_v6_xfa_t, hostnames_v6_xfa);
    for (unsigned i = 0; i < n; i++) {
        const volatile hostnames_v6_xfa_t *entry = &hostnames_v6_xfa[i];
        if (flash_strcmp(domain_name, entry->name) != 0) {
            continue;
        }
        const ipv6_addr_t *addr = (ipv6_addr_t *)&entry->addr;
        *dst = *addr;
        return sizeof(*dst);
    }

    return -ENOENT;
}
#else
static int _resolve_v6(const char *domain_name, ipv6_addr_t *dst)
{

    (void)domain_name;
    (void)dst;
    return -ENOENT;
}
#endif

int hosts_query(const char *domain_name, void *addr_out, int family)
{
    int res = -ENOENT;

    if (res < 0 && (family == AF_UNSPEC || family == AF_INET6)) {
        res = _resolve_v6(domain_name, addr_out);
    }
    if (res < 0 && (family == AF_UNSPEC || family == AF_INET)) {
        res = _resolve_v4(domain_name, addr_out);
    }

    return res;
}
