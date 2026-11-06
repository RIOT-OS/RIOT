/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup sys_auto_init_lwip_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for LPC1768 network interfaces
 *
 * @author  Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "lpc1768_eth_netdev.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

extern void lpc1768_eth_netdev_setup(netdev_t *netdev);

static netdev_t _netdev;
static lwip_netif_t _netif;

static void auto_init_lpc1768_eth(void)
{
    lpc1768_eth_netdev_setup(&_netdev);

    if (lwip_add_ethernet(&_netif, &_netdev) == NULL) {
        DEBUG("Could not add lpc1768_eth device\n");
        return;
    }
}

LWIP_INIT_ETH_NETIF(auto_init_lpc1768_eth);
/** @} */
