/*
 * Copyright (C) 2015  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_fib
 * @brief       FIB - generic address addon
 *
 * @{
 *
 * @file
 * @brief       extends the FIB functions to use generic addresses.
 *
 * @author      Martin Landsmann
 */

#ifndef FIB_ADDON_GENERIC_H_
#define FIB_ADDON_GENERIC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Adds a new entry in the corresponding FIB table for global destination and next hop
 *
 * @param[in] iface_id       the interface ID
 * @param[in] dst            the destination address
 * @param[in] dst_size       the destination address size
 * @param[in] dst_flags      the destination address flags
 * @param[in] next_hop       the next hop address to be updated
 * @param[in] next_hop_size  the next hop address size
 * @param[in] next_hop_flags the next-hop address flags
 * @param[in] lifetime       the lifetime in ms to be updates
 *
 * @return 0 on success
 *         -ENOMEM if the entry cannot be created due to unsufficient RAM
 */
int fib_add_entry(kernel_pid_t iface_id, uint8_t *dst, size_t dst_size, uint32_t dst_flags,
                  uint8_t *next_hop, size_t next_hop_size, uint32_t next_hop_flags,
                  uint32_t lifetime);

/**
 * @brief Updates an entry in the FIB table with next hop and lifetime
 *
 * @param[in] dst            the destination address
 * @param[in] dst_size       the destination address size
 * @param[in] next_hop       the next hop address to be updated
 * @param[in] next_hop_size  the next hop address size
 * @param[in] next_hop_flags the next-hop address flags
 * @param[in] lifetime       the lifetime in ms to be updates
 *
 * @return 0 on success
 *         -ENOMEM if the entry cannot be updated due to unsufficient RAM
 */
int fib_update_entry(uint8_t *dst, size_t dst_size,
                     uint8_t *next_hop, size_t next_hop_size, uint32_t next_hop_flags,
                     uint32_t lifetime);

/**
 * @brief removes an entry from the corresponding FIB table
 *
 * @param[in] dst       the destination address
 * @param[in] dst_size  the destination address size
 */
void fib_remove_entry(uint8_t *dst, size_t dst_size);

/**
 * @brief provides a next hop for a given destination
 *
 * @param[in, out] iface_id       pointer to store the interface ID for the next hop
 * @param[out] next_hop           pointer where the next hop address should be stored
 * @param[in, out] next_hop_size  the next hop address size. The value is
 *                                overwritten with the actual next hop size
 * @param[out] next_hop_flags     the next-hop address flags, e.g. compression type
 * @param[in] dst                 the destination address
 * @param[in] dst_size            the destination address size
 * @param[in] dst_flags           the destination address flags
 *
 * @return 0 on success
 *         -EHOSTUNREACH if no next hop is available in any FIB table
 *                                           all RRPs are notified before the return
 *         -ENOBUFS if the size for the next hop address is insufficient low
 */
int fib_get_next_hop(kernel_pid_t *iface_id,
                     uint8_t *next_hop, size_t *next_hop_size, uint32_t *next_hop_flags,
                     uint8_t *dst, size_t dst_size, uint32_t dst_flags);

#ifdef __cplusplus
}
#endif

#endif /* FIB_ADDON_GENERIC_H_ */
/** @} */
