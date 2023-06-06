/*
 * Copyright (C) 2019 Silke Hofstra
 * Copyright (C) 2023 Koen Zandberg
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
 * @brief       Test application for uc8151d-based e-paper SPI display driver.
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 * @author      Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <stdio.h>

#include "ztimer.h"

#include "uc8151d.h"
#include "uc8151d_params.h"

#include "pictures.h"

static const uint8_t riot_32_width = 32;
static const uint16_t riot_32_height = sizeof(riot_logo_32) / sizeof(riot_logo_32[0]);

/* Empty the display to a single color */
static void empty_screen(uc8151d_t *dev)
{
    uc8151d_wake(dev);
    uc8151d_init_full(dev);
    uc8151d_fill(dev, 0, 152, 0, 296, UC8151D_COLOR_WHITE);
    uc8151d_update_full(dev);
    uc8151d_sleep(dev);
}

static void fill_bar(uc8151d_t *dev, uint16_t y_start, uint16_t y_max)
{
    uc8151d_wake(dev);
    uc8151d_init_full(dev);
    /* Multiple regions can be updated as long as a full update is used */
    uc8151d_fill(dev, 0, 152, y_start, y_max, UC8151D_COLOR_BLACK);
    uc8151d_fill(dev, 0, 152, y_start + 32, y_max + 32, UC8151D_COLOR_BLACK);
    uc8151d_update_full(dev);
    uc8151d_sleep(dev);
}

void draw_small_riot(uc8151d_t *dev, uint8_t x, uint16_t y)
{
    uc8151d_wake(dev);
    uc8151d_init_part(dev);
    uc8151d_set_area(dev, x, x + riot_32_width, y, y + riot_32_height);
    uc8151d_write_buffer(dev, (uint8_t *)riot_logo_32, sizeof riot_logo_32);
    uc8151d_update_part(dev);
    uc8151d_sleep(dev);
}

int main(void)
{
    uc8151d_t dev;
    int init = uc8151d_init(&dev, uc8151d_params);

    if (init != 0) {
        printf("[uc8151d] Init failed: %i\n", init);
        return 1;
    }

    bool invert = false;
    uc8151d_set_frame_rate(&dev, CONFIG_UC8151D_FRAME_RATE_FAST);
    while (1) {
        /* Set both RAM buffers to the RIOT logo */
        uc8151d_wake(&dev);
        uc8151d_set_invert(&dev, invert);
        empty_screen(&dev);

        for (uint16_t i = 0; i < (296 - 48); i += 48) {
            fill_bar(&dev, i, i + 16);
        }
        ztimer_sleep(ZTIMER_MSEC, 2000);

        /* Draw small RIOT logos on the display */
        for (uint16_t y = 0; y < dev.params.size_y; y += riot_32_height) {
            for (uint16_t x = 0; x < dev.params.size_x; x += riot_32_width) {
                draw_small_riot(&dev, x, y);

                ztimer_sleep(ZTIMER_MSEC, 1);
            }
        }
        invert = !invert;
        uc8151d_set_frame_rate(&dev, CONFIG_UC8151D_FRAME_RATE_SLOW);
    }

    return 0;
}
