/*
 * Copyright (C) 2016 MUTEX NZ Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for the cc2538 network interface
 *
 * @author  Aaron Sowry <aaron@mutex.nz>
 */

#ifdef MODULE_CC2538_RF

#include "log.h"
#ifdef MODULE_GNRC_NETIF2
#include "net/gnrc/netif2/ieee802154.h"
#else
#include "net/gnrc/netdev.h"
#include "net/gnrc/netdev/ieee802154.h"
#endif

#include "cc2538_rf.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define CC2538_MAC_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#ifndef CC2538_MAC_PRIO
#ifdef MODULE_GNRC_NETIF2
#define CC2538_MAC_PRIO            (GNRC_NETIF2_PRIO)
#else
#define CC2538_MAC_PRIO            (GNRC_NETDEV_MAC_PRIO)
#endif
#endif

static cc2538_rf_t cc2538_rf_dev;
#ifndef MODULE_GNRC_NETIF2
static gnrc_netdev_t gnrc_adpt;
#endif
static char _cc2538_rf_stack[CC2538_MAC_STACKSIZE];

void auto_init_cc2538_rf(void)
{
    LOG_DEBUG("[auto_init_netif] initializing cc2538 radio\n");

    cc2538_setup(&cc2538_rf_dev);
#ifdef MODULE_GNRC_NETIF2
    if (!gnrc_netif2_ieee802154_create(_cc2538_rf_stack,
                                       CC2538_MAC_STACKSIZE,
                                       CC2538_MAC_PRIO, "cc2538_rf",
                                       (netdev_t *)&cc2538_rf_dev)) {
        LOG_ERROR("[auto_init_netif] error initializing cc2538 radio\n");
    }
#else
    int res = gnrc_netdev_ieee802154_init(&gnrc_adpt,
                                          (netdev_ieee802154_t *)&cc2538_rf_dev);

    if (res < 0) {
        LOG_ERROR("[auto_init_netif] error initializing cc2538 radio\n");
    }
    else {
        gnrc_netdev_init(_cc2538_rf_stack,
                         CC2538_MAC_STACKSIZE,
                         CC2538_MAC_PRIO,
                         "cc2538_rf",
                         &gnrc_adpt);
    }
#endif
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC2538_RF */
/** @} */
