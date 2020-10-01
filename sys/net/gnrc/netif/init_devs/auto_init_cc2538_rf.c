/*
 * Copyright (C) 2016 MUTEX NZ Ltd
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
 * @brief   Auto initialization for the cc2538 network interface
 *
 * @author  Aaron Sowry <aaron@mutex.nz>
 */

#include "log.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/ieee802154/radio.h"
#include "net/netdev/ieee802154_submac.h"

#include "cc2538_rf.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define CC2538_MAC_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#ifndef CC2538_MAC_PRIO
#define CC2538_MAC_PRIO            (GNRC_NETIF_PRIO)
#endif

#if IS_USED(MODULE_IEEE802154_RADIO_HAL)
extern ieee802154_dev_t cc2538_rf_dev;
static netdev_ieee802154_submac_t cc2538_rf_submac;
#else
static cc2538_rf_t cc2538_rf_dev;
#endif
static char _cc2538_rf_stack[CC2538_MAC_STACKSIZE];
static gnrc_netif_t _netif;

void auto_init_cc2538_rf(void)
{
    LOG_DEBUG("[auto_init_netif] initializing cc2538 radio\n");

    netdev_t *netdev;
#if IS_USED(MODULE_IEEE802154_RADIO_HAL)
    netdev_ieee802154_submac_init(&cc2538_rf_submac, &cc2538_rf_dev);
    netdev = (netdev_t*) &cc2538_rf_submac;
    cc2538_init();
#else
    netdev = &cc2538_rf_dev.netdev.netdev;
    cc2538_setup(&cc2538_rf_dev);
#endif
    gnrc_netif_ieee802154_create(&_netif, _cc2538_rf_stack,
                                 CC2538_MAC_STACKSIZE,
                                 CC2538_MAC_PRIO, "cc2538_rf",
                                 netdev);
}
/** @} */
