/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr21-xpro
 * @{
 *
 * @file
 * @brief       Network device initialization code
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "auto_init.h"
#include "net/ng_ble.h"
#include "net/ng_nomac.h"
#include "net/ng_netbase.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define MAC_STACKSIZE           (KERNEL_CONF_STACKSIZE_DEFAULT)
#define MAC_PRIO                (PRIORITY_MAIN - 3)
/** @} */

/**
 * @brief   Device descriptor for the Atmel radio
 */
static ng_netdev_t dev;

/**
 * @brief   Stack for the MAC layer thread
 */
static char nomac_stack[MAC_STACKSIZE];

void auto_init_ng_netif(void)
{
    /* initialize the radio */
    DEBUG("Initializing nrf51822 radio\n");
    blemin_init(&dev);

    /* starting NOMAC */
    DEBUG("Starting the MAC layer\n");
    ng_nomac_init(nomac_stack, sizeof(nomac_stack), 5, "nomac", &dev);

    DEBUG("Auto init of on-board radio complete\n");
}
