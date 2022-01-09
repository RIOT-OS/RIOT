/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ft5x06
 * @{
 *
 * @file
 * @brief       Driver adaption to touch_dev generic interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "kernel_defines.h"
#include "periph/gpio.h"

#include "ft5x06.h"
#include "ft5x06_touch_dev.h"

#define ENABLE_DEBUG 0
#include "debug.h"

uint16_t _ft5x06_height(const touch_dev_t *touch_dev)
{
    const ft5x06_t *dev = (const ft5x06_t *)touch_dev;
    assert(dev);

    return dev->params.ymax;
}

uint16_t _ft5x06_width(const touch_dev_t *touch_dev)
{
    const ft5x06_t *dev = (const ft5x06_t *)touch_dev;
    assert(dev);

    return dev->params.xmax;
}

uint8_t _ft5x06_touches(const touch_dev_t *touch_dev, touch_t *touches, size_t len)
{
    ft5x06_t *dev = (ft5x06_t *)touch_dev;
    assert(dev);

    uint8_t ret;
    ft5x06_read_touch_count(dev, &ret);

    if (ret && touches != NULL) {
        assert(len <= FT5X06_TOUCHES_COUNT_MAX);
        ft5x06_read_touch_positions(dev, (ft5x06_touch_position_t *)touches, len);
    }

    return ret;
}

void _ft5x06_set_event_callback(const touch_dev_t *touch_dev, touch_event_cb_t cb, void *arg)
{
    ft5x06_t *dev = (ft5x06_t *)touch_dev;
    assert(dev);

    if (gpio_is_valid(dev->params.int_pin)) {
        gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_RISING, cb, arg);
    }
}

const touch_dev_driver_t ft5x06_touch_dev_driver = {
    .height     = _ft5x06_height,
    .width      = _ft5x06_width,
    .touches    = _ft5x06_touches,
    .set_event_callback = _ft5x06_set_event_callback,
};
