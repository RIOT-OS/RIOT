/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_remote
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Re-Mote board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

static inline void rf_switch_init(void);

void board_init(void)
{
    /* initialize the boards LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    /* initialize the CPU */
    cpu_init();
    /* initialize the 2.4GHz RF switch */
    rf_switch_init();
}

/**
 * @brief Initialize the 2.4GHz Radio Frequency SW-controlled Switch
 *
 * The Re-Mote features an on-board RF switch to programatically select to
 * enable either the internal ceramic antenna, or an external antenna over an
 * uFL connector.  As default we prefer to use the internal one.
 *
 */
static void rf_switch_init(void)
{
    /* set pins to be controlled by software */
    RF_SWITCH_PORT->AFSEL &= ~(1 << RF_SWITCH_PIN);

    /* configure pins as output */
    RF_SWITCH_PORT->DIR |= (1 << RF_SWITCH_PIN);

    /* configure io-mux for used pins */
    IOC->PC_OVER[RF_SWITCH_PIN] = IOC_OVERRIDE_OE;

    /* Set to default */
    RF_SWITCH_INTERNAL;
}
