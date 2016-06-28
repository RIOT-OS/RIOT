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

#ifdef MODULE_NETDEV2_RAW802154

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "netdev2_raw802154.h"
#include "net/gnrc/netdev2/ieee802154.h"

extern netdev2_raw802154_t netdev2_raw802154;
extern char *raw802154_ifname;

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define RAW802154_MAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#define RAW802154_MAC_PRIO                (THREAD_PRIORITY_MAIN - 3)

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev2_raw802154_stack[RAW802154_MAC_STACKSIZE + DEBUG_EXTRA_STACKSIZE];
static gnrc_netdev2_t _gnrc_netdev2_raw802154;

void auto_init_netdev2_raw802154(void)
{
	int res;

    if (!raw802154_ifname) {
        return;
    }

	res = gnrc_netdev2_ieee802154_init(&_gnrc_netdev2_raw802154,
					   (netdev2_ieee802154_t *)&netdev2_raw802154);
	if (res < 0)
		DEBUG("Error initializing raw802154 virtual device!\n");
	else
		gnrc_netdev2_init(_netdev2_raw802154_stack, RAW802154_MAC_STACKSIZE,
				  RAW802154_MAC_PRIO, "gnrc_netdev2_raw802154",
				  &_gnrc_netdev2_raw802154);
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_NETDEV2_RAW802154 */
/** @} */
