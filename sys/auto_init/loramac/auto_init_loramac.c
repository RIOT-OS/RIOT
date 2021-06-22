/*
 * Copyright (C) 2019 Inria
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
 * @brief       Auto initialization for Semtech Loramac-node package
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "log.h"
#include "kernel_defines.h"

#if IS_USED(MODULE_SX127X)
#include "sx127x.h"
#include "sx127x_netdev.h"
#include "sx127x_params.h"
#endif

#if IS_USED(MODULE_SX126X)
#include "sx126x.h"
#include "sx126x_netdev.h"
#include "sx126x_params.h"
#endif

#include "semtech_loramac.h"

semtech_loramac_t loramac;

#if IS_USED(MODULE_SX127X)
static sx127x_t sx127x;
#endif

#if IS_USED(MODULE_SX126X)
static sx126x_t sx126x;
#endif

void auto_init_loramac(void)
{
#if IS_USED(MODULE_SX127X)
    sx127x_setup(&sx127x, &sx127x_params[0], 0);
    loramac.netdev = &sx127x.netdev;
    loramac.netdev->driver = &sx127x_driver;
#endif

#if IS_USED(MODULE_SX126X)
    sx126x_setup(&sx126x, &sx126x_params[0], 0);
    loramac.netdev = &sx126x.netdev;
    loramac.netdev->driver = &sx126x_driver;
#endif

    semtech_loramac_init(&loramac);
}
/** @} */
