/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
static const ipv6_addr_t sock_dns_mock_example_com_addr_ipv6 =
    IPV6_ADDR_INIT(0x2606,0x2800,0x0220,0x1,0x248,0x1893,0x25c8,0x1946);

#ifdef __cplusplus
}
#endif

/** @} */
