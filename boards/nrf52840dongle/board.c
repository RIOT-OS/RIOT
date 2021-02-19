/*
 * Copyright (C) 2019 Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52840dongle
 * @{
 *
 * @file
 * @brief       Board initialization for the nRF52840-dongle
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the board's single LED */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);

    /* initialize the board's RGB LED */
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_set(LED1_PIN);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_set(LED2_PIN);
    gpio_init(LED3_PIN, GPIO_OUT);
    gpio_set(LED3_PIN);

}
