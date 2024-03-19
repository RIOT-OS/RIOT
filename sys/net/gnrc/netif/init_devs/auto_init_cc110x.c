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
 * @brief       Auto initialization for cc110x network interfaces
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "cc110x.h"
#include "cc1xxx_common.h"
#include "cc110x_params.h"
#include "log.h"
#include "include/init_devs.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Calculate the stack size for the MAC layer thread(s)
 */
#define CC110X_MAC_STACKSIZE            (GNRC_NETIF_STACKSIZE_DEFAULT + \
                                        DEBUG_EXTRA_STACKSIZE)
#ifndef CC110X_MAC_PRIO
/**
 * @brief   The priority of the MAC layer thread
 */
#define CC110X_MAC_PRIO                 (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Calculate the number of configured CC1100/CC1101 transceivers
 */
#define CC110X_NUM                      ARRAY_SIZE(cc110x_params)

/**
 * @brief   Statically allocate memory for device descriptors
 */
cc110x_t _cc110x_devs[CC110X_NUM];
static gnrc_netif_t _netif[CC110X_NUM];
/**
 * @brief   Statically allocate memory for the MAC layer thread(s)
 */
static char stacks[CC110X_NUM][CC110X_MAC_STACKSIZE];

void auto_init_cc110x(void)
{
    for (unsigned i = 0; i < CC110X_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing cc110x #%u\n", i);

        cc110x_setup(&_cc110x_devs[i], &cc110x_params[i], i);
        gnrc_netif_cc1xxx_create(&_netif[i], stacks[i], CC110X_MAC_STACKSIZE, CC110X_MAC_PRIO,
                                 "cc110x", &_cc110x_devs[i].netdev);
    }
}
/** @} */
