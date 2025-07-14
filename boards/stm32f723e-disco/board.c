/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_stm32f723e-disco
 * @{
 *
 * @file
 * @brief       Board specific initializations for the STM32F723E-DISCO board
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
}
