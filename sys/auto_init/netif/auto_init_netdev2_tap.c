/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup auto_init_ng_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for ethernet devices
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_NETDEV2_TAP

#include "log.h"
#include "debug.h"
#include "netdev2_tap_params.h"
#include "net/gnrc/netdev2/eth.h"

#define TAP_MAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#define TAP_MAC_PRIO                (THREAD_PRIORITY_MAIN - 3)

static netdev2_tap_t netdev2_tap[NETDEV2_TAP_MAX];
static char _netdev2_eth_stack[NETDEV2_TAP_MAX][TAP_MAC_STACKSIZE + DEBUG_EXTRA_STACKSIZE];
static gnrc_netdev2_t _gnrc_netdev2_tap[NETDEV2_TAP_MAX];

void auto_init_netdev2_tap(void)
{
    for (unsigned i = 0; i < NETDEV2_TAP_MAX; i++) {
        const netdev2_tap_params_t *p = &netdev2_tap_params[i];

        LOG_DEBUG("[auto_init_netif] initializing netdev2_tap #%u on TAP %s\n",
                  i, *(p->tap_name));

        netdev2_tap_setup(&netdev2_tap[i], p);
        gnrc_netdev2_eth_init(&_gnrc_netdev2_tap[i], (netdev2_t*)&netdev2_tap[i]);

        gnrc_netdev2_init(_netdev2_eth_stack[i], TAP_MAC_STACKSIZE,
                          TAP_MAC_PRIO, "gnrc_netdev2_tap",
                          &_gnrc_netdev2_tap[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_NETDEV2_TAP */
/** @} */
