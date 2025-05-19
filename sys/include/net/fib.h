/*
 * Copyright (C) 2014  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_fib Forwarding Information Base (FIB)
 * @ingroup     net
 * @brief       FIB implementation
 *
 * This module is unused by RIOT's networking stacks, see @ref net_gnrc_ipv6_nib_ft
 * instead.
 * @{
 *
 * @file
 * @brief       Types and functions for FIB
 * @note        The used fib_table_t* used in all essential FIB functions
 *              MUST be valid.
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */


#include <stdint.h>

#include "net/fib/table.h"
#include "sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Routing Protocol (RP) message content to request/reply notification
 */
typedef struct {
    uint8_t *address;      /**< The pointer to the address */
    uint8_t address_size;  /**< The address size */
    uint32_t address_flags; /**< The flags for the given address */
} rp_address_msg_t;

/**
 * @brief message type for RP notification: no route to destination available,
 *        start route discovery if appropriate
 */
#define FIB_MSG_RP_SIGNAL_UNREACHABLE_DESTINATION (0x99)

/**
 * @brief message type for RP notification: entry used
 */
#define FIB_MSG_RP_SIGNAL_DESTINATION_USED (0x98)

/**
 * @brief message type for RP notification: source route created
 */
#define FIB_MSG_RP_SIGNAL_SOURCE_ROUTE_CREATED (0x97)

/**
 * @brief entry used to collect available destinations
 */
typedef struct {
    uint8_t dest[UNIVERSAL_ADDRESS_SIZE]; /**< The destination address */
    size_t dest_size;    /**< The destination address size */
} fib_destination_set_entry_t;

/**
 * @brief indicator of a lifetime that does not expire (2^64 - 1)
 */
#define FIB_LIFETIME_NO_EXPIRE (0xFFFFFFFFffffffff)

/**
 * @brief flag to identify if a route was set by RPL
 */
#define FIB_FLAG_RPL_ROUTE (1UL << 0)

/**
 * @brief flag used as shift for the net prefix length in bits
 */
#define FIB_FLAG_NET_PREFIX_SHIFT (24)

/**
 * @brief flag used as mask for the net prefix length in bits
 */
#define FIB_FLAG_NET_PREFIX_MASK (0xffUL << FIB_FLAG_NET_PREFIX_SHIFT)

/**
 * @brief initializes all FIB entries with 0
 *
 * @param[in] table         the fib instance to initialize
 */
void fib_init(fib_table_t *table);

/**
 * @brief de-initializes the FIB entries and source route entries
 *
 * @param[in] table     the fib instance to de-initialize
 */
void fib_deinit(fib_table_t *table);

/**
 * @brief Registration of a routing protocol handler function
 *
 * @param[in] table                 the fib table the RP should be registered to
 * @param[in] prefix                the prefix handled by the according RP
 * @param[in] prefix_addr_type_size the size of the address type used for the prefix
 *
 * @return 0 on success
 *           -ENOMEM if the entry cannot be registered (mximum registrations reached)
 *           -EINVAL if the prefix is NULL or the provided size is 0
 *
 */
int fib_register_rp(fib_table_t *table, uint8_t *prefix, size_t prefix_addr_type_size);

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
 * @brief removes all entries from the corresponding FIB table and interface combination
 *
 * @note if interface is KERNEL_PID_UNDEF, then all entries regardless of the interface
 *       will be removed.
 *
 * @param[in] table         the fib table to flush
 * @param[in] interface     entries associated with this interface will be removed
 */
void fib_flush(fib_table_t *table, kernel_pid_t interface);

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
* @brief creates a new source route
*
* @param[in, out] table the table the new source route belongs to
* @param[in, out] fib_sr pointer to store the new created source route pointer
* @param[in] sr_iface_id the interface ID used for the created source route
* @param[in] sr_flags the flags for the source route
* @param[in] sr_lifetime the lifetime in ms of the source route
*
* @return 0 on success
*         -EFAULT on wrong parameters, i.e. fib_sr is NULL and/or sr_lifetime is 0
*         -ENOBUFS on insufficient memory, i.e. all source route fields are in use
*/
int fib_sr_create(fib_table_t *table, fib_sr_t **fib_sr, kernel_pid_t sr_iface_id,
                  uint32_t sr_flags, uint32_t sr_lifetime);

/**
* @brief reads the information from the sr head to the given locations
*
* @param[in] table the fib instance to access
* @param[in] fib_sr pointer to the sr to read from
* @param[in, out] sr_iface_id pointer to store the iface_id
* @param[in, out] sr_flags pointer to store the flags
* @param[in, out] sr_lifetime pointer to store the current left lifetime
*
* @return 0 on success
*         -ENOENT on expired lifetime of the source route
*         -EFAULT on fib_sr is NULL
*/
int fib_sr_read_head(fib_table_t *table, fib_sr_t *fib_sr, kernel_pid_t *sr_iface_id,
                     uint32_t *sr_flags, uint32_t *sr_lifetime);

/**
* @brief reads the destination address from the sr head to the given location
*
* @param[in] table the fib instance to access
* @param[in] fib_sr pointer to the sr to read from
* @param[in, out] dst pointer to the store destination address bytes
* @param[in, out] dst_size pointer to store the destination address size
*
* @return 0 on success
*         -ENOENT on expired lifetime of the source route
*         -EFAULT on one of the parameter pointers is NULL
*         -ENOBUFS if the size to store the destination is insufficient low
*         -EHOSTUNREACH on the destination address is not set
*/
int fib_sr_read_destination(fib_table_t *table, fib_sr_t *fib_sr,
                            uint8_t *dst, size_t *dst_size);

/**
* @brief sets the provided parameters in the given sr header if a given parameter
*        is NULL its considered not to be changed
*
* @param[in] table the fib instance to access
* @param[in] fib_sr pointer to the sr to write to
* @param[in] sr_iface_id pointer to the new iface_id
* @param[in] sr_flags pointer to the new flags
* @param[in] sr_lifetime pointer to store the new lifetime
*
* @return 0 on success
*         -ENOENT on expired lifetime of the source route
*         -EFAULT on passed fib_sr is NULL
*/
int fib_sr_set(fib_table_t *table, fib_sr_t *fib_sr, kernel_pid_t *sr_iface_id,
               uint32_t *sr_flags, uint32_t *sr_lifetime);

/**
* @brief deletes the sr
*
* @param[in] table the fib instance to access
* @param[in, out] fib_sr pointer to the source route to be deleted
*
* @return 0 on success
*         -EFAULT on fib_sr pointer is NULL
*/
int fib_sr_delete(fib_table_t *table, fib_sr_t *fib_sr);

/**
* @brief iterates to the next entry in the sr_path
*
* @param[in] table the fib instance to access
* @param[in] fib_sr pointer to the sr to iterate
* @param[in, out] sr_path_entry pointer to the entry for iterating to the next entry
*
* @return 0 on success
*         1 on success, end reached
*         -ENOENT on expired lifetime of the source route
*         -EFAULT on fib_sr and/or sr_path_entry is NULL
*/
int fib_sr_next(fib_table_t *table, fib_sr_t *fib_sr, fib_sr_entry_t **sr_path_entry);

/**
* @brief searches the entry containing the given address
*
* @param[in] table the fib instance to access
* @param[in] fib_sr pointer to the sr to search for a hop address
* @param[in] addr pointer to the searched address bytes
* @param[in] addr_size the size in bytes of the searched address type
* @param[out] sr_path_entry pointer to the found address entry
*
* @return 0 on success
*         -EHOSTUNREACH when no address matches on the path
*         -ENOENT on expired lifetime of the source route
*         -EFAULT on one of the given parameter pointer is NULL
*/
int fib_sr_search(fib_table_t *table, fib_sr_t *fib_sr, uint8_t *addr, size_t addr_size,
                  fib_sr_entry_t **sr_path_entry);

/**
* @brief append a new entry at the end of the source route, i.e. a new destination
*
* @param[in] table the table with the source route to append the new entry
* @param[in] fib_sr pointer to the sr to append a hop address
* @param[in] dst pointer to the new destination address bytes
* @param[in] dst_size the size in bytes of the destination address type
*
* @return 0 on success
*         -EINVAL on the given destination is already on the path in the source route
*         -ENOENT on expired lifetime of the source route
*         -EFAULT on fib_sr and/or dst is NULL
*/
int fib_sr_entry_append(fib_table_t *table, fib_sr_t *fib_sr,
                        uint8_t *dst, size_t dst_size);

/**
* @brief adds a new entry behind a given sr entry
*
* @param[in] table the table with the source route to add the new entry
* @param[in] fib_sr pointer to the sr to add a hop address
* @param[in] sr_path_entry pointer to the entry after which we add the new one
* @param[in] addr pointer to the new address bytes
* @param[in] addr_size the size in bytes of the address type
* @param[in] keep_remaining_route indicate if the remaining route after sr_path_entry
*            should be kept and appended after the new entry
*
* @return 0 on success
*         -EFAULT on fib_sr and/or sr_path_entry and/or addr is NULL
*         -ENOENT on expired lifetime of the source route
*         -EINVAL on the given address is already present in the path
*/
int fib_sr_entry_add(fib_table_t *table, fib_sr_t *fib_sr,
                     fib_sr_entry_t *sr_path_entry, uint8_t *addr, size_t addr_size,
                     bool keep_remaining_route);

/**
* @brief removes an entry from a source route
*
* @param[in] table the fib instance to access
* @param[in] fib_sr pointer to the sr to delete a hop address
* @param[in] addr pointer to the address bytes to delete
* @param[in] addr_size the size in bytes of the address type
* @param[in] keep_remaining_route indicate if the remaining route
*            should be kept and appended after the predecessor of the removed entry
*
* @return 0 on success
*         -EFAULT on one of the passed pointers is NULL
*         -ENOENT on expired lifetime of the source route
*/
int fib_sr_entry_delete(fib_table_t *table, fib_sr_t *fib_sr, uint8_t *addr, size_t addr_size,
                        bool keep_remaining_route);

/**
* @brief overwrites the address of an entry with a new address
*
* @param[in] table the fib instance to access
* @param[in] fib_sr pointer to the sr to overwrite a hop address
* @param[in] addr_old pointer to the address bytes to overwrite
* @param[in] addr_old_size the size in bytes of the address type
* @param[in] addr_new pointer to the new address bytes
* @param[in] addr_new_size the size in bytes of the address type
*
* @return 0 on success
*         -EINVAL on the given address is already present in the path
*         -ENOMEM on no memory left to create a new address entry to overwrite the old one
*         -EFAULT on one of the passed pointers is NULL
*         -ENOENT on expired lifetime of the source route
*/
int fib_sr_entry_overwrite(fib_table_t *table, fib_sr_t *fib_sr,
                           uint8_t *addr_old, size_t addr_old_size,
                           uint8_t *addr_new, size_t addr_new_size);

/**
* @brief writes the address of an entry to the given out pointers
*
* @param[in] table the fib instance to access
* @param[in] fib_sr pointer to the sr to get a hop address
* @param[in] sr_path_entry pointer to the entry with the address
* @param[out] addr pointer to store the address bytes
* @param[in, out] addr_size the size in bytes of the address type
*
* @return 0 on success
*         -ENOMEM on insufficient memory provided to store the address
*         -EFAULT on one of the passed pointers is NULL
*         -ENOENT on expired lifetime of the source route
*/
int fib_sr_entry_get_address(fib_table_t *table, fib_sr_t *fib_sr, fib_sr_entry_t *sr_path_entry,
                             uint8_t *addr, size_t *addr_size);

/**
* @brief copies a source route to the given destination
*
* @param[in] table table to search for a source route
* @param[in] dst pointer to the destination address bytes
* @param[in] dst_size the size in bytes of the destination address type
* @param[out] sr_iface_id pointer to the store the iface_id for this route
* @param[in, out] sr_flags pointer to store the flags of this route
* @param[out] addr_list pointer to the location for storing the source route addresses
* @param[in, out] addr_list_size the number of elements available in addr_list
* @param[in, out] element_size the provided size for one element in addr_list
* @param[in] reverse indicator if the hops should be stored in reverse order
* @param[in, out] fib_sr pointer for cosecutive receiving matching source routes.
*                 If NULL only the first matching source route is returned.
*                 If !NULL the pointer will be overwritten with the current returned fib_sr.
*                 The FIB skips all entries until the provided fib_sr+1.
*                 The fib_sr pointer is only overwritten when a further matching sr has been found.
*
* @note The actual needed size for an element and the number of elements
*       is stored in addr_list_elements and element_size respectively
*       when the return value is NOT -EFAULT or NOT -EHOSTUNREACH.
*       However,the required size for may change in between calls.
*
* @return 0 on success, path to destination with equal flags
*         1 on success, path to destination with distinct flags
*         -EFAULT on one of the provided parameter pointers is NULL
*         -EHOSTUNREACH if no sr for the destination exists in the FIB
*         -ENOBUFS if the size to store all hops is insufficient low
*/
int fib_sr_get_route(fib_table_t *table, uint8_t *dst, size_t dst_size, kernel_pid_t *sr_iface_id,
                     uint32_t *sr_flags,
                     uint8_t *addr_list, size_t *addr_list_size, size_t *element_size,
                     bool reverse, fib_sr_t **fib_sr);

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

/**
* @brief Prints the given FIB sourceroute
*
* @param[in] table the fib instance to print
* @param [in] sr the source route to print
*/
void fib_print_sr(fib_table_t *table, fib_sr_t *sr);

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
int fib_devel_get_lifetime(fib_table_t *table, uint64_t *lifetime, uint8_t *dst,
                           size_t dst_size);

#endif

#ifdef __cplusplus
}
#endif

/** @} */
