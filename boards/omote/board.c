/*
 *Copyright (C) 2020 Oppila Microsystems - http://www.oppila.in
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_OMote
 * @{
 *
 * @file
 * @brief       Board specific implementations for the OMote board
 *
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

    /* The boot pin must be set to input otherwise it may lock the bootloader */
    gpio_init(BOOT_PIN, GPIO_IN);

    gpio_init(BTN0_PIN, BTN0_MODE);
}

/**
 * @brief Initialize the boards on-board LEDs
 *
 * The LED initialization is hard-coded in this function. As the LEDs are
 * soldered onto the board they are fixed to their CPU pins.
 */
static inline void leds_init(void)
{
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
}

/**
 * @brief Initialize the Radio interface SW-controlled Switch
 *
 * The OMote features an on-board RF switch.
 *
 */
static void rf_switch_init(void)
{
    /* Set RF 2.4GHz as default */
    gpio_init(RF_SWITCH_GPIO, GPIO_OUT);
    RF_SWITCH_2_4_GHZ;
}
