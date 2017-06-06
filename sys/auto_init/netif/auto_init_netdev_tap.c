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
#include "netif_params.h"
#include "net/gnrc/netdev/eth.h"

#define TAP_MAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#define TAP_MAC_PRIO                (THREAD_PRIORITY_MAIN - 3)

static netdev_tap_t netdev_tap[NETDEV_TAP_MAX];
#ifdef MODULE_GNRC
static char _netdev_eth_stack[NETDEV_TAP_MAX][TAP_MAC_STACKSIZE + DEBUG_EXTRA_STACKSIZE];
static gnrc_netdev_t _gnrc_netdev_tap[NETDEV_TAP_MAX];
#endif

void auto_init_netdev_tap(void)
{
    for (unsigned i = 0; i < NETDEV_TAP_MAX; i++) {
        const netdev_tap_params_t *p = &netdev_tap_params[i];
        const netif_params_t *_netif_params = netif_params_get_by_dev(
                                                        netif_params, p);

        LOG_DEBUG("[auto_init_netif] initializing netdev_tap #%s on TAP %s\n",
                  _netif_params->name, *(p->tap_name));

        netdev_tap_setup(&netdev_tap[i], p);
#ifdef MODULE_GNRC
        /* XXX super hacky way to provide the stack without requiring additional
         * fields in gnrc_netdev_t just for initialization */
        _gnrc_netdev_tap[i].dev = (netdev_t *)_netdev_eth_stack[i];
        _gnrc_netdev_tap[i].pid = TAP_MAC_STACKSIZE + DEBUG_EXTRA_STACKSIZE;
#endif
        netif_setup(_netif_params, GNRC_NETDEV_TYPE_ETH,
                    (netdev_t *)&netdev_tap[i], &_gnrc_netdev_tap[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_NETDEV_TAP */
/** @} */
