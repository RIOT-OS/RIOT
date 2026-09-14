/*
 * SPDX-FileCopyrightText: 2022 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup net_dns_mock
 * @{
 * @file
 * @brief   sock DNS mock implementation
 * @author  Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <errno.h>
#include <string.h>

#include "net/af.h"
#include "net/dns_mock.h"

int sock_dns_query(const char *domain_name, void *addr_out, int family)
{
    if (strcmp(domain_name, SOCK_DNS_MOCK_EXAMPLE_COM_HOSTNAME)) {
        return -ENOTSUP;
    }

    switch (family) {
    case AF_INET:
        memcpy(addr_out, &sock_dns_mock_example_com_addr_ipv4, sizeof(ipv4_addr_t));
        return sizeof(ipv4_addr_t);

    case AF_UNSPEC:
        /* fall-through */
    case AF_INET6:
        memcpy(addr_out, &sock_dns_mock_example_com_addr_ipv6, sizeof(ipv6_addr_t));
        return sizeof(ipv6_addr_t);
    default:
        return -EAFNOSUPPORT;
    }
}
