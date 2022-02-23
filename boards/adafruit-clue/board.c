/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-clue
 * @{
 *
 * @file
 * @brief       Board initialization for the Adafruit Clue
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the screen backlight, turn it off by default */
    gpio_init(BACKLIGHT_PIN, GPIO_OUT);
    gpio_clear(BACKLIGHT_PIN);
}
