/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     nhdp
 * @{
 *
 * @file
 * @brief       Local Information Base interface for NHDP
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef LIB_TABLE_H_
#define LIB_TABLE_H_

#include "kernel_types.h"

#include "rfc5444/rfc5444_writer.h"

#include "nhdp_address.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Local Interface Set entry (local interface tuple)
 */
typedef struct lib_entry {
    kernel_pid_t if_pid;                    /**< PID of the interface's handling thread */
    nhdp_addr_entry_t *if_addr_list_head;   /**< Pointer to head of this interface's addr list */
    struct lib_entry *next;                 /**< Pointer to next list entry */
} lib_entry_t;

/**
 * @brief                   Add an interface to the Local Information Base
 *
 * This function can also be used to add an additional address to an existing LIB tuple.
 *
 * @param[in] if_pid        PID of the interface
 * @param[in] addr          The (additional) NHDP address to register for the interface
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lib_add_if_addr(kernel_pid_t if_pid, nhdp_addr_t *addr);

/**
 * @brief                   Remove a given interface's Local Information Base entry
 *
 * @param[in] if_pid        PID of the interface that should be removed
 */
void lib_rem_if(kernel_pid_t if_pid);

/**
 * @brief                   Add addresses to the currently constructed HELLO message
 *
 * @note
 * Must not be called from outside the NHDP writer's message creation process.
 *
 * @param[in] if_pid        PID of the interface the message is constructed for
 * @param[in] wr            The NHDP writer used for message construction
 */
void lib_fill_wr_addresses(kernel_pid_t if_pid, struct rfc5444_writer *wr);

/**
 * @brief                   Check whether a given NHDP address is used as a local address
 *
 * @param[in] if_pid        PID of the interface to check for
 * @param[in] addr          Pointer to the NHDP address that has to be checked
 *
 * @return                  1 if the given address is assigned to the given interface
 * @return                  2 if the given address is assigned to any other local interface
 * @return                  0 otherwise
 */
uint8_t lib_is_reg_addr(kernel_pid_t if_pid, nhdp_addr_t *addr);

#ifdef __cplusplus
}
#endif

#endif /* LIB_TABLE_H_ */
/** @} */
