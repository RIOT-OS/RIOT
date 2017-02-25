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
 * @ingroup     auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for CC2420 network devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifdef MODULE_CC2420

#include "log.h"
#include "board.h"
#include "net/gnrc/netdev2.h"
#include "net/gnrc/netdev2/ieee802154.h"
#include "net/gnrc.h"

#include "cc2420.h"
#include "cc2420_params.h"

/**
 * @brief   MAC layer stack parameters
 * @{
 */
#define CC2420_MAC_STACKSIZE           (THREAD_STACKSIZE_MAIN)
#ifndef CC2420_MAC_PRIO
#define CC2420_MAC_PRIO                (GNRC_NETDEV2_MAC_PRIO)
#endif
/** @} */

/**
 * @brief   Get the number of configured CC2420 devices
 */
#define CC2420_NUMOF        (sizeof(cc2420_params) / sizeof(cc2420_params[0]))

/**
 * @brief   Allocate memory for dev descriptors, stacks, and 802.15.4 adaption
 * @{
 */
static cc2420_t cc2420_devs[CC2420_NUMOF];
static gnrc_netdev2_t gnrc_adpt[CC2420_NUMOF];
static char _cc2420_stacks[CC2420_NUMOF][CC2420_MAC_STACKSIZE];
/** @} */

void auto_init_cc2420(void)
{
    for (unsigned i = 0; i < CC2420_NUMOF; i++) {
        LOG_DEBUG("[auto_init_netif] initializing cc2420 #%u\n", i);

        cc2420_setup(&cc2420_devs[i], &cc2420_params[i]);
        int res = gnrc_netdev2_ieee802154_init(&gnrc_adpt[i],
                                               (netdev2_ieee802154_t *)&cc2420_devs[i]);

        if (res < 0) {
            LOG_ERROR("[auto_init_netif] error initializing cc2420 #%u\n", i);
        }
        else {
            gnrc_netdev2_init(_cc2420_stacks[i],
                              CC2420_MAC_STACKSIZE,
                              CC2420_MAC_PRIO,
                              "cc2420", &gnrc_adpt[i]);
        }
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC2420 */

/** @} */
