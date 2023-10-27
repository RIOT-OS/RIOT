/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
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
 * @brief   Auto initialization for the SLIP module
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "slipdev.h"
#include "slipdev_params.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_SLIPDEV_NUMOF ARRAY_SIZE(slipdev_params)

static lwip_netif_t netif[NETIF_SLIPDEV_NUMOF];
static slipdev_t slipdev_devs[NETIF_SLIPDEV_NUMOF];

static void auto_init_slipdev(void)
{
    for (unsigned i = 0; i < NETIF_SLIPDEV_NUMOF; i++) {
        slipdev_setup(&slipdev_devs[i], &slipdev_params[i], i);
        if (lwip_add_ethernet(&netif[i], &slipdev_devs[i].netdev) == NULL) {
            DEBUG("Could not add slipdev device\n");
            return;
        }
    }
}

LWIP_INIT_ETH_NETIF(auto_init_slipdev);
/** @} */
