/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_stm32f746g-disco
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32F746G-DISCO board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "kernel_defines.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    gpio_init(BACKLIGHT_PIN, GPIO_OUT);
    if (IS_USED(MODULE_PERIPH_LTDC)) {
        gpio_init(LCD_DISP_PIN, GPIO_OUT);
        gpio_set(LCD_DISP_PIN);
        gpio_set(BACKLIGHT_PIN);
    }
    else {
        gpio_clear(BACKLIGHT_PIN);
    }
}
