/*
 * Copyright (C) 2014  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief       forwarding table structs and base functions
 * @ingroup     net_fib
 *
 * @{
 *
 * @file
 * @brief       Types and functions for internally operating fib tables
 * @author      Martin Landsmann
 */

#ifndef FIB_TABLE_H_
#define FIB_TABLE_H_

#include <stdint.h>
#include "mutex.h"
#include "vtimer.h"
#include "ng_universal_address.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Container descriptor for a FIB entry
*/
typedef struct fib_entry_t {
    /** interface ID */
    kernel_pid_t iface_id;
    /** Lifetime of this entry */
    timex_t lifetime;
    /** Unique identifier for the type of the global address */
    uint32_t global_flags;
    /** Pointer to the shared generic address */
    struct universal_address_container_t *global;
    /** Unique identifier for the type of the next hop address */
    uint32_t next_hop_flags;
    /** Pointer to the shared generic address */
    struct universal_address_container_t *next_hop;
} fib_entry_t;

/**
 * @brief convert given ms to a timepoint from now on in the future
 * @param[in] ms the miliseconds to be converted
 * @param[out] timex the converted timepoint
 */
void fib_ms_to_timex(uint32_t ms, timex_t *timex);

/**
 * @brief returns pointer to the entry for the given destination address
 *
 * @param[in] dst                  the destination address
 * @param[in] dst_size             the destination address size
 * @param[out] entry_arr           the array to scribe the found match
 * @param[in, out] entry_arr_size  the number of entries provided by entry_arr (should be always 1)
 *                                 this value is overwritten with the actual found number
 *
 * @return 0 if we found a next-hop prefix
 *         1 if we found the exact address next-hop
 *         -EHOSTUNREACH if no fitting next-hop is available
 */
int fib_find_entry(uint8_t *dst, size_t dst_size,
                   fib_entry_t **entry_arr, size_t *entry_arr_size);

/**
* @brief updates the next hop the lifetime and the interface id for a given entry
*
* @param[in] entry          the entry to be updated
* @param[in] next_hop       the next hop address to be updated
* @param[in] next_hop_size  the next hop address size
* @param[in] next_hop_flags the next-hop address flags
* @param[in] lifetime       the lifetime in ms
*
* @return 0 if the entry has been updated
*         -ENOMEM if the entry cannot be updated due to unsufficient RAM
*/
int fib_upd_entry(fib_entry_t *entry,
                  uint8_t *next_hop, size_t next_hop_size, uint32_t next_hop_flags,
                  uint32_t lifetime);

/**
* @brief creates a new FIB entry with the provided parameters
*
* @param[in] iface_id       the interface ID
* @param[in] dst            the destination address
* @param[in] dst_size       the destination address size
* @param[in] dst_flags      the destination address flags
* @param[in] next_hop       the next hop address
* @param[in] next_hop_size  the next hop address size
* @param[in] next_hop_flags the next-hop address flags
* @param[in] lifetime       the lifetime in ms
*
* @return 0 on success
*         -ENOMEM if no new entry can be created
*/
int fib_create_entry(kernel_pid_t iface_id,
                     uint8_t *dst, size_t dst_size, uint32_t dst_flags,
                     uint8_t *next_hop, size_t next_hop_size, uint32_t next_hop_flags,
                     uint32_t lifetime);

/**
* @brief removes the given entry
*
* @param[in] entry the entry to be removed
*
* @return 0 on success
*/
int fib_remove(fib_entry_t *entry);

/**
* @brief returns the pointer to the access mutex
*
* @return the pointer to mtx_access
*/
mutex_t *fib_get_mutex(void);

/**
 * @brief signals (sends a message to) all registered routing protocols
 *        registered with a matching prefix (usually this should be only one).
 *        The message informs the recipient that no next-hop is available for the
 *        requested destination address.
 *        The receiver MUST copy the content, i.e. the address before reply.
 *
 * @param[in] dst       the destination address
 * @param[in] dst_size  the destination address size
 *
 * @return 0 on a new available entry,
 *         -ENOENT if no suiting entry is provided.
 */
int fib_signal_rp(uint8_t *dst, size_t dst_size, uint32_t dst_flags);

#ifdef __cplusplus
}
#endif

#endif /* FIB_TABLE_H_ */
/** @} */
