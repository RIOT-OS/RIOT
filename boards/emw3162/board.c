/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_emw3162
 * @{
 *
 * @file
 * @brief       Board specific implementations for the EMW3162 board
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

static void leds_init(void);

void board_init(void)
{
    /* initialize the boards LEDs, this is done first for debugging purposes */
    leds_init();

    /* initialize the CPU */
    cpu_init();
}

/**
 * @brief Initialize the boards on-board LEDs (D1 and D2)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - D1: PB0
 * - D2: PB1
 */
static void leds_init(void)
{
    gpio_init(LED_GREEN_GPIO, LED_GREEN_MODE);
    gpio_init(LED_RED_GPIO, LED_RED_MODE);
}
