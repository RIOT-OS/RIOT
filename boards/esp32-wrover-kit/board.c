/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
