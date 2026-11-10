/*
 * SPDX-FileCopyrightText: 2026 Luis A. Ruiz
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_stm32f769i-disco
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32F769I-DISCO board
 *
 * @author      Luis A. Ruiz <luisan00@hotmail.com>
 *
 * @}
 */

#include "kernel_defines.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* The FT6x06 touch controller shares the DSI panel reset line and stays
     * in reset until it is released, so release it when the touch panel is
     * used without a display driver. */
    if (IS_USED(MODULE_FT5X06)) {
        gpio_init(LCD_RST_PIN, GPIO_OUT);
        gpio_set(LCD_RST_PIN);
    }
}
