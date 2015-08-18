/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup auto_init_ng_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for netdev Ethernet network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifdef MODULE_NG_NETDEV_ETH

#include "board.h"
#include "net/ng_nomac.h"
#include "net/gnrc.h"

#include "net/ng_netdev_eth.h"
#include "net/dev_eth.h"
#include "dev_eth_tap.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define NETDEV_ETH_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#define NETDEV_ETH_MAC_PRIO          (THREAD_PRIORITY_MAIN - 4)

static char _nomac_stack[NETDEV_ETH_MAC_STACKSIZE];

void auto_init_ng_netdev_eth(void)
{
    DEBUG("Initializing NETDEV_ETH device\n");
    int res = ng_netdev_eth_init(&ng_netdev_eth, (dev_eth_t*)&dev_eth_tap);

    if (res < 0) {
        DEBUG("Error initializing NETDEV_ETH device!");
    }
    else {
        ng_nomac_init(_nomac_stack, NETDEV_ETH_MAC_STACKSIZE, NETDEV_ETH_MAC_PRIO,
                "tapnet", (ng_netdev_t *)&ng_netdev_eth);
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_NG_NETDEV_ETH */

/** @} */
