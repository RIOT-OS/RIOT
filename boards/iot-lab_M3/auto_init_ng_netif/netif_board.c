/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_iot-lab_M3
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
#include "ng_at86rf2xx.h"
#include "net/ng_nomac.h"
#include "net/ng_netbase.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define MAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#define MAC_PRIO                (THREAD_PRIORITY_MAIN - 3)
/** @} */

/**
 * @brief   Device descriptor for the Atmel radio
 */
static ng_at86rf2xx_t radio;

/**
 * @brief   Stack for the MAC layer thread
 */
static char nomac_stack[MAC_STACKSIZE];


void auto_init_ng_netif(void)
{
    /* initialize the radio */
    DEBUG("Initializing AT86RF231 radio\n");
    ng_at86rf2xx_init(&radio, AT86RF231_SPI, AT86RF231_SPI_CLK,
                      AT86RF231_CS, AT86RF231_INT,
                      AT86RF231_SLEEP, AT86RF231_RESET);
    /* starting NOMAC */
    DEBUG("Starting the MAC layer\n");
    ng_nomac_init(nomac_stack, sizeof(nomac_stack), MAC_PRIO, "at86rf233",
                  (ng_netdev_t *)(&radio));
    DEBUG("Auto init of on-board radio complete\n");
}
