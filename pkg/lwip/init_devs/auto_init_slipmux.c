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
 * @brief   Auto initialization for the SLIPMUX module
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "slipmux.h"
#include "slipmux_internal.h"
#include "slipmux_params.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_SLIPDEV_NUMOF ARRAY_SIZE(slipmux_params)

static lwip_netif_t netif[NETIF_SLIPDEV_NUMOF];

static void auto_init_slipdev(void)
{
    for (unsigned i = 0; i < NETIF_SLIPDEV_NUMOF; i++) {
        LWIP_ASSERT("slipmux_init() must run first!", slipmux_devs[i].netdev.driver != NULL);
        if (lwip_add_ethernet(&netif[i], &slipmux_devs[i].netdev) == NULL) {
            DEBUG("Could not add slipdev device\n");
            return;
        }
    }
}

LWIP_INIT_ETH_NETIF(auto_init_slipdev);

/** @} */
