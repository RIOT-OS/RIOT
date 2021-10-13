/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 *
 * @file
 * @brief       Driver for the WS2812 or the SK6812 RGB LEDs sold as NeoPixel
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "ws281x.h"
#include "ws281x_constants.h"
#include "ws281x_params.h"
#include "periph/gpio.h"

/* Default buffer used in ws281x_params.h. Will be optimized out if unused */
uint8_t ws281x_buf[WS281X_PARAM_NUMOF * WS281X_BYTES_PER_DEVICE];

void ws281x_set_buffer(void *_dest, uint16_t n, color_rgb_t c)
{
    uint8_t *dest = _dest;
    dest[WS281X_BYTES_PER_DEVICE * n + WS281X_OFFSET_R] = c.r;
    dest[WS281X_BYTES_PER_DEVICE * n + WS281X_OFFSET_G] = c.g;
    dest[WS281X_BYTES_PER_DEVICE * n + WS281X_OFFSET_B] = c.b;
}
