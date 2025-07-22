/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_esp32_esp-wrover-kit
 * @{
 *
 * @file
 * @brief       Board specific definitions for esp32-wrover-kit
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board.h"

void board_init(void)
{
#if MODULE_ILI9341
    gpio_init(LCD_BACKLIGHT, GPIO_OUT);
#endif
}
