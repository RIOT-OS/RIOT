/*
 * Copyright (C)  2017 Thomas Perrot <thomas.perrot@tupi.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-leonardo
 * @{
 *
 * @file
 * @brief       Board specific led initialization for Arduino Leonardo
 *
 * @author      Thomas Perrot <thomas.perrot@tupi.fr>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

void led_init(void)
{
    /* initialize the on-board LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    LED0_OFF;
    gpio_init(LED1_PIN, GPIO_OUT);
    LED2_OFF;
    gpio_init(LED2_PIN, GPIO_OUT);
    LED2_OFF;
}
