/*
 * Copyright (C) 2020 Bruno Chianca <brunobcf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-itsybitsy-nrf52
 * @{
 *
 * @file
 * @brief       Board initialization for the Adafruit ItsyBitsy nRF52840
 *
 * @author      Bruno Chianca <brunobcf@gmail.com>
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
}
