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
#include "sx127x.h"
#include "sx127x_netdev.h"
#include "sx127x_params.h"
#include "semtech_loramac.h"

semtech_loramac_t loramac;
static sx127x_t sx127x;

void auto_init_loramac(void)
{
    sx127x_setup(&sx127x, &sx127x_params[0], 0);
    loramac.netdev = (netdev_t *)&sx127x;
    loramac.netdev->driver = &sx127x_driver;
    semtech_loramac_init(&loramac);
}
/** @} */
