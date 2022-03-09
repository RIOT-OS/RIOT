/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for SX1272/SX1276 LoRa interfaces
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <assert.h>

#include "log.h"
#include "board.h"
#include "net/gnrc/netif/lorawan_base.h"
#include "net/gnrc/netif/raw.h"
#include "net/gnrc.h"

#include "sx127x.h"
#include "sx127x_params.h"

/**
 * @brief   Calculate the number of configured SX127x devices
 */
#define SX127X_NUMOF        ARRAY_SIZE(sx127x_params)

/**
 * @brief   Define stack parameters for the MAC layer thread
 */
#define SX127X_STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#ifndef SX127X_PRIO
#define SX127X_PRIO                (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Allocate memory for device descriptors, stacks, and GNRC adaption
 */
static sx127x_t sx127x_devs[SX127X_NUMOF];
static char sx127x_stacks[SX127X_NUMOF][SX127X_STACKSIZE];
static gnrc_netif_t _netif[SX127X_NUMOF];

void auto_init_sx127x(void)
{
    for (unsigned i = 0; i < SX127X_NUMOF; ++i) {
#if defined(MODULE_SX1272)
        LOG_DEBUG("[auto_init_netif] initializing sx1272 #%u\n", i);
#else /* MODULE_SX1276 */
        LOG_DEBUG("[auto_init_netif] initializing sx1276 #%u\n", i);
#endif

        sx127x_setup(&sx127x_devs[i], &sx127x_params[i], i);
        if (IS_USED(MODULE_GNRC_NETIF_LORAWAN)) {
            /* Currently only one lora device is supported */
            assert(SX127X_NUMOF == 1);

            gnrc_netif_lorawan_create(&_netif[i], sx127x_stacks[i],
                                      SX127X_STACKSIZE, SX127X_PRIO,
                                      "sx127x", &sx127x_devs[i].netdev);
        }
        else {
            gnrc_netif_raw_create(&_netif[i], sx127x_stacks[i],
                                  SX127X_STACKSIZE, SX127X_PRIO,
                                  "sx127x", &sx127x_devs[i].netdev);
        }
    }
}
/** @} */
