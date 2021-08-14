/*
￼* Copyright (C) 2017 Neo Nenaco <neo@nenaco.de>
￼*
￼* This file is subject to the terms and conditions of the GNU Lesser
￼* General Public License v2.1. See the file LICENSE in the top level
￼* directory for more details.
￼*
￼*/

/**
 * @ingroup sys_auto_init_lwip_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for MRF24J40 network interfaces
 *
 * @author  Neo Nenaco <neo@nenaco.de>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "mrf24j40.h"
#include "mrf24j40_params.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_MRF24J40_NUMOF        ARRAY_SIZE(mrf24j40_params)

static struct netif netif[NETIF_MRF24J40_NUMOF];
static mrf24j40_t mrf24j40_devs[NETIF_MRF24J40_NUMOF];

void auto_init_mrf24j40(void)
{
    for (unsigned i = 0; i < NETIF_MRF24J40_NUMOF; i++) {
        mrf24j40_setup(&mrf24j40_devs[i], &mrf24j40_params[i], i);
        if (lwip_add_6lowpan(&netif[i], &mrf24j40_devs[i].netdev.netdev) == NULL) {
            DEBUG("Could not add mrf24j40 device\n");
            return;
        }
    }
}
/** @} */
