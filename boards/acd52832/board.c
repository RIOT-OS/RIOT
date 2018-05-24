/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_acd52832
 * @{
 *
 * @file
 * @brief       Board initialization for the ACD52832
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the boards LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);
}
