/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    ng_netif    Network interfaces
 * @ingroup     net
 * @brief       Abstraction layer for network interfaces
 *
 * Network interfaces in this context are threads for protocols that are
 * below the network layer.
 *
 * @{
 *
 * @file
 * @brief   Definitions for network interfaces
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_NETIF_H_
#define NG_NETIF_H_

#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of network interfaces
 */
#ifndef NG_NETIF_NUMOF
#define NG_NETIF_NUMOF  (1)
#endif

/**
 * @brief   The add/remove operation to set network layer protocol
 *          specific options for an interface.
 *
 * @param[in] pid   The PID to the new interface.
 */
typedef void (*ng_netif_op_t)(kernel_pid_t pid);

/**
 * @brief   The add and remove handlers to set network layer protocol
 *          specific options for an interface.
 *
 * @details If you implement a pair, please add it to the list in ng_netif.c
 *          statically.
 */
typedef struct {
    ng_netif_op_t add;      /**< The add operation */
    ng_netif_op_t remove;   /**< The remove operation */
} ng_netif_handler_t;

/**
 * @brief   Initializes module.
 */
void ng_netif_init(void);

/**
 * @brief   Adds a thread as interface.
 *
 * @param[in] pid   PID of the added thread.
 *
 * @return  0, on success,
 * @return  -ENOMEM, if maximum number of interfaces has been exceeded.
 */
int ng_netif_add(kernel_pid_t pid);

/**
 * @brief   Removes a thread as interface.
 *
 * @param[in] pid   PID of the removed thread.
 */
void ng_netif_remove(kernel_pid_t pid);

/**
 * @brief   Get all active interfaces.
 *
 * @param[out] size Size of the resulting array.
 *
 * @return  All active interfaces. If @p size is 0 on return, the return value
 *          is undefined. No order must be ensured.
 */
kernel_pid_t *ng_netif_get(size_t *size);

#ifdef __cplusplus
}
#endif

#endif /* NG_NETIF_H_ */
/** @} */
