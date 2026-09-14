/*
 * Copyright (C) Freie Universität Berlin
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
 * @brief   Auto initialization for at86rf2xx network interfaces
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"

#include "lwip_init_devs.h"
#include "net/netdev/ieee802154_submac.h"
#include "event/thread.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_AT86RF2XX_NUMOF        ARRAY_SIZE(at86rf2xx_params)

static lwip_netif_t netif[NETIF_AT86RF2XX_NUMOF];
static at86rf2xx_bhp_ev_t at86rf2xx_bhp[NETIF_AT86RF2XX_NUMOF];
static netdev_ieee802154_submac_t at86rf2xx_netdev[NETIF_AT86RF2XX_NUMOF];

static void auto_init_at86rf2xx(void)
{
    for (unsigned i = 0; i < NETIF_AT86RF2XX_NUMOF; i++) {
        at86rf2xx_init_event(&at86rf2xx_bhp[i], &at86rf2xx_params[i], &at86rf2xx_netdev[i].submac.dev, EVENT_PRIO_HIGHEST);
        netdev_register(&at86rf2xx_netdev[i].dev.netdev, NETDEV_AT86RF2XX, i);
        netdev_ieee802154_submac_init(&at86rf2xx_netdev[i]);
        if (lwip_add_6lowpan(&netif[i], &at86rf2xx_netdev[i].dev.netdev) == NULL) {
            DEBUG("Could not add at86rf2xx device\n");
            return;
        }
    }
}

LWIP_INIT_6LOWPAN_NETIF(auto_init_at86rf2xx);
/** @} */
