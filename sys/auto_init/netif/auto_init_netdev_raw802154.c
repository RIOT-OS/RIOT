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

#ifdef MODULE_NETDEV_RAW802154

#include "log.h"
#include "debug.h"
#include "netdev_raw802154.h"
#include "net/gnrc/netif/ieee802154.h"

extern char *raw802154_ifname;

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define RAW802154_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#ifndef RAW802154_MAC_PRIO
#define RAW802154_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif

static char _raw802154_stack[RAW802154_MAC_STACKSIZE];

void auto_init_netdev_raw802154(void)
{
    LOG_DEBUG("[auto_init_netif] initializing raw802154\n");

    if (!raw802154_ifname)
        return;

    netdev_raw802154_setup(&netdev_raw802154, raw802154_ifname, 1);
#if defined(MODULE_GNRC_GOMACH)
    gnrc_netif_gomach_create(_raw802154_stack,
                             RAW802154_MAC_STACKSIZE,
                             RAW802154_MAC_PRIO, "raw802154-gomach",
                             (netdev_t *)&netdev_raw802154);
#elif defined(MODULE_GNRC_LWMAC)
    gnrc_netif_lwmac_create(_raw802154_stack,
                            RAW802154_MAC_STACKSIZE,
                            RAW802154_MAC_PRIO, "raw802154-lwmac",
                            (netdev_t *)&netdev_raw802154);
#else
    gnrc_netif_ieee802154_create(_raw802154_stack,
                                 RAW802154_MAC_STACKSIZE,
                                 RAW802154_MAC_PRIO, "raw802154",
                                 (netdev_t *)&netdev_raw802154);
#endif
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_NETDEV_RAW802154 */
/** @} */
