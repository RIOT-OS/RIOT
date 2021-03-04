/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
        /* STMPE811 driver returns the position with origin at the bottom left
        corner and portrait orientation, so convert them to use top left corner
        as origin and landscape orientation. */
        touches[0].x = pos.y;
        touches[0].y = dev->params.xmax - pos.x;

        DEBUG("X: %i, Y: %i\n", touches[0].x, touches[0].y)
    }

    return ret;
}

void _stmpe811_set_event_callback(const touch_dev_t *touch_dev, touch_event_cb_t cb, void *arg)
{
    stmpe811_t *dev = (stmpe811_t *)touch_dev;
    assert(dev);

    dev->cb = (stmpe811_event_cb_t)cb;
    dev->cb_arg = arg;
}

const touch_dev_driver_t stmpe811_touch_dev_driver = {
    .height     = _stmpe811_height,
    .width      = _stmpe811_width,
    .touches    = _stmpe811_touches,
    .set_event_callback = _stmpe811_set_event_callback,
};
