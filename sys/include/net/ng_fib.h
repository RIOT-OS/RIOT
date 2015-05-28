/*
 * Copyright (C) 2014  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_fib Forwarding Information Base (FIB)
 * @ingroup     net
 * @brief       FIB implementation
 *
 * @{
 *
 * @file
 * @brief       Types and functions for FIB
 * @author      Martin Landsmann
 */

#ifndef FIB_H_
#define FIB_H_

#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Routing Protocol (RP) message content to request/reply notification
 */
typedef struct rp_address_msg_t {
    uint8_t *address;      /**< The pointer to the address */
    uint8_t address_size;  /**< The address size */
    uint32_t address_flags; /**< The flags for the given address */
} rp_address_msg_t;

#define FIB_MSG_RP_SIGNAL (0x99)     /**< message type for RP notifications */

/**
 * @brief indicator of a lifetime that does not expire (2^32 - 1)
 */
#define FIB_LIFETIME_NO_EXPIRE (0xFFFFFFFF)

/**
 * @brief initializes all FIB entries with 0
 */
void fib_init(void);

/**
 * @brief de-initializes the FIB entries
 */
void fib_deinit(void);

/**
 * @brief Registration of a routing protocol handler function
 *
 * @param[in] prefix the prefix handled by the according RP
 * @param[in] prefix_size the prefix size
 * @return 0 on success
 *           -ENOMEM if the entry cannot be registered (mximum registrations reached)
 *           -EINVAL if the prefix is NULL or the provided size is 0
 *
 */
int fib_register_rp(uint8_t *prefix, size_t prefix_size);

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
 *         -ENOMEM if the entry cannot be created due to insufficient RAM
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
 *         -ENOMEM if the entry cannot be updated due to insufficient RAM
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
                     uint8_t *next_hop, size_t *next_hop_size, uint32_t* next_hop_flags,
                     uint8_t *dst, size_t dst_size, uint32_t dst_flags);

/**
 * @brief returns the actual number of used FIB entries
 */
int fib_get_num_used_entries(void);

/**
 * @brief Prints the kernel_pid_t for all registered RRPs
 */
void fib_print_notify_rrp(void);

/**
 * @brief Prints the FIB content (does not print the entries)
 */
void fib_print_fib_table(void);

/**
 * @brief Prints the FIB content
 */
void fib_print_routes(void);

#if FIB_DEVEL_HELPER
/**
 * @brief get the point in time at which the entry for destination dst expires.
 *
 * @param[out] lifetime  pointer where the expiration time is written on success
 * @param[in]  dst       the destination address
 * @param[in]  dst_size  the destination address size
 *
 * @return 0             on success: entry for dst found and lifetime copied
 *         -EHOSTUNREACH if no fitting entry is available
 */
int fib_devel_get_lifetime(timex_t *lifetime, uint8_t *dst, size_t dst_size);
#endif

#ifdef __cplusplus
}
#endif

#endif /* FIB_H_ */
/** @} */
