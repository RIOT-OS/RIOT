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
 * @brief       Types and functions for the FIB
 * @author      Martin Landsmann
 */

#ifndef FIB_H_
#define FIB_H_

/**
* @note: The specific prototype functions to manipulate FIB entries are present in
* each of the following headers.
*
* This separation enables to extend the prototypes for specific address types easily.
* Additionally it allows to switch only the required function prototypes by
* only include the required fib_addon_*.h header(s).
* At least one header MUST be included.
*/
#include "ng_fib/ng_fib_addon_generic.h"
#include "ng_fib/ng_fib_addon_ipv6.h"

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
 * @brief maximum number of FIB table entries handled
 */
#define FIB_MAX_FIB_TABLE_ENTRIES (20)

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
 * @brief initializes all FIB entries with 0
 */
void fib_init(void);

/**
 * @brief de-initializes the FIB entries
 */
void fib_deinit(void);

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

#ifdef __cplusplus
}
#endif

#endif /* FIB_H_ */
/** @} */
