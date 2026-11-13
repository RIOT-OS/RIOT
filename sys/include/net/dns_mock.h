/*
 * SPDX-FileCopyrightText: 2022 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
 * @author  Martine S. Lenders <mlenders@riot-os.org>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"

#ifndef SOCK_DNS_MOCK_HOSTNAME
/**
 * @brief Hostname used to query.
 */
#define SOCK_DNS_MOCK_HOSTNAME "example.com"
#endif

#ifndef SOCK_DNS_MOCK_ADDR_IPV4
/**
 * @brief IPv4 address)for @ref SOCK_DNS_MOCK_HOSTNAME
 */
#define SOCK_DNS_MOCK_ADDR_IPV4 IPV4_ADDR_INIT(93, 184, 216, 34)
#endif

#ifndef SOCK_DNS_MOCK_ADDR_IPV6
/**
 * @brief IPv6 address for @ref SOCK_DNS_MOCK_HOSTNAME
 */
#define SOCK_DNS_MOCK_ADDR_IPV6 { \
    0x26, 0x06, 0x28, 0x00, 0x02, 0x20, 0x00, 0x01, \
    0x02, 0x48, 0x18, 0x93, 0x25, 0xc8, 0x19, 0x46  \
}
#endif

/**
 * @brief IPv4 address for @ref SOCK_DNS_MOCK_HOSTNAME.
 */
static const ipv4_addr_t sock_dns_mock_addr_ipv4 =
    SOCK_DNS_MOCK_ADDR_IPV4;

/**
 * @brief IPv6 address for @ref SOCK_DNS_MOCK_HOSTNAME.
 *        Address represents "2606:2800:220:1:248:1893:25c8:1946".
 */
static const ipv6_addr_t sock_dns_mock_addr_ipv6 = {
    SOCK_DNS_MOCK_ADDR_IPV6
};

#ifdef __cplusplus
}
#endif

/** @} */
