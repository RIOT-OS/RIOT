/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Generic display device test application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "disp_dev.h"

#include "riot_logo.h"

#include "test_utils/expect.h"

#if IS_USED(MODULE_ILI9341)
#include "ili9341.h"
#endif

#ifdef LCD_SCREEN_WIDTH
#define DISPLAY_BUFFER_MAX_SIZE (LCD_SCREEN_WIDTH)
#else
#define DISPLAY_BUFFER_MAX_SIZE (320)
#endif
static uint16_t display_buffer[DISPLAY_BUFFER_MAX_SIZE] = { 0 };

int main(void)
{
    /* Use the first screen */
    disp_dev_reg_t *disp_dev = disp_dev_reg_find_screen(0);
    if (!disp_dev) {
        puts("No screen found!");
        return -1;
    }

    disp_dev_set_invert(disp_dev->dev, true);
    disp_dev_backlight_on();

    uint16_t max_width = disp_dev_width(disp_dev->dev);
    uint16_t max_height = disp_dev_height(disp_dev->dev);

#if IS_USED(MODULE_ILI9341)
    ili9341_t *ili9341 = (ili9341_t *)disp_dev->dev;
    expect(ili9341);
    expect(max_width == ili9341->dev.params->lines);
    expect(max_height == 240);
#endif

    disp_dev_area_t area;
    for (uint16_t y = 0; y < max_height; ++y) {
        area.x1 = 0;
        area.x2 = max_width - 1;
        area.y1 = y;
        area.y2 = y;
        disp_dev_map(disp_dev->dev, &area, display_buffer);
    }

    area.x1 = ((max_width - RIOT_LOGO_WIDTH) >> 1);
    area.x2 = ((max_width + RIOT_LOGO_WIDTH) >> 1) - 1;
    area.y1 = ((max_height - RIOT_LOGO_HEIGHT) >> 1);
    area.y2 = ((max_height + RIOT_LOGO_HEIGHT) >> 1) - 1;
    disp_dev_map(disp_dev->dev, &area, (const uint16_t *)picture);

    puts("SUCCESS");

    return 0;
}
