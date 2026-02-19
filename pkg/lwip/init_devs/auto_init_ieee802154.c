/*
 * Copyright (C) 2026 Technische Universität Dresden
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
 * @brief   Auto initialization for all IEEE 802.15.4 network interfaces
 *
 * @author  Lukas Luger <lukas.luger@mailbox.tu-dresden.de>
 */

#include "net/ieee802154/radio.h"
#include "net/ieee802154/init_radio.h"
#include "net/netdev/ieee802154_submac.h"

#include "lwip.h"
#include "lwip_init_devs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

static lwip_netif_t netif[IEEE802154_RADIO_COUNT];
static netdev_ieee802154_submac_t submac_netdev[IEEE802154_RADIO_COUNT];

static void auto_init_ieee802154(void)
{
    (void) netif;
    (void) submac_netdev;
    const netdev_type_t type = netdev_get_default_type();

    for (unsigned i = 0; i < IEEE802154_RADIO_COUNT; i++) {
        ieee802154_radio_init(&submac_netdev[i].submac.dev, i, &lwip_event_queue);

        netdev_register(&submac_netdev[i].dev.netdev, type, i);
        netdev_ieee802154_submac_init(&submac_netdev[i]);

        if (lwip_add_6lowpan(&netif[i], &submac_netdev[i].dev.netdev) == NULL) {
            DEBUG("Could not add  device\n");
            return;
        }
    }
}

LWIP_INIT_6LOWPAN_NETIF(auto_init_ieee802154);
/** @} */
