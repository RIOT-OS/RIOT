/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_stmpe811
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

#include "stmpe811.h"
#include "stmpe811_touch_dev.h"

#define ENABLE_DEBUG 0
#include "debug.h"

uint16_t _stmpe811_height(const touch_dev_t *touch_dev)
{
    const stmpe811_t *dev = (const stmpe811_t *)touch_dev;
    assert(dev);

    return dev->params.ymax;
}

uint16_t _stmpe811_width(const touch_dev_t *touch_dev)
{
    const stmpe811_t *dev = (const stmpe811_t *)touch_dev;
    assert(dev);

    return dev->params.xmax;
}

uint8_t _stmpe811_touches(const touch_dev_t *touch_dev, touch_t *touches, size_t len)
{
    (void)len;

    stmpe811_t *dev = (stmpe811_t *)touch_dev;
    assert(dev);

    stmpe811_touch_state_t state;
    stmpe811_read_touch_state(dev, &state);
    uint8_t ret = (state == STMPE811_TOUCH_STATE_PRESSED);

    if (ret && touches != NULL) {
        stmpe811_touch_position_t pos;
        stmpe811_read_touch_position(dev, &pos);
        touches[0].x = pos.x;
        touches[0].y = pos.y;

        DEBUG("X: %i, Y: %i\n", touches[0].x, touches[0].y);
    }

    return ret;
}

void _stmpe811_set_event_callback(const touch_dev_t *touch_dev, touch_event_cb_t cb, void *arg)
{
    stmpe811_t *dev = (stmpe811_t *)touch_dev;
    assert(dev);

    if (gpio_is_valid(dev->params.int_pin)) {
        gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_FALLING, cb, arg);
    }
}

const touch_dev_driver_t stmpe811_touch_dev_driver = {
    .height     = _stmpe811_height,
    .width      = _stmpe811_width,
    .touches    = _stmpe811_touches,
    .set_event_callback = _stmpe811_set_event_callback,
};
