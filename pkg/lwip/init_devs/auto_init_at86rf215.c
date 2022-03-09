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

#define ENABLE_DEBUG    0
#include "debug.h"

#define USED_BANDS (IS_USED(MODULE_AT86RF215_SUBGHZ) + IS_USED(MODULE_AT86RF215_24GHZ))
#define NETIF_AT86RF215_NUMOF   ARRAY_SIZE(at86rf215_params)

static struct netif netif[NETIF_AT86RF215_NUMOF * USED_BANDS];
static at86rf215_t at86rf215_devs[NETIF_AT86RF215_NUMOF * USED_BANDS];

static void auto_init_at86rf215(void)
{
    unsigned i = 0;
    for (unsigned j = 0; j < NETIF_AT86RF215_NUMOF;j++) {

        at86rf215_t *dev_09 = NULL;
        at86rf215_t *dev_24 = NULL;
        struct netif *netif_09 = NULL;
        struct netif *netif_24 = NULL;

        if (IS_USED(MODULE_AT86RF215_SUBGHZ)) {
            dev_09   = &at86rf215_devs[i];
            netif_09 = &netif[i];
            ++i;
        }

        if (IS_USED(MODULE_AT86RF215_24GHZ)) {
            dev_24   = &at86rf215_devs[i];
            netif_24 = &netif[i];
            ++i;
        }

        at86rf215_setup(dev_09, dev_24, &at86rf215_params[j], j);

        if (dev_09) {
            if (lwip_add_6lowpan(netif_09, &dev_09->netdev.netdev) == NULL) {
                DEBUG("Could not add sub-GHz at86rf215 device #%u\n", j);
            }
        }
        if (dev_24) {
            if (lwip_add_6lowpan(netif_24, &dev_24->netdev.netdev) == NULL) {
                DEBUG("Could not add 2.4-GHz at86rf215 device #%u\n", j);
            }
        }
    }
}

LWIP_INIT_6LOWPAN_NETIF(auto_init_at86rf215);
/** @} */
