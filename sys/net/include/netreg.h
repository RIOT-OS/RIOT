/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netreg Network module registry
 * @ingroup     net
 * @brief       Global registry for lookup of network protocol handlers and
 *              network interfaces
 * @{
 *
 * @file
 * @brief       Interface definition for the global network module registry
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __NETREG_H_
#define __NETREG_H_

#include <stdint.h>

#include "kernel.h"
#include "netmod.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Structure for holding one element int the registry
 */
typedef struct {
    netreg_entry_t *next;           /**< point to the next entry */
    kernel_pid_t pid;        /**< the PID of the referenced module */
} netreg_entry_t;

/**
 * @brief   Initialize the registry
 *
 * No return value, as this can not fail.
 */
void netreg_init(void);

/**
 * @brief   Register a network module
 *
 * @param[in] module        global protocol or interface number
 * @param[in] pid           the PID of the module
 *
 * @return                  something useful...
 */
int netreg_register(netmod_t module, kernel_pid_t pid);

/**
 * @brief Unregister a module from a list
 *
 * @param[in] module        module to unregister from
 * @param[in] pid           the callers PID
 *
 * @return                  something even more useful...
 */
int netreg_unregister(netmod_t module, kernel_pid_t pid);

/**
 * @brief   Register an interface with the given interface id (PID)
 *
 * @param pid [description]
 * @return [description]
 */
int netreg_add_interface(kernel_pid_t pid);

/**
 * @brief   Remove the interface with the given interface id (PID)
 *
 * @param[in] pid           interface id
 *
 * @return                  0 on success
 * @return                  -1 if device was not registered
 */
int netreg_remove_interface(kernel_pid_t pid);

/**
 * @brief   Lookup a list of PIDs for a given protocol/interface
 *
 * @param[out] entry        head of the list of entries
 * @param[in]  module       module id to look up
 *
 * @return                  the first PID registered for the requested module
 * @return                  KERNEL_PID_UNDEF if list for module is empty
 */
kernel_pid_t netreg_lookup(netreg_entry_t *entry, netmod_t module);

/**
 * @brief   Get the first element from the list of registered network interfaces
 *
 * @param[in|out] entry     pointer to the current element in the list
 *
 * @return                  the PID of the first network interface
 * @return                  KERNEL_PID_UNDEF if no device is registered
 */
kernel_pid_t netreg_get_interfaces(netreg_entry_t *entry);

/**
 * @brief   Get the next PID in the given list
 *
 * @param[in|out] entry     pointer to the previous entry, will be advanced
 *
 * @return                  the next PID in the list
 * @return                  KERNEL_PID_UNDEF if end of list is reached
 */
kernel_pid_t netreg_getnext(netreg_entry_t *entry);

#ifdef __cplusplus
}
#endif

#endif /* __NETREG_H_ */
/** @} */
