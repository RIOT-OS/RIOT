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

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_AT86RF2XX_NUMOF        ARRAY_SIZE(at86rf2xx_params)

static struct netif netif[NETIF_AT86RF2XX_NUMOF];
static at86rf2xx_t at86rf2xx_devs[NETIF_AT86RF2XX_NUMOF];

static void auto_init_at86rf2xx(void)
{
    for (unsigned i = 0; i < NETIF_AT86RF2XX_NUMOF; i++) {
        at86rf2xx_setup(&at86rf2xx_devs[i], &at86rf2xx_params[i], i);
        if (lwip_add_6lowpan(&netif[i], &at86rf2xx_devs[i].netdev.netdev) == NULL) {
            DEBUG("Could not add at86rf2xx device\n");
            return;
        }
    }
}

LWIP_INIT_6LOWPAN_NETIF(auto_init_at86rf2xx);
/** @} */
