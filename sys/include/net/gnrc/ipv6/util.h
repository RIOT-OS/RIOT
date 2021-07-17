/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup net_gnrc_ipv6_util     GNRC IPv6 utility functions
 * @ingroup  net_gnrc_ipv6
 * @brief   GNRC IPv6 utility functions
 * @{
 *
 * @file
 * @brief   GNRC IPv6 utility functions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_IPV6_UTIL_H
#define NET_GNRC_IPV6_UTIL_H

#include "net/ipv6/addr.h"
#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Configures a prefix delegation lease that is provided by the server.
 *
 * @param[in] netif     Network interface the prefix delegation was for.
 * @param[in] pfx       Prefix for the prefix delegation.
 * @param[in] pfx_len   Length of @p pfx in bits.
 * @param[in] valid     Valid lifetime of the prefix delegation.
 * @param[in] pref      Preferred lifetime of the prefix delegation.
 */
void gnrc_util_conf_prefix(gnrc_netif_t *netif, const ipv6_addr_t *pfx,
                           unsigned pfx_len, uint32_t valid,
                           uint32_t pref);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_UTIL_H */
/** @} */
