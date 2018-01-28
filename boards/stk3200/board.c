/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stk3200
 * @{
 *
 * @file
 * @brief       Board specific implementations STK3200 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* enable the board controller, to enable virtual com port */
#if BC_ENABLED
    gpio_init(BC_PIN, GPIO_OUT);
    gpio_set(BC_PIN);
#endif

    /* initialize the LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);

    /* initialize the push buttons */
    gpio_init(PB0_PIN, GPIO_IN);
    gpio_init(PB1_PIN, GPIO_IN);
}
