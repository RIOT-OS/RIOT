/*
 * Copyright (C) 2015  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_fib
 * @brief       FIB - ipv6 addon
 *
 * @{
 *
 * @file
 * @brief       extends the FIB functions specifically for ipv6.
 *
 * @author      Martin Landsmann
 */

#ifndef FIB_ADDON_IPV6_H_
#define FIB_ADDON_IPV6_H_

#include "ng_ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Adds a new ipv6 entry in the FIB table for global destination and next hop
 *
 * @param[in] iface_id       the interface ID
 * @param[in] dst            the destination address
 * @param[in] next_hop       the next hop address to be updated
 * @param[in] lifetime       the lifetime in ms to be updates
 *
 * @return 0 on success
 *         -ENOMEM if the entry cannot be created due to unsufficient RAM
 */
int fib_add_entry_ipv6(kernel_pid_t iface_id, ng_ipv6_addr_t *dst,
                       ng_ipv6_addr_t *next_hop, uint32_t lifetime);

/**
 * @brief Updates an ipv6 entry in the FIB table with next hop and lifetime
 *
 * @param[in] dst            the destination address
 * @param[in] next_hop       the next hop address to be updated
 * @param[in] lifetime       the lifetime in ms to be updates
 *
 * @return 0 on success
 *         -ENOMEM if the entry cannot be updated due to unsufficient RAM
 */
int fib_update_entry_ipv6(ng_ipv6_addr_t *dst, ng_ipv6_addr_t *next_hop,
                          uint32_t lifetime);

/**
 * @brief removes an entry from the corresponding FIB table
 *
 * @param[in] dst       the destination address
 */
void fib_remove_entry_ipv6(ng_ipv6_addr_t *dst);

/**
 * @brief provides a next hop for a given destination
 *
 * @param[in, out] iface_id       pointer to store the interface ID for the next hop
 * @param[out] next_hop           pointer where the next hop address should be stored
 * @param[in] dst                 the destination address
 *
 * @return 0 on success
 *         -EHOSTUNREACH if no next hop is available in any FIB table
 *                                           all RRPs are notified before the return
 *         -ENOBUFS if the size for the next hop address is insufficient low
 */
int fib_get_next_hop_ipv6(kernel_pid_t *iface_id, ng_ipv6_addr_t *next_hop,
                          ng_ipv6_addr_t *dst);

#ifdef __cplusplus
}
#endif

#endif /* FIB_ADDON_IPV6_H_ */
/** @} */
