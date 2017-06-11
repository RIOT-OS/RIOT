/*
 * Copyright (C)  2017 Tom Keddie
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sparkfun-samd-mini
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Sparkfun SAMD Mini board
 *
 * @author      Tom Keddie <github@bronwenandtom.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();
    /* initialize the on-board LED on pin PA17 */
    gpio_init(LED0_PIN, GPIO_OUT);
}
