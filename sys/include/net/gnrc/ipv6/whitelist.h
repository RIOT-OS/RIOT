/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_whitelist IPv6 address whitelist
 * @ingroup     net_gnrc_ipv6
 * @brief       This allows you to only accept IPv6 addresses that are defined in this list.
 * @{
 *
 * @file
 * @brief   IPv6 whitelist definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_IPV6_WHITELIST_H
#define GNRC_IPV6_WHITELIST_H

#include <stdbool.h>

#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Maximum size of the whitelist.
 */
#ifndef GNRC_IPV6_WHITELIST_SIZE
#define GNRC_IPV6_WHITELIST_SIZE    (8)
#endif

/**
 * @brief   Adds an IPv6 address to the whitelist.
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  0, on success.
 * @return  -1, if whitelist is full.
 */
int gnrc_ipv6_whitelist_add(const ipv6_addr_t *addr);

/**
 * @brief   Removes an IPv6 address from the whitelist.
 *
 * Addresses not in the whitelist will be ignored.
 *
 * @param[in] addr  An IPv6 address.
 */
void gnrc_ipv6_whitelist_del(const ipv6_addr_t *addr);

/**
 * @brief   Checks if an IPv6 address is whitelisted.
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  true, if @p addr is whitelisted.
 * @return  false, if @p addr is not whitelisted.
 */
bool gnrc_ipv6_whitelisted(const ipv6_addr_t *addr);

/**
 * @brief   Prints the whitelist.
 */
void gnrc_ipv6_whitelist_print(void);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_IPV6_WHITELIST_H */
/** @} */
