/*
 * Copyright (C) 2020 Locha Inc
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
 * @brief   Auto initialization for the cc26x2/cc13x2 Sub-GHz network interface
 *
 * @author  Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#include "log.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/ieee802154/radio.h"
#include "net/netdev/ieee802154_submac.h"

#include "cc26x2_cc13x2_rf.h"

/**
 * @brief   Define stack parameters for the cc26x2_cc13x2_rf netif thread
 * @{
 */
#define CC26X2_CC13X2_RF_STACKSIZE          (THREAD_STACKSIZE_DEFAULT)
#ifndef CC26X2_CC13X2_RF_PRIO
#define CC26X2_CC13X2_RF_PRIO               (GNRC_NETIF_PRIO)
#endif

static char _stack[CC26X2_CC13X2_RF_STACKSIZE];
static gnrc_netif_t _netif;

static netdev_ieee802154_submac_t cc26x2_cc13x2_rf_netdev;

void auto_init_cc26x2_cc13x2_rf(void)
{
    LOG_DEBUG("[auto_init_netif] initializing cc26x2_cc13x2 radio\n");

    netdev_register(&cc26x2_cc13x2_rf_netdev.dev.netdev, NETDEV_CC26XX_CC13XX, 0);
    netdev_ieee802154_submac_init(&cc26x2_cc13x2_rf_netdev);
    cc26x2_cc13x2_rf_hal_setup(&cc26x2_cc13x2_rf_netdev.submac.dev);

    cc26x2_cc13x2_rf_init();
    gnrc_netif_ieee802154_create(&_netif, _stack,
                                 CC26X2_CC13X2_RF_STACKSIZE,
                                 CC26X2_CC13X2_RF_PRIO, "cc26x2_cc13x2_rf",
                                 &cc26x2_cc13x2_rf_netdev.dev.netdev);
}
/** @} */
