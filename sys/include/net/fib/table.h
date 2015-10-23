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

#ifndef FIB_TABLE_H_
#define FIB_TABLE_H_

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
typedef struct fib_entry_t {
    /** interface ID */
    kernel_pid_t iface_id;
    /** Lifetime of this entry (an absolute time-point is stored by the FIB) */
    uint64_t lifetime;
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
 * @brief Meta information about the FIB table
 */
typedef struct {
    fib_entry_t *entries;   /**< array holding the FIB entries */
    size_t size;            /**< number of entries in this table */
    mutex_t mtx_access;     /** access mutex to control exclusive operations on calls */
    size_t notify_rp_pos;   /** registered RPs for notifications about unreachable destinations */
    kernel_pid_t notify_rp[FIB_MAX_REGISTERED_RP]; /** the kernel_pid_t for notifying the RPs */
    universal_address_container_t* prefix_rp[FIB_MAX_REGISTERED_RP]; /** prefix handled by the RP */
} fib_table_t;

#ifdef __cplusplus
}
#endif

#endif /* FIB_TABLE_H_ */
/** @} */
