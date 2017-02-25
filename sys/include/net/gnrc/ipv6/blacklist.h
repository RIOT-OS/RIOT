/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2016 Martin Landsmann <martin.landsmann@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_blacklist IPv6 address blacklist
 * @ingroup     net_gnrc_ipv6
 * @brief       This refuses IPv6 addresses that are defined in this list.
 * @{
 *
 * @file
 * @brief   IPv6 blacklist definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Martin Landsmann <martin.landsmann@haw-hamburg.de>
 */
#ifndef GNRC_IPV6_BLACKLIST_H
#define GNRC_IPV6_BLACKLIST_H

#include <stdbool.h>

#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Maximum size of the blacklist.
 */
#ifndef GNRC_IPV6_BLACKLIST_SIZE
#define GNRC_IPV6_BLACKLIST_SIZE    (8)
#endif

/**
 * @brief   Adds an IPv6 address to the blacklist.
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  0, on success.
 * @return  -1, if blacklist is full.
 */
int gnrc_ipv6_blacklist_add(const ipv6_addr_t *addr);

/**
 * @brief   Removes an IPv6 address from the blacklist.
 *
 * Addresses not in the blacklist will be ignored.
 *
 * @param[in] addr  An IPv6 address.
 */
void gnrc_ipv6_blacklist_del(const ipv6_addr_t *addr);

/**
 * @brief   Checks if an IPv6 address is blacklisted.
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  true, if @p addr is blacklisted.
 * @return  false, if @p addr is not blacklisted.
 */
bool gnrc_ipv6_blacklisted(const ipv6_addr_t *addr);

/**
 * @brief   Prints the blacklist.
 */
void gnrc_ipv6_blacklist_print(void);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_IPV6_BLACKLIST_H */
/** @} */
