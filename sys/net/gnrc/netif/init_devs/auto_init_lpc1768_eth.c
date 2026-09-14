/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialize LPC1768 ethernet driver
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "log.h"
#include "lpc1768_eth_netdev.h"
#include "include/init_devs.h"
#include "net/gnrc/netif/ethernet.h"

static netdev_t _netdev;

static char _stack[GNRC_NETIF_STACKSIZE_DEFAULT];
static gnrc_netif_t _netif;

void auto_init_lpc1768_eth(void)
{
    LOG_DEBUG("[auto_init_netif] initializing lpc1768_eth\n");

    /* setup netdev device */
    lpc1768_eth_netdev_setup(&_netdev);

    /* initialize netdev <-> gnrc adapter state */
    int res = gnrc_netif_ethernet_create(&_netif, _stack, GNRC_NETIF_STACKSIZE_DEFAULT,
                                         GNRC_NETIF_PRIO, "lpc1768_eth", &_netdev);

    if (res < 0) {
        LOG_ERROR("[auto_init_netif] Failed to create netif for lpc1768_eth device: %d\n", res);
        return;
    }
}
/** @} */
