/*
 * Copyright (C) 2014  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief       forwarding table structs
 * @ingroup     net_fib
 *
 * @{
 *
 * @file
 * @brief       Types and functions for operating fib tables
 * @author      Martin Landsmann
 */

#ifndef FIB_TABLE_H_
#define FIB_TABLE_H_

#include <stdint.h>
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

#ifdef __cplusplus
}
#endif

#endif /* FIB_TABLE_H_ */
/** @} */
