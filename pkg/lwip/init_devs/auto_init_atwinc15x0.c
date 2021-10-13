/*
 * Copyright (C) Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_auto_init_lwip_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for atwinc15x0 network interfaces
 *
 * @author  Gunar Schorcht <gunar@schorcht.net>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "atwinc15x0.h"
#include "atwinc15x0_params.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_ATWINC_NUMOF ARRAY_SIZE(atwinc15x0_params)

static struct netif netif[NETIF_ATWINC_NUMOF];
static atwinc15x0_t atwinc15x0_devs[NETIF_ATWINC_NUMOF];

static void auto_init_atwinc15x0(void)
{
    for (unsigned i = 0; i < NETIF_ATWINC_NUMOF; i++) {
        atwinc15x0_setup(&atwinc15x0_devs[i], &atwinc15x0_params[i]);
        if (lwip_add_ethernet(&netif[i], &atwinc15x0_devs[i].netdev) == NULL) {
            DEBUG("Could not add atwinc15x0 device\n");
            return;
        }
    }
}

LWIP_INIT_ETH_NETIF(auto_init_atwinc15x0);
/** @} */
