/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_netif    Network interfaces
 * @ingroup     net_gnrc
 * @brief       Abstraction layer for GNRC's network interfaces
 *
 * Network interfaces in the context of GNRC are threads for protocols that are
 * below the network layer.
 *
 * @{
 *
 * @file
 * @brief   Definitions for GNRC's network interfaces
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 */
#ifndef GNRC_NETIF_H_
#define GNRC_NETIF_H_

#include <stdlib.h>
#include <stdbool.h>
#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of network interfaces
 */
#ifndef GNRC_NETIF_NUMOF
#define GNRC_NETIF_NUMOF    (1)
#endif

/**
 * @brief   The add/remove operation to set network layer protocol
 *          specific options for an interface.
 *
 * @param[in] pid   The PID to the new interface.
 */
typedef void (*gnrc_netif_op_t)(kernel_pid_t pid);

/**
 * @brief   The add and remove handlers to set network layer protocol
 *          specific options for an interface.
 *
 * @details If you implement a pair, please add it to the list in gnrc_netif.c
 *          statically.
 */
typedef struct {
    gnrc_netif_op_t add;    /**< The add operation */
    gnrc_netif_op_t remove; /**< The remove operation */
} gnrc_netif_handler_t;

/**
 * @brief   Initializes module.
 */
void gnrc_netif_init(void);

/**
 * @brief   Adds a thread as interface.
 *
 * @param[in] pid   PID of the added thread.
 *
 * @return  0, on success,
 * @return  -ENOMEM, if maximum number of interfaces has been exceeded.
 */
int gnrc_netif_add(kernel_pid_t pid);

/**
 * @brief   Removes a thread as interface.
 *
 * @param[in] pid   PID of the removed thread.
 */
void gnrc_netif_remove(kernel_pid_t pid);

/**
 * @brief   Get all active interfaces.
 *
 * @param[out] netifs   List of all active interfaces. There is no order ensured.
 *                      It must at least fit @ref GNRC_NETIF_NUMOF elements.
 *
 * @return  The number of active interfaces.
 */
size_t gnrc_netif_get(kernel_pid_t *netifs);

/**
 * @brief   Check if an interface exist.
 *
 * @param[in] pid   The PID to be checked.
 *
 * @return  True, if an interface @p pid exists.
 * @return  False, otherwise
 */
bool gnrc_netif_exist(kernel_pid_t pid);

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
char *gnrc_netif_addr_to_str(char *out, size_t out_len, const uint8_t *addr,
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
size_t gnrc_netif_addr_from_str(uint8_t *out, size_t out_len, const char *str);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETIF_H_ */
/** @} */
