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
#include "net/gnrc/netdev2.h"
#include "net/gnrc/netdev2/ieee802154.h"

#include "cc2538_rf.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define CC2538_MAC_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#ifndef CC2538_MAC_PRIO
#define CC2538_MAC_PRIO            (GNRC_NETDEV2_MAC_PRIO)
#endif

static cc2538_rf_t cc2538_rf_dev;
static gnrc_netdev2_t gnrc_adpt;
static char _cc2538_rf_stack[CC2538_MAC_STACKSIZE];

void auto_init_cc2538_rf(void)
{
    int res;

    LOG_DEBUG("[auto_init_netif] initializing cc2538 radio\n");

    cc2538_setup(&cc2538_rf_dev);
    res = gnrc_netdev2_ieee802154_init(&gnrc_adpt,
                                       (netdev2_ieee802154_t *)&cc2538_rf_dev);

    if (res < 0) {
        LOG_ERROR("[auto_init_netif] error initializing cc2538 radio\n");
    }
    else {
        gnrc_netdev2_init(_cc2538_rf_stack,
                          CC2538_MAC_STACKSIZE,
                          CC2538_MAC_PRIO,
                          "cc2538_rf",
                          &gnrc_adpt);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC2538_RF */
/** @} */
