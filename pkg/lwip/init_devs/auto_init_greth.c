/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_lwip_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for Gaisler GRETH network interfaces
 *
 * @author      Matvii Ivashchenko
 * @}
 */

#include <assert.h>

#include "container.h"
#include "greth.h"
#include "greth_params.h"

#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_GRETH_NUMOF       ARRAY_SIZE(greth_params)

/* The GRETH driver keeps its DMA descriptor rings and ISR device pointer in
 * file-scope state, so it currently supports a single instance only. */
static_assert(NETIF_GRETH_NUMOF == 1, "the greth driver supports only one instance");

static lwip_netif_t netif[NETIF_GRETH_NUMOF];
static greth_t greth_devs[NETIF_GRETH_NUMOF];

static void auto_init_greth(void)
{
    for (unsigned i = 0; i < NETIF_GRETH_NUMOF; i++) {
        greth_setup(&greth_devs[i], &greth_params[i], i);
        if (lwip_add_ethernet(&netif[i], &greth_devs[i].netdev) == NULL) {
            DEBUG("Could not add greth device\n");
            return;
        }
    }
}

LWIP_INIT_ETH_NETIF(auto_init_greth);
/** @} */
