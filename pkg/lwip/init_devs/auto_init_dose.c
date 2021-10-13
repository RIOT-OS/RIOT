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
 * @brief   Auto initialization for DOSE network interfaces
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "dose.h"
#include "dose_params.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_DOSE_NUMOF    ARRAY_SIZE(dose_params)

static struct netif netif[NETIF_DOSE_NUMOF];
static dose_t dose_devs[NETIF_DOSE_NUMOF];

static void auto_init_dose(void)
{
    for (unsigned i = 0; i < NETIF_DOSE_NUMOF; i++) {
        dose_setup(&dose_devs[i], &dose_params[i], i);
        if (lwip_add_ethernet(&netif[i], &dose_devs[i].netdev) == NULL) {
            DEBUG("Could not add DOSE device #%u\n", i);
        }
    }
}

LWIP_INIT_ETH_NETIF(auto_init_dose);
/** @} */
