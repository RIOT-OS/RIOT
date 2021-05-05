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
#include "lcd_disp_dev.h"
#include "riot_logo.h"

/* static buffer to clear a screen */
static const uint16_t blank[320][320] = {0x0000};

int main(void)
{
    disp_dev_reg_t *dev = disp_dev_reg;

    if (dev == NULL) {
        puts("No display device devices present");
        return 1;
    }

    while (dev) {
        printf("\nDisplay Id: %02x\n", dev->screen_id);
        disp_dev_set_invert(dev->dev, false);
        disp_dev_backlight_on();

        uint16_t max_width = disp_dev_width(dev->dev);
        uint16_t max_height = disp_dev_height(dev->dev);

        puts("\tprogessive clear all");
        uint16_t color = 0xFFFF;
        for (uint16_t y = 0; y < max_height; ++y) {
            for (uint16_t x = 0; x < max_width; ++x) {
                disp_dev_map(dev->dev, x, x, y, y, &color);
            }
        }
        printf("\tblank of %"PRIu16"x%"PRIu16"\n", max_width, max_height);
        disp_dev_map(dev->dev, 0, max_width - 1, 0, max_height - 1, (const uint16_t *)blank);

        /* Approximate middle of the display */
        uint8_t x1 = (max_width / 2) - (RIOT_LOGO_WIDTH / 2);
        uint8_t y1 = (max_height / 2) - (RIOT_LOGO_HEIGHT / 2);
        disp_dev_map(dev->dev, x1, x1 + RIOT_LOGO_WIDTH - 1, y1, y1 +  RIOT_LOGO_HEIGHT - 1, (const uint16_t *)picture);

        dev = dev->next;
    }

    puts("SUCCESS");

    return 0;
}
