/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_msbiot
 * @{
 *
 * @file
 * @brief       Board specific implementations for the MSB-IoT board
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

static void leds_init(void);

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    leds_init();
}

/**
 * @brief Initialize the boards standard LEDs (RED, YELLOW, GREEN)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LED RED:    PB8
 * - LED YELLOW: PB14
 * - LED GREEN:  PB15
 */
static void leds_init(void)
{
    gpio_init(GPIO_PIN(PORT_B, 8), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_B, 14), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_B, 15), GPIO_OUT);
}
