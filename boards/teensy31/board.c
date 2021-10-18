/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_teensy31
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Teensy3.1 & 3.2 boards
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 *
 * @}
 */

#include <stddef.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the boards LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);

}
