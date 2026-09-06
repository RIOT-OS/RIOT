/*
 * Copyright (C) 2022 HAW Hamburg
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
 * @brief   Auto initialization for KW2XRF network interfaces
 *
 * @author  José I. Álamos <jose.alamos@haw-hamburg.de>
 */

#include "kw2xrf.h"
#include "kw2xrf_params.h"

#include "lwip.h"
#include "lwip_init_devs.h"

#include "bhp/event.h"
#include "net/netdev/ieee802154_submac.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_KW2XRF_NUMOF        ARRAY_SIZE(kw2xrf_params)

static lwip_netif_t netif[NETIF_KW2XRF_NUMOF];
static kw2xrf_t kw2xrf_devs[NETIF_KW2XRF_NUMOF];
static netdev_ieee802154_submac_t kw2xrf_netdev[NETIF_KW2XRF_NUMOF];

static void auto_init_kw2xrf(void)
{
    for (unsigned i = 0; i < NETIF_KW2XRF_NUMOF; i++) {
        bhp_event_init(&netif[i].bhp, &lwip_event_queue,
                       &kw2xrf_radio_hal_irq_handler, &kw2xrf_netdev[i].submac.dev);
        kw2xrf_init(&kw2xrf_devs[i], &kw2xrf_params[i], &kw2xrf_netdev[i].submac.dev,
                    bhp_event_isr_cb, &netif[i].bhp);

        netdev_register(&kw2xrf_netdev[i].dev.netdev, NETDEV_KW2XRF, i);
        netdev_ieee802154_submac_init(&kw2xrf_netdev[i], NULL);

        if (lwip_add_6lowpan(&netif[i], &kw2xrf_netdev[i].dev.netdev) == NULL) {
            DEBUG("Could not add kw2xrf device\n");
            return;
        }
    }
}

LWIP_INIT_6LOWPAN_NETIF(auto_init_kw2xrf);
/** @} */
