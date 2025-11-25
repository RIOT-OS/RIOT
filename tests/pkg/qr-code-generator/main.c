/*
 * Copyright (C) 2021 Inria
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
 * @brief       Test application for the qr-code-generator package
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include <stdbool.h>
#include <stdint.h>

#include "test_utils/expect.h"

#include "qrcodegen.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifndef MESSAGE_TO_ENCODE
#define MESSAGE_TO_ENCODE   "unknown"
#endif

/* Compute buffer sizes based on the message to encode len to avoid a
 * failed assertion in qrcodegen */
#define ENCODER_VERSION     (sizeof(MESSAGE_TO_ENCODE) >> 3)
static uint8_t qr0[qrcodegen_BUFFER_LEN_FOR_VERSION(ENCODER_VERSION)];
static uint8_t buffer[qrcodegen_BUFFER_LEN_FOR_VERSION(ENCODER_VERSION)];

#ifdef MODULE_DISP_DEV
#ifdef LCD_SCREEN_WIDTH
#define DISPLAY_BUFFER_MAX_SIZE (LCD_SCREEN_WIDTH)
#else
#define DISPLAY_BUFFER_MAX_SIZE (320)
#endif
static uint16_t display_buffer[DISPLAY_BUFFER_MAX_SIZE] = { 0 };
#endif

int main(void)
{
    expect(ENCODER_VERSION <= qrcodegen_VERSION_MAX);

    if (!qrcodegen_encodeText(MESSAGE_TO_ENCODE,
                              buffer, qr0, qrcodegen_Ecc_MEDIUM,
                              qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX,
                              qrcodegen_Mask_AUTO, true)) {
        puts("Encoding error");
        return -1;
    }

    int size = qrcodegen_getSize(qr0);

#ifdef MODULE_DISP_DEV
    /* Use the first screen */
    disp_dev_reg_t *disp_dev = disp_dev_reg_find_screen(0);
    if (!disp_dev) {
        puts("No screen found!");
        return -1;
    }
    disp_dev_backlight_on();

    /* Compute scaling factor and height/width offsets */
    const uint8_t scale = disp_dev_height(disp_dev->dev) / size;
    const uint8_t w_offset = (disp_dev_width(disp_dev->dev) - (size * scale)) / 2;
    const uint8_t h_offset = (disp_dev_height(disp_dev->dev) - (size * scale)) / 2;

    /* Clear the screen */
    disp_dev_area_t area;
    for (uint16_t y = 0; y < disp_dev_height(disp_dev->dev); y ++) {
        area.x1 = 0;
        area.x2 = disp_dev_width(disp_dev->dev) - 1;
        area.y1 = area.y2 = y;
        disp_dev_map(disp_dev->dev, &area, display_buffer);
    }

    /* Prepare a subset of the display buffer for white tiles */
    for (int w = 0; w < scale; w++) {
        for (int h = 0; h < scale; h++) {
            display_buffer[w + h * scale] = UINT16_MAX;
        }
    }
#endif

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
#ifdef MODULE_DISP_DEV
            if (qrcodegen_getModule(qr0, x, y)) {
                area.x1 = w_offset + (x * scale);
                area.x2 = w_offset + ((x + 1)* scale) - 1;
                area.y1 = h_offset + (y * scale);
                area.y2 = h_offset + ((y + 1)* scale) - 1;
                disp_dev_map(disp_dev->dev, &area, display_buffer);
            }
#endif
            printf("%s", qrcodegen_getModule(qr0, x, y) ? "██" : "  ");

        }
        puts("");
    }

    return 0;
}
