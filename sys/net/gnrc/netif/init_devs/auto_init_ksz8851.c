/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for KSZ8851SNL ethernet devices
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "include/init_devs.h"
#include "ksz8851.h"
#include "ksz8851_params.h"
#include "log.h"
#include "net/gnrc/netif/ethernet.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define KSZ8851_MAC_STACKSIZE   (GNRC_NETIF_STACKSIZE_DEFAULT)
#ifndef KSZ8851_MAC_PRIO
#  define KSZ8851_MAC_PRIO        (GNRC_NETIF_PRIO)
#endif
/** @} */

/**
 * @brief   Find out how many of these devices we need to care for
 */
#define KSZ8851_NUM    ARRAY_SIZE(ksz8851_params)

/**
 * @brief   Allocate memory for the device descriptors
 * @{
 */
static ksz8851_t dev[KSZ8851_NUM];
/** @} */

static gnrc_netif_t _netif[KSZ8851_NUM];

/**
 * @brief   Stacks for the MAC layer threads
 */
static char stack[KSZ8851_NUM][KSZ8851_MAC_STACKSIZE];

void auto_init_ksz8851(void)
{
    for (unsigned i = 0; i < KSZ8851_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing ksz8851 #%u\n", i);

        /* setup netdev device */
        ksz8851_setup(&dev[i], &ksz8851_params[i], i);
        gnrc_netif_ethernet_create(&_netif[i], stack[i], KSZ8851_MAC_STACKSIZE, KSZ8851_MAC_PRIO,
                                   "ksz8851", &dev[i].netdev);
    }
}
/** @} */
