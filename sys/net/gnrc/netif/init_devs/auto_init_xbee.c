/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for XBee network interfaces
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "log.h"
#include "board.h"
#include "gnrc_netif_xbee.h"
#include "xbee.h"
#include "xbee_params.h"

/**
 * @brief   Calculate the number of configured XBee devices
 */
#define XBEE_NUM        ARRAY_SIZE(xbee_params)

/**
 * @brief   Define stack parameters for the MAC layer thread
 */
#define XBEE_MAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#ifndef XBEE_MAC_PRIO
#define XBEE_MAC_PRIO                (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Allocate memory for device descriptors, stacks, and GNRC adaption
 */
static xbee_t xbee_devs[XBEE_NUM];
static char stacks[XBEE_NUM][XBEE_MAC_STACKSIZE];

static gnrc_netif_t _netif[XBEE_NUM];

void auto_init_xbee(void)
{
    for (unsigned i = 0; i < XBEE_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing xbee #%u\n", i);

        xbee_setup(&xbee_devs[i], &xbee_params[i]);
        gnrc_netif_xbee_create(&_netif[i], stacks[i], XBEE_MAC_STACKSIZE, XBEE_MAC_PRIO,
                               "xbee", (netdev_t *)&xbee_devs[i]);
    }
}
/** @} */
