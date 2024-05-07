/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32s3_wt32_sc01_plus
 * @{
 *
 * @file
 * @brief       Board specific initializations for ESP32-S3 WT32-SC01 Plus
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board.h"

void board_init(void)
{
#if MODULE_ST77XX
    gpio_init(LCD_BACKLIGHT, GPIO_OUT);
#endif
#if MODULE_FT5X06
    gpio_init(FT5X06_PARAM_RST_PIN, GPIO_OUT);
    gpio_set(FT5X06_PARAM_RST_PIN);
#endif
}
