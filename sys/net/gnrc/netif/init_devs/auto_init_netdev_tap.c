/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for ethernet devices
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "log.h"
#include "debug.h"
#include "netdev_tap_params.h"
#include "net/gnrc/netif/ethernet.h"
#include "include/init_devs.h"

#define TAP_MAC_STACKSIZE           (GNRC_NETIF_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#define TAP_MAC_PRIO                (GNRC_NETIF_PRIO)

netdev_tap_t netdev_taps[NETDEV_TAP_MAX];
static char _netdev_eth_stack[NETDEV_TAP_MAX][TAP_MAC_STACKSIZE];

static gnrc_netif_t _netif[NETDEV_TAP_MAX];

void auto_init_netdev_tap(void)
{
    for (unsigned i = 0; i < NETDEV_TAP_MAX; i++) {
        const netdev_tap_params_t *p = &netdev_tap_params[i];

        if (p->tap_name == NULL) {
            continue;
        }

        LOG_DEBUG("[auto_init_netif] initializing netdev_tap #%u on TAP %s\n",
                  i, *(p->tap_name));

        netdev_tap_setup(&netdev_taps[i], p, i);
        gnrc_netif_ethernet_create(&_netif[i], _netdev_eth_stack[i], TAP_MAC_STACKSIZE,
                                   TAP_MAC_PRIO, "gnrc_netdev_tap",
                                   &netdev_taps[i].netdev);
    }
}
/** @} */
