/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
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
 * @brief   Auto initialization for cc2538 network interfaces
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "cc2538_rf.h"
#include "net/netdev/ieee802154_submac.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

static lwip_netif_t netif;
static netdev_ieee802154_submac_t cc2538_rf_netdev;

static void auto_init_cc2538_rf(void)
{
    netdev_register(&cc2538_rf_netdev.dev.netdev, NETDEV_CC2538, 0);
    netdev_ieee802154_submac_init(&cc2538_rf_netdev, NULL);
    cc2538_rf_hal_setup(&cc2538_rf_netdev.submac.dev);

    cc2538_init();

    if (lwip_add_6lowpan(&netif, &cc2538_rf_netdev.dev.netdev) == NULL) {
       DEBUG("Could not add CC2538 device\n");
    }
}

LWIP_INIT_6LOWPAN_NETIF(auto_init_cc2538_rf);
/** @} */
