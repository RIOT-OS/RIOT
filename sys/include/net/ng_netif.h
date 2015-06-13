/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_netif    Network interfaces
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

#include <stdlib.h>
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
 * @param[out] netifs   List of all active interfaces. There is no order ensured.
 *                      It must at least fit @ref NG_NETIF_NUMOF elements.
 *
 * @return  The number of active interfaces.
 */
size_t ng_netif_get(kernel_pid_t *netifs);

/**
 * @brief   Converts a hardware address to a human readable string.
 *
 * @details The format will be like `xx:xx:xx:xx` where `xx` are the bytes
 *          of @p addr in hexadecimal representation.
 *
 * @pre @p out_len >= 3 * @p addr_len
 *
 * @param[out] out      A string to store the output in.
 * @param[out] out_len  Length of @p out. Must be at least
 *                      3 * @p addr_len long.
 * @param[in] addr      A hardware address.
 * @param[in] addr_len  Length of @p addr.
 *
 * @return  Copy of @p out on success.
 * @return  NULL, if @p out_len < 3 * @p addr_len.
 */
char *ng_netif_addr_to_str(char *out, size_t out_len, const uint8_t *addr,
                           size_t addr_len);

/**
 * @brief   Parses a string of colon-separated hexadecimals to a hardware
 *          address.
 *
 * @details The input format must be like `xx:xx:xx:xx` where `xx` will be the
 *          bytes of @p addr in hexadecimal representation.
 *
 * @param[out] out      The resulting hardware address.
 * @param[out] out_len  Length of @p out.
 * @param[in] str       A string of colon-separated hexadecimals.
 *
 * @return  Actual length of @p out on success.
 * @return  0, on failure.
 */
size_t ng_netif_addr_from_str(uint8_t *out, size_t out_len, const char *str);

#ifdef __cplusplus
}
#endif

#endif /* NG_NETIF_H_ */
/** @} */
