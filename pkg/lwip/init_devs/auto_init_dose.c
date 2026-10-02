/*
 * SPDX-FileCopyrightText: 2021 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
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

static lwip_netif_t netif[NETIF_DOSE_NUMOF];
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
