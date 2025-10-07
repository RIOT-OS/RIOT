/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup sys_auto_init_lwip_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for kw41zrf network interfaces
 *
 * @author  Stepan Konoplev <stepan.konoplev@haw-hamburg.de>
 */

#include "kw41zrf.h"
#include "net/netdev/ieee802154_submac.h"
#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"


static lwip_netif_t netif;
static netdev_ieee802154_submac_t kw41zrf_netdev;

void auto_init_kw41zrf(void)
{
    DEBUG("[auto_init_netif] initializing kw41zrf #%u\n", 0);

    netdev_register(&kw41zrf_netdev.dev.netdev, NETDEV_KW41ZRF, 0);
    netdev_ieee802154_submac_init(&kw41zrf_netdev);

    kw41zrf_hal_setup(&kw41zrf_netdev.submac.dev);
    kw41zrf_init();

    if (lwip_add_6lowpan(&netif, &kw41zrf_netdev.dev.netdev) == NULL) {
        DEBUG("Could not add kw41zrf device\n");
        return;
    }
}

LWIP_INIT_6LOWPAN_NETIF(auto_init_kw41zrf);
/** @} */
