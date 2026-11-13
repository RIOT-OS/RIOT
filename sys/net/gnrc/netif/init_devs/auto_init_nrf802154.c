/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization the nRF52840 radio in IEEE802.15.4 mode
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "kernel_defines.h"
#include "log.h"
#include "board.h"
#include "nrf802154.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc/netif/ieee802154_mac.h"
#include "include/init_devs.h"
#include "net/netdev/ieee802154_submac.h"

#if IS_USED(MODULE_OPENDSME)
#include "opendsme/opendsme.h"
#endif

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

#if IS_USED(MODULE_GNRC_NETIF_IEEE802154_MAC)
static gnrc_netif_ieee802154_mac_dev_t _dev;

static int _radio_init_cb(ieee802154_dev_t *radio, ieee802154_dev_type_t dev_type,
                          unsigned idx, void *arg)
{
    (void)dev_type;
    (void)arg;
    if (idx > 0) {
        return -EINVAL;
    }

    nrf802154_hal_setup(radio);
    return nrf802154_init();
}
#else
static netdev_ieee802154_submac_t nrf802154_netdev;
#endif

void auto_init_nrf802154(void)
{
    LOG_DEBUG("[auto_init_netif] initializing nrf802154\n");

#if IS_USED(MODULE_GNRC_NETIF_IEEE802154_MAC)
    gnrc_netif_ieee802154_mac_set_dev_type(IEEE802154_DEV_TYPE_NRF802154);
    gnrc_netif_ieee802154_mac_set_radio_init_cb(_radio_init_cb, NULL);

    gnrc_netif_ieee802154_mac_create(&_netif, _stack, NRF802154_MAC_STACKSIZE,
                                     NRF802154_MAC_PRIO, "nrf802154", &_dev);
#else
    netdev_register(&nrf802154_netdev.dev.netdev, NETDEV_NRF802154, 0);

    nrf802154_init();
#if IS_USED(MODULE_OPENDSME)
    nrf802154_hal_setup(&nrf802154_netdev.submac.dev);
    /* NOTE: This casts a Radio HAL descriptor to a netdev and should be
     * addressed as soon as the GNRC<->netdev dependency is removed.
     */
    gnrc_netif_opendsme_create(&_netif, _stack,
                               NRF802154_MAC_STACKSIZE,
                               NRF802154_MAC_PRIO, "nrf802154",
                               (netdev_t *) &nrf802154_netdev.submac.dev);
#else
    netdev_ieee802154_submac_init(&nrf802154_netdev);
    nrf802154_hal_setup(&nrf802154_netdev.submac.dev);
    gnrc_netif_ieee802154_create(&_netif, _stack,
                                 NRF802154_MAC_STACKSIZE,
                                 NRF802154_MAC_PRIO, "nrf802154",
                                 &nrf802154_netdev.dev.netdev);
#endif
#endif
}
/** @} */
