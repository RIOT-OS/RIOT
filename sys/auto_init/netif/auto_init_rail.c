/*
 * Copyright (C) 2018 Hochschule RheinMain
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


#ifdef MODULE_RAIL

#include <stdint.h>

#define ENABLE_DEBUG  (0)
#include "debug.h"

#include "log.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc.h"

#include "rail_drv.h"
#include "rail_params.h"

#define RAIL_MAC_STACKSIZE      (THREAD_STACKSIZE_DEFAULT)
#ifndef RAIL_MAC_PRIO
#define RAIL_MAC_PRIO           (GNRC_NETIF_PRIO)
#endif

#define RAIL_NUM (sizeof(rail_params) / sizeof(rail_params[0]))

static rail_t rail_devs[RAIL_NUM];
static char _rail_stacks[RAIL_NUM][RAIL_MAC_STACKSIZE];



void auto_init_rail(void)
{
    for (unsigned i = 0; i < RAIL_NUM; i++) {

        LOG_DEBUG("[auto_init_netif] initializing rail #%u\n", i);

        rail_setup(&rail_devs[i], &rail_params[i]);

        /* init ieee802154 layer */
        gnrc_netif_ieee802154_create(_rail_stacks[i],
                                     RAIL_MAC_STACKSIZE,
                                     RAIL_MAC_PRIO,
                                     "rail 802.15.4",
                                     (netdev_t *)&rail_devs[i]);
    }
}

#else
typedef int dont_be_pedantic;

#endif   /* ----- #ifdef MODULE_RAIL_INC  ----- */
