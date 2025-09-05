/*
 * SPDX-FileCopyrightText: 2019 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 *
 * @file
 * @brief       Implementation of `ws281x_write()` for VT100 terminals
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 * @}
 */
#include <stdio.h>
#include "ws281x.h"

void ws281x_write_buffer(ws281x_t *dev, const void *buf, size_t size)
{
    (void) dev;
    const uint8_t *src = buf;

    for (unsigned i = 0; i < size; i += WS281X_BYTES_PER_DEVICE) {
        int r = src[i + WS281X_OFFSET_R];
        int g = src[i + WS281X_OFFSET_G];
        int b = src[i + WS281X_OFFSET_B];
        printf("\033[48;2;%d;%d;%dm ", r, g, b);
    }
}

void ws281x_prepare_transmission(ws281x_t *dev)
{
    (void) dev;

    /* clear the line and reset cursor position */
    printf("\033[2K\r");
}

void ws281x_end_transmission(ws281x_t *dev)
{
    (void) dev;

    /* set color back to normal */
    printf("\033[0m");
}
