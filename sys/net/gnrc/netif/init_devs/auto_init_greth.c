/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for Gaisler GRETH Ethernet devices
 *
 * @author      Matvii Ivashchenko
 * @}
 */

#include <assert.h>

#include "container.h"
#include "log.h"
#include "greth.h"
#include "greth_params.h"
#include "net/gnrc/netif/ethernet.h"
#include "include/init_devs.h"

#define GRETH_MAC_STACKSIZE     (GNRC_NETIF_STACKSIZE_DEFAULT)
#ifndef GRETH_MAC_PRIO
#define GRETH_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif

#define GRETH_NUM               ARRAY_SIZE(greth_params)

/* The GRETH driver keeps its DMA descriptor rings and ISR device pointer in
 * file-scope state, so it currently supports a single instance only. */
static_assert(GRETH_NUM == 1, "the greth driver supports only one instance");

static greth_t dev[GRETH_NUM];
static gnrc_netif_t _netif[GRETH_NUM];
static char stack[GRETH_NUM][GRETH_MAC_STACKSIZE];

void auto_init_greth(void)
{
    for (unsigned i = 0; i < GRETH_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing greth #%u\n", i);
        greth_setup(&dev[i], &greth_params[i], i);
        gnrc_netif_ethernet_create(&_netif[i], stack[i], GRETH_MAC_STACKSIZE,
                                   GRETH_MAC_PRIO, "greth",
                                   &dev[i].netdev);
    }
}
/** @} */
