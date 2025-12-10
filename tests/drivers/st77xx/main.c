/*
 * SPDX-FileCopyrightText: 2018 Koen Zandberg <koen@bergzand.net>
 * SPDX-FileCopyrightText: 2021 Francisco Molina
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for lcd tft displays
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include "timex.h"
#include "ztimer.h"
#include "benchmark.h"
#include "board.h"
#include "lcd.h"
#include "lcd_internal.h"

#include "riot_logo.h"

#include "st77xx.h"
#include "st77xx_params.h"

int main(void)
{
    lcd_t dev;
    dev.driver = &lcd_st77xx_driver;

    puts("lcd TFT display test application");

    /* initialize the sensor */
    printf("Initializing display...");

    /* Enable backlight if macro is defined */
#ifdef BACKLIGHT_ON
    BACKLIGHT_ON;
#endif

    if (lcd_init(&dev, &st77xx_params[0]) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return 1;
    }

#if MODULE_LCD_PARALLEL
    if (gpio_is_valid(st77xx_params[0].rdx_pin)) {
        uint8_t data[4];

        lcd_read_cmd(&dev, LCD_CMD_RDDIDIF, data, 3);
        printf("lcd ID: %02x %02x %02x\n", data[0], data[1], data[2]);

        lcd_read_cmd(&dev, LCD_CMD_RDDST, data, 4);
        printf("lcd status: %02x %02x %02x %02x\n", data[0], data[1], data[2], data[3]);
    }
#endif

    puts("lcd TFT display filling map");
    lcd_fill(&dev, 0, dev.params->lines - 1, 0, dev.params->rgb_channels - 1,
             0x0000);
    puts("lcd TFT display map filled");

    /* Fill square with blue */
    puts("Drawing blue rectangle");
    lcd_fill(&dev, 0, dev.params->lines / 3, 0, dev.params->rgb_channels - 1,
             0x001F);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    puts("Drawing green rectangle");
    lcd_fill(&dev, dev.params->lines / 3, 2 * (dev.params->lines / 3), 0,
             dev.params->rgb_channels - 1, 0x07E0);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    puts("Drawing red rectangle");
    lcd_fill(&dev, 2 * (dev.params->lines / 3), dev.params->lines, 0,
             dev.params->rgb_channels - 1, 0xf800);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    lcd_invert_on(&dev);
    puts("lcd TFT display inverted");
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);
    lcd_invert_off(&dev);
    puts("lcd TFT display normal");

    puts("lcd TFT display clear screen with benchmarking");
    BENCHMARK_FUNC("fill", 1,
                   lcd_fill(&dev,
                            0, dev.params->lines - 1,
                            0, dev.params->rgb_channels - 1,
                            0x0000));

#ifndef CONFIG_NO_RIOT_IMAGE
    printf("Write pixmap of size %u x %u with benchmarking\n",
           RIOT_LOGO_WIDTH, RIOT_LOGO_HEIGHT);
    /* Approximate middle of the display */
    uint8_t x1 = (dev.params->lines / 2) - (RIOT_LOGO_WIDTH / 2);
    uint8_t y1 = (dev.params->rgb_channels / 2) - (RIOT_LOGO_HEIGHT / 2);
    BENCHMARK_FUNC("fill", 1,
                   lcd_pixmap(&dev,
                              x1, x1 + RIOT_LOGO_WIDTH - 1,
                              y1, y1 +  RIOT_LOGO_HEIGHT - 1,
                              (const uint16_t *)picture));
#endif
    while (1) {}

    return 0;
}
