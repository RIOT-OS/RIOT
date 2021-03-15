/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for lcd tft displays
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include "timex.h"
#include "ztimer.h"
#include "board.h"
#include "lcd.h"

#include "riot_logo.h"

#if IS_USED(MODULE_ST7735)
#include "st7735.h"
#include "st7735_params.h"
#elif IS_USED(MODULE_ILI9341)
#include "ili9341.h"
#include "ili9341_params.h"
#else
#error "Include either module 'st7735' or 'ili9341'"
#endif

int main(void)
{
    lcd_t dev;

#if IS_USED(MODULE_ST7735)
    dev.driver = &lcd_st7735_driver;
#elif IS_USED(MODULE_ILI9341)
    dev.driver = &lcd_ili9341_driver;
#endif

    puts("lcd TFT display test application");

    /* initialize the sensor */
    printf("Initializing display...");

    /* Enable backlight if macro is defined */
#ifdef BACKLIGHT_ON
    BACKLIGHT_ON;
#endif

#if IS_USED(MODULE_ST7735)
    if (lcd_init(&dev, (lcd_params_t *)&st7735_params[0].params) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return 1;
    }
#endif
#if IS_USED(MODULE_ILI9341)
    if (lcd_init(&dev, &ili9341_params[0]) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return 1;
    }
#endif

    puts("lcd TFT display filling map");
    lcd_fill(&dev, 0, dev.params->lines, 0, dev.params->rgb_channels, 0x0000);
    puts("lcd TFT display map filled");

    /* Fill square with blue */
    puts("Drawing blue rectangle");
    lcd_fill(&dev, 0, dev.params->lines / 3, 0, dev.params->rgb_channels, 0x001F);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    puts("Drawing green rectangle");
    lcd_fill(&dev, dev.params->lines / 3, 2 * (dev.params->lines / 3), 0,
             dev.params->rgb_channels, 0x07E0);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    puts("Drawing red rectangle");
    lcd_fill(&dev, 2 * (dev.params->lines / 3), dev.params->lines, 0,
             dev.params->rgb_channels, 0xf800);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    lcd_invert_on(&dev);
    puts("lcd TFT display inverted");
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);
    lcd_invert_off(&dev);
    puts("lcd TFT display normal");

    puts("lcd TFT display clear screen");
    lcd_fill(&dev, 0, dev.params->lines, 0, dev.params->rgb_channels, 0x0000);
#ifndef CONFIG_NO_RIOT_IMAGE
    /* Approximate middle of the display */
    uint8_t x1 = (dev.params->lines / 2) - (RIOT_LOGO_WIDTH / 2);
    uint8_t y1 = (dev.params->rgb_channels / 2) - (RIOT_LOGO_HEIGHT / 2);
    lcd_pixmap(&dev, x1, x1 + RIOT_LOGO_WIDTH - 1, y1, y1 +  RIOT_LOGO_HEIGHT - 1,
               (const uint16_t *)picture);
#endif
    while (1) {}

    return 0;
}
