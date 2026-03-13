/*
 * Copyright (C) 2017 Neo Nenaco <neo@nenaco.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

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

#include "bhp/event.h"
#include "lwip.h"
#include "lwip_init_devs.h"
#include "net/netdev/ieee802154_submac.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_MRF24J40_NUMOF        ARRAY_SIZE(mrf24j40_params)

static lwip_netif_t netif[NETIF_MRF24J40_NUMOF];
static mrf24j40_t mrf24j40_devs[NETIF_MRF24J40_NUMOF];
static netdev_ieee802154_submac_t mrf24j40_netdev[NETIF_MRF24J40_NUMOF];

static void auto_init_mrf24j40(void)
{
    for (unsigned i = 0; i < NETIF_MRF24J40_NUMOF; i++) {
        bhp_event_init(&netif[i].bhp, &lwip_event_queue, &mrf24j40_radio_irq_handler,
                       &mrf24j40_netdev[i].submac.dev);
        mrf24j40_init(&mrf24j40_devs[i], &mrf24j40_params[i], &mrf24j40_netdev[i].submac.dev,
                      bhp_event_isr_cb, &netif[i].bhp);


        netdev_register(&mrf24j40_netdev[i].dev.netdev, NETDEV_MRF24J40, i);
        netdev_ieee802154_submac_init(&mrf24j40_netdev[i], NULL);

        if (lwip_add_6lowpan(&netif[i], &mrf24j40_netdev[i].dev.netdev) == NULL) {
            DEBUG("Could not add mrf24j40 device\n");
            return;
        }
    }
}

LWIP_INIT_6LOWPAN_NETIF(auto_init_mrf24j40);
/** @} */
