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
#include "qrcodegen.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifndef MESSAGE_TO_ENCODE
#define MESSAGE_TO_ENCODE   "unknown"
#endif

static uint8_t qr0[qrcodegen_BUFFER_LEN_FOR_VERSION(2)];
static uint8_t buffer[qrcodegen_BUFFER_LEN_FOR_VERSION(2)];

int main(void)
{
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

    uint16_t white = UINT16_MAX;
    uint16_t black = 0;
    for (uint16_t y = 0; y < disp_dev_height(disp_dev->dev); y++) {
        for (uint16_t x = 0; x <  disp_dev_width(disp_dev->dev); x++) {
            disp_dev_map(disp_dev->dev, x, x, y, y, &black);
        }
    }

    const uint8_t scale = disp_dev_height(disp_dev->dev) / size;
    const uint8_t w_offset = (disp_dev_width(disp_dev->dev) - (size * scale)) / 2;
    const uint8_t h_offset = (disp_dev_height(disp_dev->dev) - (size * scale)) / 2;
#endif

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
#ifdef MODULE_DISP_DEV
            for (int w = x * scale; w < (x + 1) * scale; w++) {
                for (int h = y * scale; h < (y + 1) * scale; h++) {
                    if (qrcodegen_getModule(qr0, x, y)) {
                        disp_dev_map(disp_dev->dev,
                                     w + w_offset, w + w_offset, h + h_offset, h + h_offset,
                                     &white);
                    }
                }
            }
#endif
            printf("%s", qrcodegen_getModule(qr0, x, y) ? "██" : "  ");

        }
        puts("");
    }

    return 0;
}
