/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_utils   Network helper functions
 * @ingroup     net
 * @brief       Common network helper functions
 * @{
 *
 * @file
 * @brief       Common network interface API definitions
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <stdint.h>
#include <stddef.h>

#include "net/ipv6/addr.h"
#include "net/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Parse an IPv6 address / hostname string.
 *          If the @ref net_sock_dns module is used, this will
 *          attempt to resolve hostnames via DNS to IPv6 addresses.
 *
 * @param[out]  addr        IPv6 address of the host
 * @param[out]  netif       Interface if address is link-local
 * @param[in]   hostname    IPv6 address string or hostname
 *
 * @return  0 on success, error otherwise
 */
int netutils_get_ipv6(ipv6_addr_t *addr, netif_t **netif, const char *hostname);

#ifdef __cplusplus
}
#endif

#endif /* NET_UTILS_H */
/** @} */
