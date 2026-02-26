/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       AMG88xx display output support
 *
 * Renders the 8x8 thermal image as a heatmap on a display device.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "byteorder.h"
#include "shell.h"
#include "disp_dev.h"

#include "amg88xx.h"
#include "amg88xx_constants.h"

#if IS_USED(MODULE_DISP_DEV)

/**
 * @brief   Pixel scale factor for display output
 *
 * Each sensor pixel is rendered as a DISPLAY_SCALE x DISPLAY_SCALE block. A
 * size of 8 results in a 64x64 pixel output.
 *
 * Larger values will use more memory and make display updates slower.
 */
#ifndef AMG88XX_TEST_DISPLAY_SCALE
#  define AMG88XX_TEST_DISPLAY_SCALE        (8)
#endif

/**
 * @brief   Temperature range for heatmap color mapping (raw units)
 *
 * Default range: 40..120 raw = 10.0..30.0 deg C.
 */
#ifndef AMG88XX_TEST_DISPLAY_TEMP_MIN
#  define AMG88XX_TEST_DISPLAY_TEMP_MIN     (40)
#endif
#ifndef AMG88XX_TEST_DISPLAY_TEMP_MAX
#  define AMG88XX_TEST_DISPLAY_TEMP_MAX     (120)
#endif

/**
 * @brief   Pointer to the display device to use for output.
 */
static disp_dev_t *_display = NULL;

/**
 * @brief   Buffer for one scaled scanline of pixel data
 */
static uint16_t _pixel_buf[AMG88XX_PIXELS_HORIZONTAL * AMG88XX_TEST_DISPLAY_SCALE *
                           AMG88XX_TEST_DISPLAY_SCALE];

/**
 * @brief   Convert a temperature value to an BGR565 heatmap color
 *
 * Maps the value from blue (cold) through green to red (hot).
 */
static uint16_t _temp_to_bgr565(int16_t val)
{
    /* clamp to range */
    if (val < AMG88XX_TEST_DISPLAY_TEMP_MIN) {
        val = AMG88XX_TEST_DISPLAY_TEMP_MIN;
    }
    if (val > AMG88XX_TEST_DISPLAY_TEMP_MAX) {
        val = AMG88XX_TEST_DISPLAY_TEMP_MAX;
    }

    /* normalize to 0..255 */
    int range = AMG88XX_TEST_DISPLAY_TEMP_MAX - AMG88XX_TEST_DISPLAY_TEMP_MIN;
    int norm = ((val - AMG88XX_TEST_DISPLAY_TEMP_MIN) * 255) / range;

    uint8_t r, g, b;

    if (norm < 128) {
        /* cold: blue to green */
        b = (uint8_t)(255 - norm * 2);
        g = (uint8_t)(norm * 2);
        r = 0;
    }
    else {
        /* hot: green to red */
        b = 0;
        g = (uint8_t)(255 - (norm - 128) * 2);
        r = (uint8_t)((norm - 128) * 2);
    }

    /* pack as BGR565 in little-endian (byte-swapped for SPI displays) */
    uint16_t color = ((b >> 3) << 11) | ((g >> 2) << 5) | (r >> 3);

    return htons(color);
}

void display_update(const int16_t *pixels)
{
    if (_display == NULL) {
        return;
    }

    const unsigned rows = AMG88XX_PIXELS_VERTICAL;
    const unsigned cols = AMG88XX_PIXELS_HORIZONTAL;
    const unsigned scale = AMG88XX_TEST_DISPLAY_SCALE;
    const unsigned row_width = cols * scale;

    for (unsigned row = 0; row < rows; row++) {
        /* compute one scaled scanline */
        for (unsigned col = 0; col < cols; col++) {
            unsigned index = (row * cols) + col;

            uint16_t color = _temp_to_bgr565(pixels[index]);

            for (unsigned s = 0; s < scale; s++) {
                _pixel_buf[col * scale + s] = color;
            }
        }

        /* duplicate the first scanline for the remaining scale-1 lines */
        for (unsigned s = 1; s < scale; s++) {
            memcpy(&_pixel_buf[s * row_width], _pixel_buf, row_width * sizeof(uint16_t));
        }

        /* write the entire block in one call */
        disp_dev_area_t area = {
            .x1 = 0,
            .x2 = (uint16_t)(row_width - 1),
            .y1 = (uint16_t)(row * scale),
            .y2 = (uint16_t)((row + 1) * scale - 1),
        };

        disp_dev_map(_display, &area, _pixel_buf);
    }
}

static int _cmd_display(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <on|off>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "on") == 0) {
        if (_display == NULL) {
            disp_dev_reg_t *reg = disp_dev_reg_find_screen(0);

            if (reg == NULL) {
                puts("error: no display found");
                return 1;
            }

            _display = reg->dev;
        }

        disp_dev_backlight_on();

        /* clear the screen by reusing _pixel_buf as a zeroed row buffer */
        uint16_t w = disp_dev_width(_display);
        uint16_t h = disp_dev_height(_display);

        memset(_pixel_buf, 0, sizeof(_pixel_buf));

        for (uint16_t y = 0; y < h; y++) {
            for (uint16_t x = 0; x < w; x += ARRAY_SIZE(_pixel_buf)) {
                uint16_t chunk = ((unsigned)(w - x) < ARRAY_SIZE(_pixel_buf))
                               ? (uint16_t)(w - x) : ARRAY_SIZE(_pixel_buf);
                disp_dev_area_t area = {
                    .x1 = x,
                    .x2 = (uint16_t)(x + chunk - 1),
                    .y1 = y,
                    .y2 = y,
                };
                disp_dev_map(_display, &area, _pixel_buf);
            }
        }

        puts("Display output enabled");
    }
    else if (strcmp(argv[1], "off") == 0) {
        disp_dev_backlight_off();
        _display = NULL;

        puts("Display output disabled");
    }
    else {
        printf("usage: %s <on|off>\n", argv[0]);
        return 1;
    }

    return 0;
}

SHELL_COMMAND(display, "enable or disable display output (on|off)", _cmd_display);

#endif
