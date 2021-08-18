/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief   Auto initialization the nRF52840 radio in IEEE802.15.4 mode
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "log.h"
#include "board.h"
#include "nrf802154.h"
#include "net/gnrc/netif/ieee802154.h"
#include "include/init_devs.h"
#include "net/netdev/ieee802154_submac.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#ifndef NRF802154_MAC_STACKSIZE
#define NRF802154_MAC_STACKSIZE     (IEEE802154_STACKSIZE_DEFAULT)
#endif
#ifndef NRF802154_MAC_PRIO
#define NRF802154_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif
/** @} */

static char _stack[NRF802154_MAC_STACKSIZE];
static gnrc_netif_t _netif;

static netdev_ieee802154_submac_t nrf802154_netdev;

void auto_init_nrf802154(void)
{
    LOG_DEBUG("[auto_init_netif] initializing nrf802154\n");

    netdev_register(&nrf802154_netdev.dev.netdev, NETDEV_NRF802154, 0);
    netdev_ieee802154_submac_init(&nrf802154_netdev);
    nrf802154_hal_setup(&nrf802154_netdev.submac.dev);

    nrf802154_init();
    gnrc_netif_ieee802154_create(&_netif, _stack,
                                 NRF802154_MAC_STACKSIZE,
                                 NRF802154_MAC_PRIO, "nrf802154",
                                 &nrf802154_netdev.dev.netdev);
}
/** @} */
