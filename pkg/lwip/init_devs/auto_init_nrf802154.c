/*
 * Copyright (C) 2021 Inria
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
 * @brief   Auto initialization for NRF802154 network interfaces
 *
 * @author  Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "nrf802154.h"
#include "net/netdev/ieee802154_submac.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

static lwip_netif_t netif;
static netdev_ieee802154_submac_t nrf802154_netdev;

static void auto_init_nrf802154(void)
{
    netdev_register(&nrf802154_netdev.dev.netdev, NETDEV_NRF802154, 0);
    netdev_ieee802154_submac_init(&nrf802154_netdev, NULL);

    nrf802154_hal_setup(&nrf802154_netdev.submac.dev);
    nrf802154_init();
    if (lwip_add_6lowpan(&netif, &nrf802154_netdev.dev.netdev) == NULL) {
        DEBUG("Could not add nrf802154 device\n");
        return;
    }
}

LWIP_INIT_6LOWPAN_NETIF(auto_init_nrf802154);
/** @} */
