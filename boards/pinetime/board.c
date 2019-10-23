/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2020 Inria
 *               2020 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pinetime
 * @{
 *
 * @file
 * @brief       Board initialization for the PineTime
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
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

    /* initialize pins */
    gpio_init(VCC33, GPIO_OUT);
    gpio_init(BUTTON0_ENABLE, GPIO_OUT);
    gpio_init(BUTTON0, GPIO_IN);
    gpio_init(VIBRATOR, GPIO_OUT);
    gpio_init(LCD_BACKLIGHT_LOW, GPIO_OUT);
    gpio_init(LCD_BACKLIGHT_MID, GPIO_OUT);
    gpio_init(LCD_BACKLIGHT_HIGH, GPIO_OUT);

    gpio_set(VCC33);
    gpio_set(BUTTON0_ENABLE);
    gpio_set(VIBRATOR);
    gpio_set(LCD_BACKLIGHT_LOW);
    gpio_set(LCD_BACKLIGHT_MID);
    gpio_set(LCD_BACKLIGHT_HIGH);
}
