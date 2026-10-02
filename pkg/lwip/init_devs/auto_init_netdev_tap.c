/*
 * SPDX-FileCopyrightText: 2021-2022 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup sys_auto_init_lwip_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for TAP network interfaces
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "netdev_tap.h"
#include "netdev_tap_params.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_TAP_NUMOF        (NETDEV_TAP_MAX)

static lwip_netif_t netif[NETIF_TAP_NUMOF];
static netdev_tap_t netdev_taps[NETIF_TAP_NUMOF];

static void auto_init_netdev_tap(void)
{
    for (unsigned i = 0; i < NETIF_TAP_NUMOF; i++) {
        if (netdev_tap_params[i].tap_name == NULL) {
            continue;
        }
        netdev_tap_setup(&netdev_taps[i], &netdev_tap_params[i], i);
        if (lwip_add_ethernet(&netif[i], &netdev_taps[i].netdev) == NULL) {
            DEBUG("Could not add netdev_tap device\n");
            return;
        }
    }
}

LWIP_INIT_ETH_NETIF(auto_init_netdev_tap);
/** @} */
