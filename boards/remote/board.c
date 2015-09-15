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

static inline void leds_init(void);
static inline void rf_switch_init(void);

void board_init(void)
{
    /* initialize the boards LEDs */
    leds_init();
    /* initialize the CPU */
    cpu_init();
    /* initialize the 2.4GHz RF switch */
    rf_switch_init();
}

/**
 * @brief Initialize the boards on-board LEDs (LD3 and LD4)
 *
 * The LED initialization is hard-coded in this function. As the LED (RGB) are
 * soldered onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LED1: PD2 (red)
 * - LED2: PC3 (blue)
 * - LED3: PD5 (green)
 */
static inline void leds_init(void)
{
    /* set pins to be controlled by software */
    LED_PORT_C->AFSEL &= ~(1 << LED_BLUE_PIN);
    LED_PORT_D->AFSEL &= ~((1 << LED_RED_PIN) | (1 << LED_GREEN_PIN));

    /* configure pins as output */
    LED_PORT_C->DIR |= (1 << LED_BLUE_PIN);
    LED_PORT_D->DIR |= ((1 << LED_RED_PIN) | (1 << LED_GREEN_PIN));

    /* configure io-mux for used pins */
    IOC->PC_OVER[LED_BLUE_PIN] = IOC_OVERRIDE_OE;
    IOC->PD_OVER[LED_GREEN_PIN] = IOC_OVERRIDE_OE;
    IOC->PD_OVER[LED_RED_PIN] = IOC_OVERRIDE_OE;

    /* Shoot rainbows */
    LED_RAINBOW();
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
