/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_qn9080dk
 * @{
 *
 * @file
 * @brief       Board specific implementations for the QN9080DK base board
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

#include "periph/gpio.h"

void board_init(void)
{
    /* Initialize LEDs and Buttons. */
    gpio_init(LED_RED_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_BLUE_PIN, GPIO_OUT);
    gpio_init(BTN1_PIN, BTN1_MODE);
    gpio_init(BTN2_PIN, BTN2_MODE);

    /* initialize the CPU */
    cpu_init();
}
