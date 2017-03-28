/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for ethernet devices
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_NETDEV_TAP

#include "log.h"
#include "debug.h"
#include "netdev_tap_params.h"
#include "net/gnrc/netdev/eth.h"

#define TAP_MAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#define TAP_MAC_PRIO                (THREAD_PRIORITY_MAIN - 3)

static netdev_tap_t netdev_tap[NETDEV_TAP_MAX];
static char _netdev_eth_stack[NETDEV_TAP_MAX][TAP_MAC_STACKSIZE + DEBUG_EXTRA_STACKSIZE];
static gnrc_netdev_t _gnrc_netdev_tap[NETDEV_TAP_MAX];

void auto_init_netdev_tap(void)
{
    for (unsigned i = 0; i < NETDEV_TAP_MAX; i++) {
        const netdev_tap_params_t *p = &netdev_tap_params[i];

        LOG_DEBUG("[auto_init_netif] initializing netdev_tap #%u on TAP %s\n",
                  i, *(p->tap_name));

        netdev_tap_setup(&netdev_tap[i], p);
        gnrc_netdev_eth_init(&_gnrc_netdev_tap[i], (netdev_t*)&netdev_tap[i]);

        gnrc_netdev_init(_netdev_eth_stack[i], TAP_MAC_STACKSIZE,
                         TAP_MAC_PRIO, "gnrc_netdev_tap",
                         &_gnrc_netdev_tap[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_NETDEV_TAP */
/** @} */
