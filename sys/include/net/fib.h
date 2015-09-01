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
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef FIB_H_
#define FIB_H_

#include "net/fib/table.h"
#include "kernel_types.h"
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
 * @brief the size in bytes of a full address
 * @todo replace with UNIVERSAL_ADDRESS_SIZE (#3022)
*/
#define FIB_DESTINATION_SIZE_SUBSTITUTE (16)

/**
 * @brief entry used to collect available destinations
 */
typedef struct fib_destination_set_entry_t {
    uint8_t dest[FIB_DESTINATION_SIZE_SUBSTITUTE]; /**< The destination address */
    size_t dest_size;    /**< The destination address size */
} fib_destination_set_entry_t;

/**
 * @brief indicator of a lifetime that does not expire (2^32 - 1)
 */
#define FIB_LIFETIME_NO_EXPIRE (0xFFFFFFFF)

/**
 * @brief initializes all FIB entries with 0
 *
 * @param[in] table         the fib instance to initialize
 *
 * @pre @p table is not NULL and points to a pre-filled struct
 */
void fib_init(fib_table_t *table);

/**
 * @brief de-initializes the FIB entries
 *
 * @param[in] table         the fib instance to de-initialize
 */
void fib_deinit(fib_table_t *table);

/**
 * @brief Registration of a routing protocol handler function
 *
 * @param[in] prefix                the prefix handled by the according RP
 * @param[in] prefix_addr_type_size the size of the address type used for the prefix
 *
 * @return 0 on success
 *           -ENOMEM if the entry cannot be registered (mximum registrations reached)
 *           -EINVAL if the prefix is NULL or the provided size is 0
 *
 */
int fib_register_rp(uint8_t *prefix, size_t prefix_addr_type_size);

/**
 * @brief Adds a new entry in the corresponding FIB table for global destination and next hop
 *
 * @param[in] table          the fib table the entry should be added to
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
 *         -EFAULT if dst and/or next_hop is not a valid pointer
 */
int fib_add_entry(fib_table_t *table, kernel_pid_t iface_id, uint8_t *dst,
                  size_t dst_size, uint32_t dst_flags, uint8_t *next_hop,
                  size_t next_hop_size, uint32_t next_hop_flags,
                  uint32_t lifetime);

/**
 * @brief Updates an entry in the FIB table with next hop and lifetime
 *
 * @param[in] table          the fib table containing the entry to update
 * @param[in] dst            the destination address
 * @param[in] dst_size       the destination address size
 * @param[in] next_hop       the next hop address to be updated
 * @param[in] next_hop_size  the next hop address size
 * @param[in] next_hop_flags the next-hop address flags
 * @param[in] lifetime       the lifetime in ms to be updates
 *
 * @return 0 on success
 *         -ENOMEM if the entry cannot be updated due to insufficient RAM
 *         -EFAULT if dst and/or next_hop is not a valid pointer
 */
int fib_update_entry(fib_table_t *table, uint8_t *dst, size_t dst_size,
                     uint8_t *next_hop, size_t next_hop_size,
                     uint32_t next_hop_flags, uint32_t lifetime);

/**
 * @brief removes an entry from the corresponding FIB table
 *
 * @param[in] table         the fib table containing the entry to remove
 * @param[in] dst           the destination address
 * @param[in] dst_size      the destination address size
 */
void fib_remove_entry(fib_table_t *table, uint8_t *dst, size_t dst_size);

/**
 * @brief provides a next hop for a given destination
 *
 * @param[in] table               the fib table that should be searched
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
 *         -EFAULT if dst and/or next_hop is not a valid pointer
 *         -EINVAL if one of the other passed out pointers is NULL
 */
int fib_get_next_hop(fib_table_t *table, kernel_pid_t *iface_id,
                     uint8_t *next_hop, size_t *next_hop_size,
                     uint32_t* next_hop_flags, uint8_t *dst, size_t dst_size,
                     uint32_t dst_flags);

/**
* @brief provides a set of destination addresses matching the given prefix
* If the out buffer is insufficient low or passed as NULL,
* the function will continue to count the number of matching entries
* and provide the number to the caller.
*
* @param[in] table              the fib table that should be searched
* @param[in] prefix             the destination address
* @param[in] prefix_size        the destination address size
* @param[out] dst_set           the destination addresses matching the prefix
* @param[in,out] dst_set_size   the number of entries available on in and used
*                               on out
*
* @return 0 on success
*         -EHOSTUNREACH if no entry matches the type in the FIB
*         -ENOBUFS if the size for the found entries is insufficient low
*                  The actual needed size is stored then in dst_set_size,
*                  however the required size may change in between calls.
*/
int fib_get_destination_set(fib_table_t *table, uint8_t *prefix,
                            size_t prefix_size,
                            fib_destination_set_entry_t *dst_set,
                            size_t *dst_set_size);

/**
 * @brief returns the actual number of used FIB entries
 *
 * @param[in] table         the fib instance to check
 */
int fib_get_num_used_entries(fib_table_t *table);

/**
 * @brief Prints the kernel_pid_t for all registered RRPs
 */
void fib_print_notify_rrp(void);

/**
 * @brief Prints the FIB content (does not print the entries)
 *
 * @param[in] table         the fib instance to print
 */
void fib_print_fib_table(fib_table_t *table);

/**
 * @brief Prints the FIB content
 *
 * @param[in] table         the fib instance to print
 */
void fib_print_routes(fib_table_t *table);

#if FIB_DEVEL_HELPER
/**
 * @brief get the point in time at which the entry for destination dst expires.
 *
 * @param[in]  table     the fib instance to check
 * @param[out] lifetime  pointer where the expiration time is written on success
 * @param[in]  dst       the destination address
 * @param[in]  dst_size  the destination address size
 *
 * @return 0             on success: entry for dst found and lifetime copied
 *         -EHOSTUNREACH if no fitting entry is available
 */
int fib_devel_get_lifetime(fib_table_t *table, timex_t *lifetime, uint8_t *dst,
                           size_t dst_size);

#endif

#ifdef __cplusplus
}
#endif

#endif /* FIB_H_ */
/** @} */
