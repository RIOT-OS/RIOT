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
 * @brief   Auto initialization for XBee network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_SLIPDEV

#include "log.h"
#include "board.h"
#include "net/gnrc/netdev.h"
#include "net/gnrc/netdev/raw.h"
#include "net/gnrc.h"

#include "slipdev.h"
#include "slipdev_params.h"

#define SLIPDEV_NUM (sizeof(slipdev_params)/sizeof(slipdev_params_t))

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define SLIPDEV_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#ifndef SLIPDEV_PRIO
#define SLIPDEV_PRIO            (GNRC_NETDEV_MAC_PRIO)
#endif

static slipdev_t slipdevs[SLIPDEV_NUM];
static gnrc_netdev_t gnrc_adpt[SLIPDEV_NUM];
static char _slipdev_stacks[SLIPDEV_NUM][SLIPDEV_STACKSIZE];

void auto_init_slipdev(void)
{
    for (unsigned i = 0; i < SLIPDEV_NUM; i++) {
        const slipdev_params_t *p = &slipdev_params[i];

        LOG_DEBUG("[auto_init_netif] initializing slip #%u\n", i);

        slipdev_setup(&slipdevs[i], p);
        kernel_pid_t res = gnrc_netdev_raw_init(&gnrc_adpt[i],
                                                (netdev_t *)&slipdevs[i]);

        if (res < 0) {
            LOG_ERROR("[auto_init_netif] error initializing slipdev #%u\n", i);
        }
        else {
            gnrc_netdev_init(_slipdev_stacks[i], SLIPDEV_STACKSIZE,
                             SLIPDEV_PRIO, "slipdev", &gnrc_adpt[i]);
        }
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_SLIPDEV */
/** @} */
