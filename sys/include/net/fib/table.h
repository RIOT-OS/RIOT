/*
 * Copyright (C) 2014  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_fib
 * @{
 *
 * @file
 * @brief       Types and functions for operating fib tables
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 */

#ifndef NET_FIB_TABLE_H
#define NET_FIB_TABLE_H

#include <stdint.h>

#include "kernel_types.h"
#include "universal_address.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief maximum number of handled routing protocols (RP)
 *        used to notify the saved kernel_pid_t on unreachable destination
 */
#define FIB_MAX_REGISTERED_RP (5)

/**
 * @brief Container descriptor for a FIB entry
 */
typedef struct {
    /** interface ID */
    kernel_pid_t iface_id;
    /** Lifetime of this entry (an absolute time-point is stored by the FIB) */
    uint64_t lifetime;
    /** Unique identifier for the type of the global address */
    uint32_t global_flags;
    /** Pointer to the shared generic address */
    universal_address_container_t *global;
    /** Unique identifier for the type of the next hop address */
    uint32_t next_hop_flags;
    /** Pointer to the shared generic address */
    universal_address_container_t *next_hop;
} fib_entry_t;

/**
* @brief Container descriptor for a FIB source route entry
*/
typedef struct fib_sr_entry {
    /** Pointer to the shared generic address */
    universal_address_container_t *address;
    /** Pointer to the next shared generic address on the source route */
    struct fib_sr_entry *next;
} fib_sr_entry_t;

/**
* @brief Container descriptor for a FIB source route
*/
typedef struct {
    /** interface ID */
    kernel_pid_t sr_iface_id;
    /** Lifetime of this entry (an absolute time-point is stored by the FIB) */
    uint64_t sr_lifetime;
    /** Flags for this source route */
    uint32_t sr_flags;
    /** Pointer to the first hop on the source route */
    fib_sr_entry_t *sr_path;
    /** Pointer to the destination of the source route */
    fib_sr_entry_t *sr_dest;
} fib_sr_t;

/**
* @brief Container for one FIB source route table,
*        combining source routes and an entry pool
*/
typedef struct {
    /** pointer to source route header array */
    fib_sr_t *headers;
    /** pointer to entry pool array holding all hop entries for this table */
    fib_sr_entry_t *entry_pool;
    /** the maximum number of elements in the entry pool */
    size_t entry_pool_size;
} fib_sr_meta_t;

/**
* @brief FIB table type for single hop entries
*/
#define FIB_TABLE_TYPE_SH (1)

/**
* @brief FIB table type for source routes
*/
#define FIB_TABLE_TYPE_SR (FIB_TABLE_TYPE_SH + 1)

/**
* @brief Meta information of a FIB table
*/
typedef struct {
    /** A single hop OR source route data array */
    union{
        /** array holding the FIB entries for single hops */
        fib_entry_t *entries;
        /** array holding the FIB entries for source routes */
        fib_sr_meta_t *source_routes;
    }data;
    /** the kind of this FIB table, single hop or source route.
    *   This value indicates what is stored in `data` of this table
    */
    uint8_t table_type;
    /** the maximum number of entries in this FIB table */
    size_t size;
    /** table access mutex to grant exclusive operations on calls */
    mutex_t mtx_access;
    /** current number of registered RPs. */
    size_t notify_rp_pos;
    /** the kernel_pid_t of the registered RPs.
    *   Used to notify the RPs by the FIB on certain conditions,
    *   e.g. when a destination is unreachable
    */
    kernel_pid_t notify_rp[FIB_MAX_REGISTERED_RP];
    /** the prefix handled by each registered RP.
    *   Used to dispatch if the RP is responsible for the condition,
    *   e.g. when the unreachable destination is covered by the prefix
    */
    universal_address_container_t* prefix_rp[FIB_MAX_REGISTERED_RP];
} fib_table_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_FIB_TABLE_H */
/** @} */
