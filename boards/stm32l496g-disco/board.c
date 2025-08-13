/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_stm32l496g-disco
 * @{
 *
 * @file
 * @brief       Board specific initializations for the STM32L496G-DISCO board
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board.h"

void board_init(void)
{
#if MODULE_ST77XX
    /* initialize the pin for the HIGH active LCD_BL signal */
    gpio_init(BACKLIGHT_PIN, GPIO_OUT);
#endif
#if MODULE_ST77XX || MODULE_FT5X06
    /* enable VDD_LCD by the LOW active LCD_PWR_ON signal connected to PH0 */
    gpio_init(LCD_DISP_PIN, GPIO_OUT);
    gpio_clear(LCD_DISP_PIN);
#endif
}
