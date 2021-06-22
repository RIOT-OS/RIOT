/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for CC2420 network devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#include "log.h"
#include "board.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc.h"
#include "include/init_devs.h"

#include "cc2420.h"
#include "cc2420_params.h"

/**
 * @brief   MAC layer stack parameters
 * @{
 */
#define CC2420_MAC_STACKSIZE           (IEEE802154_STACKSIZE_DEFAULT)
#ifndef CC2420_MAC_PRIO
#define CC2420_MAC_PRIO                (GNRC_NETIF_PRIO)
#endif
/** @} */

/**
 * @brief   Get the number of configured CC2420 devices
 */
#define CC2420_NUMOF        ARRAY_SIZE(cc2420_params)

/**
 * @brief   Allocate memory for dev descriptors, stacks, and 802.15.4 adaption
 * @{
 */
static cc2420_t cc2420_devs[CC2420_NUMOF];
static char _cc2420_stacks[CC2420_NUMOF][CC2420_MAC_STACKSIZE];
static gnrc_netif_t _netif[CC2420_NUMOF];
/** @} */

void auto_init_cc2420(void)
{
    for (unsigned i = 0; i < CC2420_NUMOF; i++) {
        LOG_DEBUG("[auto_init_netif] initializing cc2420 #%u\n", i);

        cc2420_setup(&cc2420_devs[i], &cc2420_params[i], i);
        gnrc_netif_ieee802154_create(&_netif[i], _cc2420_stacks[i], CC2420_MAC_STACKSIZE,
                                     CC2420_MAC_PRIO, "cc2420",
                                     &cc2420_devs[i].netdev.netdev);
    }
}
/** @} */
