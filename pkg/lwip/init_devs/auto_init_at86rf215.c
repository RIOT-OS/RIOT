/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
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
 * @brief   Auto initialization for at86rf215 network interfaces
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "at86rf215.h"
#include "at86rf215_params.h"
#include "lwip_init_devs.h"
#include "event/thread.h"
#include "net/netdev/ieee802154_submac.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define USED_BANDS (IS_USED(MODULE_AT86RF215_SUBGHZ) + IS_USED(MODULE_AT86RF215_24GHZ))

#define NETIF_AT86RF215_NUMOF   ARRAY_SIZE(at86rf215_params)

static lwip_netif_t netif[NETIF_AT86RF215_NUMOF * USED_BANDS];
static at86rf215_t at86rf215_devs[NETIF_AT86RF215_NUMOF * USED_BANDS];
static at86rf215_bhp_ev_t at86rf215_bhp[NETIF_AT86RF215_NUMOF];
static netdev_ieee802154_submac_t at86rf215_netdev[NETIF_AT86RF215_NUMOF * USED_BANDS];

static void auto_init_at86rf215(void)
{
    unsigned i = 0;

    for (unsigned j = 0; j < NETIF_AT86RF215_NUMOF; j++) {
        unsigned idx_09 = 0;
        unsigned idx_24 = 0;
        at86rf215_t *dev_09 = NULL;
        at86rf215_t *dev_24 = NULL;
        lwip_netif_t *netif_09 = NULL;
        lwip_netif_t *netif_24 = NULL;
        ieee802154_dev_t *hal_09 = NULL;
        ieee802154_dev_t *hal_24 = NULL;
        netdev_ieee802154_submac_t *netdev_09 = NULL;
        netdev_ieee802154_submac_t *netdev_24 = NULL;

        if (IS_USED(MODULE_AT86RF215_SUBGHZ)) {
            idx_09    = i;
            dev_09    = &at86rf215_devs[i];
            netif_09  = &netif[i];
            netdev_09 = &at86rf215_netdev[i];
            hal_09    = &at86rf215_netdev[i].submac.dev;
            ++i;
        }

        if (IS_USED(MODULE_AT86RF215_24GHZ)) {
            idx_24    = i;
            dev_24    = &at86rf215_devs[i];
            netif_24  = &netif[i];
            netdev_24 = &at86rf215_netdev[i];
            hal_24    = &at86rf215_netdev[i].submac.dev;
            ++i;
        }

        at86rf215_init_event(&at86rf215_bhp[j], hal_09, hal_24, EVENT_PRIO_HIGHEST);
        at86rf215_init(dev_09, dev_24, hal_09, hal_24, &at86rf215_params[j], &at86rf215_bhp[j]);

        if (IS_USED(MODULE_AT86RF215_SUBGHZ)) {
            netdev_register(&netdev_09->dev.netdev, NETDEV_AT86RF215, idx_09);
            netdev_ieee802154_submac_init(netdev_09);
        }

        if (IS_USED(MODULE_AT86RF215_24GHZ)) {
            netdev_register(&netdev_24->dev.netdev, NETDEV_AT86RF215, idx_24);
            netdev_ieee802154_submac_init(netdev_24);
        }

        if (netdev_09 != NULL && lwip_add_6lowpan(netif_09, &netdev_09->dev.netdev) == NULL) {
            DEBUG("Could not add sub-GHz at86rf215 device #%u\n", j);
        }

        if (netdev_24 != NULL && lwip_add_6lowpan(netif_24, &netdev_24->dev.netdev) == NULL) {
            DEBUG("Could not add 2.4-GHz at86rf215 device #%u\n", j);
        }
    }
}

LWIP_INIT_6LOWPAN_NETIF(auto_init_at86rf215);
/** @} */
