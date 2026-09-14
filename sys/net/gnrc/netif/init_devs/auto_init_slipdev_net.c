/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for XBee network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "log.h"
#include "board.h"
#include "net/gnrc/netif/raw.h"
#include "net/gnrc.h"
#include "include/init_devs.h"

#include "slipdev.h"
#include "slipdev_params.h"

#define SLIPDEV_NUM ARRAY_SIZE(slipdev_params)

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define SLIPDEV_STACKSIZE       (GNRC_NETIF_STACKSIZE_DEFAULT)
#ifndef SLIPDEV_PRIO
#define SLIPDEV_PRIO            (GNRC_NETIF_PRIO)
#endif

static slipdev_t slipdevs[SLIPDEV_NUM];
static char _slipdev_stacks[SLIPDEV_NUM][SLIPDEV_STACKSIZE];

static gnrc_netif_t _netif[SLIPDEV_NUM];

void auto_init_slipdev(void)
{
    for (unsigned i = 0; i < SLIPDEV_NUM; i++) {
        const slipdev_params_t *p = &slipdev_params[i];

        LOG_DEBUG("[auto_init_netif] initializing slip #%u\n", i);

        slipdev_setup(&slipdevs[i], p, i);
        gnrc_netif_raw_create(&_netif[i], _slipdev_stacks[i], SLIPDEV_STACKSIZE,
                              SLIPDEV_PRIO, "slipdev",
                              &slipdevs[i].netdev);
    }
}
/** @} */
