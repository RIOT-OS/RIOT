/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_elecrow-crowpanel-esp32s3-rotary-128
 * @{
 *
 * @file
 * @brief       Board specific initializations for the Elecrow CrowPanel
 *              ESP32-S3 1.28-inch rotary round display board (Touch version)
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 */

#include "board.h"

void board_init(void)
{
#if MODULE_GC9A01
    gpio_init(LCD_BACKLIGHT, GPIO_OUT);
    gpio_clear(LCD_BACKLIGHT);
#endif

#if MODULE_GC9A01 || MODULE_CST816S
    gpio_init(POWER_LCD_3V3_PIN, GPIO_OUT);
    gpio_set(POWER_LCD_3V3_PIN);
#endif
}
