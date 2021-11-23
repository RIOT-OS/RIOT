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
 * @brief       Test application for the ili9431 tft display
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include "timex.h"
#include "ztimer.h"
#include "board.h"
#include "ili9341.h"
#include "ili9341_params.h"

#include "riot_logo.h"

int main(void)
{
    ili9341_t dev;

    puts("ili9341 TFT display test application");

    /* initialize the sensor */
    printf("Initializing display...");

    /* Enable backlight if macro is defined */
#ifdef BACKLIGHT_ON
    BACKLIGHT_ON;
#endif

    if (ili9341_init(&dev, &ili9341_params[0]) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    puts("ili9341 TFT display filling map");
    ili9341_fill(&dev, 0, 319, 0, 239, 0x0000);
    puts("ili9341 TFT display map filled");

    /* Fill square with blue */
    puts("Drawing blue rectangle");
    ili9341_fill(&dev, 10, 59, 10, 109, 0x001F);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    puts("Drawing green rectangle");
    ili9341_fill(&dev, 10, 59, 10, 109, 0x07E0);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    puts("Drawing red rectangle");
    ili9341_fill(&dev, 10, 59, 10, 109, 0xf800);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    ili9341_invert_on(&dev);
    puts("ili9341 TFT display inverted");
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);
    ili9341_invert_off(&dev);
    puts("ili9341 TFT display normal");

    /* Make the same square black again */
    ili9341_fill(&dev, 10, 59, 10, 109, 0x0000);

#ifndef CONFIG_NO_RIOT_IMAGE
    /* Approximate middle of the display */
    ili9341_pixmap(&dev, 95, 222, 85, 153, (const uint16_t *)picture);
#endif
    while (1) {
    }

    return 0;
}
