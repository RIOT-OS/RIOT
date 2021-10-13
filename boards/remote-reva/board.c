/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_remote-reva
 * @{
 *
 * @file
 * @brief       Board specific implementations for the RE-Mote revision A board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "fancy_leds.h"

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
 * @brief Initialize the boards on-board LEDs
 *
 * The LED initialization is hard-coded in this function. As the LED (RGB) are
 * soldered onto the board they are fixed to their CPU pins.
 */
static inline void leds_init(void)
{
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);

    /* Shoot rainbows */
    LED_RAINBOW();
}

/**
 * @brief Initialize the Radio interface SW-controlled Switch
 *
 * The RE-Mote features an on-board RF switch to programmatically select to
 * enable either the 2.4GHz or Sub-1GHz RF interface to the RP-SMA connector
 * to use an external antenna.  As default we prefer to use the 2.4GHz RF.
 *
 */
static void rf_switch_init(void)
{
    /* Set RF 2.4GHz as default */
    gpio_init(RF_SWITCH_GPIO, GPIO_OUT);
    RF_SWITCH_2_4_GHZ;
}
