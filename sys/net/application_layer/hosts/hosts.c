/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

/* define hostnames cross file array */
XFA_INIT_CONST(hostnames_v6_xfa_t, hostnames_v6_xfa);

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

int hosts_query(const char *domain_name, void *addr_out, int family)
{
    int res = -ENOENT;

    if (family == AF_UNSPEC || family == AF_INET6) {
        res = _resolve_v6(domain_name, addr_out);
    }

    return res;
}

/* always add localhost */
HOST_IPV6(localhost, IPV6_ADDR_LOOPBACK);
