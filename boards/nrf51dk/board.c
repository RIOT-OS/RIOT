/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf51dk
 * @{
 *
 * @file
 * @brief       Board initialization code for the nRF51DK
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the board LEDs, set pins as output and turn LEDs off */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);
    gpio_set(LED1_PIN);
    gpio_set(LED2_PIN);
    gpio_set(LED3_PIN);

    /* initialize the board buttons */
    gpio_init(BTN0_PIN, BTN0_MODE);
    gpio_init(BTN1_PIN, BTN1_MODE);
    gpio_init(BTN2_PIN, BTN2_MODE);
    gpio_init(BTN3_PIN, BTN3_MODE);

}
