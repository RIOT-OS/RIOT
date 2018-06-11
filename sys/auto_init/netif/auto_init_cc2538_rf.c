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

#ifdef MODULE_CC2538_RF

#include "log.h"
#include "net/gnrc/netif/ieee802154.h"

#include "cc2538_rf.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define CC2538_MAC_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#ifndef CC2538_MAC_PRIO
#define CC2538_MAC_PRIO            (GNRC_NETIF_PRIO)
#endif

static cc2538_rf_t cc2538_rf_dev;
static char _cc2538_rf_stack[CC2538_MAC_STACKSIZE];

void auto_init_cc2538_rf(void)
{
    LOG_DEBUG("[auto_init_netif] initializing cc2538 radio\n");

    cc2538_setup(&cc2538_rf_dev);
    gnrc_netif_ieee802154_create(_cc2538_rf_stack,
                                 CC2538_MAC_STACKSIZE,
                                 CC2538_MAC_PRIO, "cc2538_rf",
                                 (netdev_t *)&cc2538_rf_dev);
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC2538_RF */
/** @} */
