/*
 * SPDX-FileCopyrightText: 2021 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup sys_auto_init_lwip_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for sam0_eth network interfaces
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "sam0_eth_netdev.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

static netdev_t sam0_eth;
extern void sam0_eth_setup(netdev_t *netdev);

static lwip_netif_t netif;

static void auto_init_sam0_eth(void)
{
    sam0_eth_setup(&sam0_eth);
    if (lwip_add_ethernet(&netif, &sam0_eth) == NULL) {
        DEBUG("Could not add sam0_eth device\n");
        return;
    }
}

LWIP_INIT_ETH_NETIF(auto_init_sam0_eth);
/** @} */
