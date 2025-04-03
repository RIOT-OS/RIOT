/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_DNS_MOCK_H
#define NET_DNS_MOCK_H
/**
 * @defgroup    net_dns_mock DNS defines
 * @ingroup     net
 * @brief       Generic DNS mock values
 * @{
 *
 * @file
 * @brief   Generic DNS mock values
 *
 * @author  Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"

/**
 * @brief Hostname used to query.
 */
#define SOCK_DNS_MOCK_EXAMPLE_COM_HOSTNAME "example.com"

/**
 * @brief IPv4 address for @ref SOCK_DNS_MOCK_EXAMPLE_COM_HOSTNAME.
 */
static const ipv4_addr_t sock_dns_mock_example_com_addr_ipv4 =
    IPV4_ADDR_INIT(93, 184, 216, 34);

/**
 * @brief IPv6 address for @ref SOCK_DNS_MOCK_EXAMPLE_COM_HOSTNAME.
 *        Address represents "2606:2800:220:1:248:1893:25c8:1946".
 */
static const ipv6_addr_t sock_dns_mock_example_com_addr_ipv6 = { {
        0x26, 0x06, 0x28, 0x00, 0x02, 0x20, 0x00, 0x01,
        0x02, 0x48, 0x18, 0x93, 0x25, 0xc8, 0x19, 0x46
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_DNS_MOCK_H */
