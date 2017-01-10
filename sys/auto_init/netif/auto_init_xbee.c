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
 * @ingroup     auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for XBee network interfaces
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef MODULE_XBEE

#include "board.h"
#include "net/gnrc/netdev2/xbee_adpt.h"
#include "xbee_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Calculate the number of configured XBee devices
 */
#define XBEE_NUM        (sizeof(xbee_params) / sizeof(xbee_params_t))

/**
 * @brief   Define stack parameters for the MAC layer thread
 */
#define XBEE_MAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#ifndef XBEE_MAC_PRIO
#define XBEE_MAC_PRIO                (GNRC_NETDEV2_MAC_PRIO)
#endif

/**
 * @brief   Allocate memory for device descriptors, stacks, and GNRC adaption
 */
static xbee_t xbee_devs[XBEE_NUM];
static gnrc_netdev2_t gnrc_adpt[XBEE_NUM];
static char stacks[XBEE_MAC_STACKSIZE][XBEE_NUM];

void auto_init_xbee(void)
{
    for (size_t i = 0; i < XBEE_NUM; i++) {
        DEBUG("Initializing XBee radio #%u\n", i);

        xbee_setup(&xbee_devs[i], &xbee_params[i]);
        gnrc_netdev2_xbee_init(&gnrc_adpt[i], &xbee_devs[i]);
        gnrc_netdev2_init(stacks[i], XBEE_MAC_STACKSIZE, XBEE_MAC_PRIO,
                              "xbee", &gnrc_adpt[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_XBEE */
/** @} */
