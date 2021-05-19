/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#include "disp_dev.h"

#include "riot_logo.h"

#include "test_utils/expect.h"

#if IS_USED(MODULE_ILI9341)
#include "ili9341.h"
#endif

#define DISPLAY_BUFFER_MAX_SIZE (320)
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
    expect(max_width == ili9341->params->lines);
    expect(max_height == 240);
#endif

    for (uint16_t y = 0; y < max_height; ++y) {
        disp_dev_map(disp_dev->dev, 0, max_width - 1, y, y, display_buffer);
    }

    disp_dev_map(disp_dev->dev, 95, 222, 85, 153, (const uint16_t *)picture);

    puts("SUCCESS");

    return 0;
}
