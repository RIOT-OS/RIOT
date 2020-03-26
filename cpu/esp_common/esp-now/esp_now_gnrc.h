/*
 * Copyright (C) 2018 Timo Rothenpieler
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common_esp_now
 * @{
 *
 * @file
 * @brief       ESP-NOW adaption for @ref net_gnrc_netif
 *
 * @author      Timo Rothenpieler <timo.rothenpieler@uni-bremen.de>
 */
#ifndef ESP_NOW_GNRC_H
#define ESP_NOW_GNRC_H

#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Creates the ESP-NOW network interface
 * @see     gnrc_netif_create
 * @param   [out]   netif       The interface. May not be `NULL`.
 * @param   [in]    stack       The stack for the network interface's thread.
 * @param   [in]    stacksize   Size of stack.
 * @param   [in]    priority    Priority for the network interface's thread.
 * @param   [in]    name        Name for the network interface. May be NULL.
 * @param   [in]    dev         Device for the interface.
 *
 * @return  0 on success
 * @return  negative number on error
 */
int gnrc_netif_esp_now_create(gnrc_netif_t *netif, char *stack, int stacksize, char priority,
                              char *name, netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* ESP_NOW_GNRC_H */
/** @} */
