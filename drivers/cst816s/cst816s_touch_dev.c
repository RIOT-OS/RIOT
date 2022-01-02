/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cst816s
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

#include "board.h"
#include "cst816s.h"
#include "cst816s_touch_dev.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CST816S_XMAX
#define CST816S_XMAX    240
#endif

#ifndef CST816S_YMAX
#define CST816S_YMAX    240
#endif

uint16_t _cst816s_height(const touch_dev_t *touch_dev)
{
    const cst816s_t *dev = (const cst816s_t *)touch_dev;
    assert(dev);

    return CST816S_YMAX;
}

uint16_t _cst816s_width(const touch_dev_t *touch_dev)
{
    const cst816s_t *dev = (const cst816s_t *)touch_dev;
    assert(dev);

    return CST816S_XMAX;
}

uint8_t _cst816s_touches(const touch_dev_t *touch_dev, touch_t *touches, size_t len)
{
    (void)len;

    cst816s_t *dev = (cst816s_t *)touch_dev;
    assert(dev);

    cst816s_touch_data_t data;
    cst816s_read(dev, &data);
    uint8_t ret = (data.action == CST816S_TOUCH_DOWN);

    if (ret && touches != NULL) {
        touches[0].x = data.x;
        touches[0].y = data.y;

        DEBUG("X: %i, Y: %i\n", touches[0].x, touches[0].y);
    }

    return ret;
}

void _cst816s_set_event_callback(const touch_dev_t *touch_dev, touch_event_cb_t cb, void *arg)
{
    cst816s_t *dev = (cst816s_t *)touch_dev;
    assert(dev);

    dev->cb = (cst816s_irq_cb_t)cb;
    dev->cb_arg = arg;
}

const touch_dev_driver_t cst816s_touch_dev_driver = {
    .height     = _cst816s_height,
    .width      = _cst816s_width,
    .touches    = _cst816s_touches,
    .set_event_callback = _cst816s_set_event_callback,
};
