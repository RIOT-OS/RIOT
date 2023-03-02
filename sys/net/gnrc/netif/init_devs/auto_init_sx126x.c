/*
 * Copyright (C) 2021 Inria
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
 * @brief       Auto initialization for SX1261/2 LoRa interfaces
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <assert.h>

#include "log.h"
#include "board.h"
#include "net/gnrc/netif/lorawan_base.h"
#include "net/gnrc/netif/raw.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/netdev/ieee802154_submac.h"
#include "net/gnrc.h"
#include "include/init_devs.h"
#include "bhp/event.h"

#include "sx126x.h"
#include "sx126x_params.h"

/**
 * @brief   Calculate the number of configured SX126X devices
 */
#define SX126X_NUMOF                ARRAY_SIZE(sx126x_params)

/**
 * @brief   Define stack parameters for the MAC layer thread
 */
#define SX126X_STACKSIZE            (IEEE802154_STACKSIZE_DEFAULT)
#ifndef SX126X_PRIO
#define SX126X_PRIO                 (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Allocate memory for device descriptors, stacks, and GNRC adaption
 */
static sx126x_t sx126x_devs[SX126X_NUMOF];
static char sx126x_stacks[SX126X_NUMOF][SX126X_STACKSIZE];
static netdev_ieee802154_submac_t sx126x_netdev[SX126X_NUMOF];
static gnrc_netif_t _netif[SX126X_NUMOF];
void auto_init_sx126x(void)
{
    for (unsigned i = 0; i < SX126X_NUMOF; ++i) {
        LOG_DEBUG("[auto_init_netif] initializing sx126x #%u\n", i);
        

        netdev_register(&sx126x_netdev[i].dev.netdev, NETDEV_SX126X, i);
        netdev_ieee802154_submac_init(&sx126x_netdev[i]);
        
        sx126x_hal_setup(&sx126x_devs[i], &sx126x_netdev[i].submac.dev);
        sx126x_init(&sx126x_devs[i], &sx126x_params[i]);
        sx126x_setup(&sx126x_devs[i], i);
        
            gnrc_netif_ieee802154_create(&_netif[i], sx126x_stacks[i],
                                  SX126X_STACKSIZE, SX126X_PRIO,
                                  "sx126x", &sx126x_netdev[i].dev.netdev);

    }
}
/** @} */
