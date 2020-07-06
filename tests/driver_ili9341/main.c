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
#include "xtimer.h"
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
    ili9341_coordinates_t coordinates;

    puts("ili9341 TFT display filling map");
    coordinates.x1 = 0;
    coordinates.x2 = 319;
    coordinates.y1 = 0;
    coordinates.y2 = 239;
    ili9341_fill(&dev, &coordinates, 0x0000);
    puts("ili9341 TFT display map filled");

    /* Fill square with blue */
    puts("Drawing blue rectangle");
    coordinates.x1 = 10;
    coordinates.x2 = 59;
    coordinates.y1 = 10;
    coordinates.y2 = 109;
    ili9341_fill(&dev, &coordinates, 0x001F);
    xtimer_sleep(1);

    puts("Drawing green rectangle");
    ili9341_fill(&dev, &coordinates, 0x07E0);
    xtimer_sleep(1);

    puts("Drawing red rectangle");
    ili9341_fill(&dev, &coordinates, 0xf800);
    xtimer_sleep(1);

    ili9341_invert_on(&dev);
    puts("ili9341 TFT display inverted");
    xtimer_sleep(1);
    ili9341_invert_off(&dev);
    puts("ili9341 TFT display normal");

    /* Make the same square black again */
    ili9341_fill(&dev, &coordinates, 0x0000);

#ifndef NO_RIOT_IMAGE
    /* Approximate middle of the display */
    coordinates.x1 = 95;
    coordinates.x2 = 222;
    coordinates.y1 = 85;
    coordinates.y2 = 153;
    ili9341_pixmap(&dev, &coordinates, (const uint16_t *)picture);
#endif
    while (1) {
    }

    return 0;
}
