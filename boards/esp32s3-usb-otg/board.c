/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_esp32s3_usb_otg
 * @{
 *
 * @file
 * @brief       Board specific initializations for ESP32-S3-USB-OTG boards
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board.h"

void board_init(void)
{
#if MODULE_ST77XX
    gpio_init(LCD_BACKLIGHT, GPIO_OUT);
#endif
}
