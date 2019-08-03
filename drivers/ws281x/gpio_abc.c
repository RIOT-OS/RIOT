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
 * @brief       Implementation of `ws281x_write_buffer()` using GPIO ABC
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "ws281x.h"
#include "ws281x_constants.h"
#include "periph/gpio.h"
#include "periph/gpio_abc.h"

void ws281x_write_buffer(ws281x_t *dev, const void *_buf, size_t size)
{
    const uint8_t *buf = _buf;
    assert(dev);
    for (unsigned i_byte = 0; i_byte < size; i_byte++) {
        for (unsigned i_bit = 0; i_bit < 8; i_bit++) {
            if (buf[i_byte] & (0x80 >> i_bit)) {
                gpio_set_for(dev->params.pin, WS281X_TICKS_ONE_HIGH);
                gpio_clear_for(dev->params.pin, WS281X_TICKS_ONE_LOW);
            }
            else {
                gpio_set_for(dev->params.pin, WS281X_TICKS_ZERO_HIGH);
                gpio_clear_for(dev->params.pin, WS281X_TICKS_ZERO_LOW);
            }
        }
    }
}

int ws281x_init(ws281x_t *dev, const ws281x_params_t *params)
{
    if (!dev || !params || !params->buf) {
        return -EINVAL;
    }

    memset(dev, 0, sizeof(ws281x_t));
    dev->params = *params;

    if (gpio_init(dev->params.pin, GPIO_OUT)) {
        return -EIO;
    }

    return 0;
}
