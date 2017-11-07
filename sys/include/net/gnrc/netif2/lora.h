/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_netif2
 *
 * @file
 * @brief       GNRC to LoRa netdev glue code interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef NET_GNRC_NETIF2_LORA_H
#define NET_GNRC_NETIF2_LORA_H

#include "net/gnrc/netdev.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Creates a LoRa network interface
 *
 * @param[in] stack     The stack for the network interface's thread.
 * @param[in] stacksize Size of @p stack.
 * @param[in] priority  Priority for the network interface's thread.
 * @param[in] name      Name for the network interface. May be NULL.
 * @param[in] dev       Device for the interface
 *
 * @see @ref gnrc_netif2_create()
 *
 * @return  The network interface on success.
 * @return  NULL, on error.
 */
gnrc_netif2_t *gnrc_netif2_lora_create(char *stack, int stacksize,
                                       char priority, char *name,
                                       netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF2_LORA_H */
/** @} */