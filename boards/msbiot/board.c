/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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

static void gpios_init(void);

void board_init(void)
{
    gpios_init();
}

/**
 * @brief Initialize the on board GPIO periphs (3 LEDs, 2 buttons)
 *
 * The LED and button initialization is hard-coded in this function. As the LEDs
 * and buttons are soldered onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LED RED:    PB8
 * - LED YELLOW: PB14
 * - LED GREEN:  PB15
 * - BUTTON T1:  PB13
 * - BUTTON T2:  PA0
 */
static void gpios_init(void)
{
    gpio_init(BUTTON0_PIN, GPIO_IN);
    gpio_init(BUTTON1_PIN, GPIO_IN);
}
