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

#define ENABLE_DEBUG (1)
#include "debug.h"

#include "netdev2_tap.h"
#include "net/gnrc/netdev2/eth.h"

extern netdev2_tap_t netdev2_tap;

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define TAP_MAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#define TAP_MAC_PRIO                (THREAD_PRIORITY_MAIN - 3)

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev2_eth_stack[TAP_MAC_STACKSIZE + DEBUG_EXTRA_STACKSIZE];
static gnrc_netdev2_t _gnrc_netdev2_tap;

void auto_init_netdev2_tap(void)
{
    gnrc_netdev2_eth_init(&_gnrc_netdev2_tap, (netdev2_t*)&netdev2_tap);

    gnrc_netdev2_init(_netdev2_eth_stack, TAP_MAC_STACKSIZE,
            TAP_MAC_PRIO, "gnrc_netdev2_tap", &_gnrc_netdev2_tap);
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_NETDEV2_TAP */
/** @} */
