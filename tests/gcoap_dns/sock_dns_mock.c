/*
 * Copyright (C) 2021 Freie Universität Berlin
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
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <errno.h>
#include <string.h>

#include "net/af.h"
#include "net/ipv6/addr.h"
#include "net/sock/dns.h"

int sock_dns_query(const char *domain_name, void *addr_out, int family)
{
    const ipv6_addr_t a = { {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
        }
    };

    switch (family) {
        case AF_UNSPEC:
        case AF_INET6:
            break;
        default:
            return -EAFNOSUPPORT;
    }

    if (strcmp(domain_name, "example.org") != 0) {
        return -ENOTSUP;
    }

    memcpy(addr_out, &a, sizeof(a));
    return sizeof(a);
}
/** @} */

/** @} */
