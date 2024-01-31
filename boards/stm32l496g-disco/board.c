/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
