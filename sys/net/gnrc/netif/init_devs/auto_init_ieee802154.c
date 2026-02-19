/*
 * Copyright (C) 2026 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for IEEE 802.15.4 network interfaces
 *
 * @author  Lukas Luger <lukas.luger@mailbox.tu-dresden.de>
 */

#include "log.h"
#include "net/gnrc/netif/ieee802154.h"
#include "include/init_devs.h"

#include "net/ieee802154/init_radio.h"
#include "net/netdev/ieee802154_submac.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define IEEE802154_MAC_STACKSIZE       (IEEE802154_STACKSIZE_DEFAULT)
#ifndef IEEE802154_MAC_PRIO
#define IEEE802154_MAC_PRIO            (GNRC_NETIF_PRIO)
#endif

static netdev_ieee802154_submac_t _rf_netdev[IEEE802154_RADIO_COUNT];
static char _rf_stacks[IEEE802154_RADIO_COUNT][IEEE802154_MAC_STACKSIZE];
static gnrc_netif_t _netif[IEEE802154_RADIO_COUNT];


void auto_init_ieee802154(void)
{
    LOG_DEBUG("[auto_init_netif] initializing ieee802154 radio\n");

#if IS_USED(MODULE_OPENDSME)
    ieee802154_radio_init(&_rf_netdev[0].submac.dev, 0, NULL);
    gnrc_netif_opendsme_create(&_netif[0], _rf_stacks[0],
                                IEEE802154_MAC_STACKSIZE,
                                IEEE802154_MAC_PRIO, "ieee802154",
                                (netdev_t*) &_rf_netdev[0].submac.dev);
#else
    const netdev_type_t type = netdev_get_default_type();
    for (unsigned i = 0; i < IEEE802154_RADIO_COUNT; i++) {
        netdev_register(&_rf_netdev[i].dev.netdev, type, 0);
        netdev_ieee802154_submac_init(&_rf_netdev[i]);
        ieee802154_radio_init(&_rf_netdev[i].submac.dev, i, &_netif[i].evq[GNRC_NETIF_EVQ_INDEX_PRIO_HIGH]);

        gnrc_netif_ieee802154_create(&_netif[i], _rf_stacks[i],
                                    IEEE802154_MAC_STACKSIZE,
                                    IEEE802154_MAC_PRIO, "ieee802154",
                                    &_rf_netdev[i].dev.netdev);
    }
#endif
}
/** @} */
