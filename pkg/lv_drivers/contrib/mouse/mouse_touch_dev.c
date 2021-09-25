/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_lvgl_drivers_mouse
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

#include "indev/mouse.h"
#include "indev/mouse_touch_dev.h"

#define ENABLE_DEBUG 0
#include "debug.h"

uint16_t _mouse_height(const touch_dev_t *touch_dev)
{
    (void)touch_dev;
    return LV_VER_RES;
}

uint16_t _mouse_width(const touch_dev_t *touch_dev)
{
    (void)touch_dev;
    return LV_HOR_RES;
}

uint8_t _mouse_touches(const touch_dev_t *touch_dev, touch_t *touches, size_t len)
{
    (void)len;
    (void)touch_dev;
    lv_indev_data_t data;
    mouse_read(NULL, &data);
    touches[0].x = data.point.x;
    touches[0].y = data.point.y;
    return data.state == LV_INDEV_STATE_PR ? 1 : 0;
}

void _mouse_set_event_callback(const touch_dev_t *touch_dev, touch_event_cb_t cb, void *arg)
{
    (void)cb;
    (void)touch_dev;
    (void)arg;
}

const touch_dev_driver_t mouse_touch_dev_driver = {
    .height = _mouse_height,
    .width = _mouse_width,
    .touches = _mouse_touches,
    .set_event_callback = _mouse_set_event_callback,
};
