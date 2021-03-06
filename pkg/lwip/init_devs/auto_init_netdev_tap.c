/*
 * Copyright (C) Freie Universität Berlin
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

static struct netif netif[NETIF_TAP_NUMOF];
static netdev_tap_t netdev_taps[NETIF_TAP_NUMOF];

void auto_init_netdev_tap(void)
{
    for (unsigned i = 0; i < NETIF_TAP_NUMOF; i++) {
        netdev_tap_setup(&netdev_taps[i], &netdev_tap_params[i]);
        if (lwip_add_ethernet(&netif[i], &netdev_taps[i].netdev) == NULL) {
            DEBUG("Could not add netdev_tap device\n");
            return;
        }
    }
}
/** @} */
