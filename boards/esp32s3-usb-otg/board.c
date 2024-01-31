/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
