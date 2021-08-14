/*
 * Copyright (C) Wouter Symons
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_auto_init_lwip_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for stm32_eth network interfaces
 *
 * @author  Wouter Symons <wsymons@nalys-group.com>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "stm32_eth.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

static netdev_t stm32_eth;
extern void stm32_eth_netdev_setup(netdev_t *netdev);

static struct netif netif;

void auto_init_stm32_eth(void)
{
    stm32_eth_netdev_setup(&stm32_eth);
    if (lwip_add_ethernet(&netif, &stm32_eth) == NULL) {
        DEBUG("Could not add stm32_eth device\n");
        return;
    }
}
/** @} */
