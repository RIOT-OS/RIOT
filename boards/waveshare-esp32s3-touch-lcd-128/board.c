/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_waveshare-esp32s3-touch-lcd-128
 * @{
 *
 * @file
 * @brief       Board specific initializations for the Waveshare ESP32-S3
 *              1.28-inch round display board (Touch version)
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board.h"

void board_init(void)
{
#if MODULE_GC9A01
    gpio_init(LCD_BACKLIGHT, GPIO_OUT);
#endif
}
