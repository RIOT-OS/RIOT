/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Mock implementation of sock_dns
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#include <string.h>

#include "net/af.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "net/sock/dns.h"

int sock_dns_query(const char *domain_name, void *addr_out, int family)
{
    const ipv4_addr_t addr_ipv4 = { { 0x5d, 0xb8, 0xd8, 0x22 } };

    const ipv6_addr_t addr_ipv6 = { {
            0x26, 0x06, 0x28, 0x00, 0x02, 0x20, 0x00, 0x01,
            0x02, 0x48, 0x18, 0x93, 0x25, 0xc8, 0x19, 0x46
        }
    };

    if (strcmp(domain_name, "example.com")) {
        return -ENOTSUP;
    }

    switch (family) {
    case AF_INET:
        memcpy(addr_out, &addr_ipv4, sizeof(addr_ipv4));
        return 0;
    case AF_INET6:
        memcpy(addr_out, &addr_ipv6, sizeof(addr_ipv6));
        return 0;
    default:
        return -EAFNOSUPPORT;
    }
}
/** @} */
