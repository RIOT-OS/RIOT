/*
 * Copyright (C) 2016 Freie Universität Berlin
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
 */

#ifdef MODULE_CC2420

#include "board.h"
#include "net/gnrc/netdev2.h"
#include "net/gnrc/netdev2/ieee802154.h"
#include "net/gnrc.h"

#include "cc2420.h"
#include "cc2420_params.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   MAC layer stack parameters
 * @{
 */
// #define STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#define STACKSIZE           (THREAD_STACKSIZE_MAIN)
#define PRIO                (THREAD_PRIORITY_MAIN - 4)
/** @} */

/**
 * @brief   Get the number of configured CC2420 devices
 */
#define CC2420_NUMOF        (sizeof(cc2420_params) / sizeof(cc2420_params[0]))

/**
 * @brief   Allocate memory for dev descriptors, stacks, and 802.15.4 adaption
 * @{
 */
static cc2420_t devs[CC2420_NUMOF];
static gnrc_netdev2_t adpt[CC2420_NUMOF];
static char stacks[CC2420_NUMOF][STACKSIZE];
/** @} */

void auto_init_cc2420(void)
{
    for (unsigned i = 0; i < CC2420_NUMOF; i++) {
        DEBUG("Initializing CC2420 radios #%u\n", i);

        cc2420_setup(&devs[i], &cc2420_params[i]);
        int res = gnrc_netdev2_ieee802154_init(&adpt[i],
                                               (netdev2_ieee802154_t *)&devs[i]);

        if (res < 0) {
            DEBUG("Error initializing CC2420 radio device!\n");
        }
        else {
            gnrc_netdev2_init(stacks[i], STACKSIZE, PRIO, "cc2420", &adpt[i]);
        }
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC2420 */

/** @} */
