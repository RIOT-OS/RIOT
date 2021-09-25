/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_lvgl_drivers_monitor
 * @{
 *
 * @file
 * @brief       Driver adaption to disp_dev generic interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include "display/monitor.h"
#include "display/monitor_disp_dev.h"

static void _monitor_map(const disp_dev_t *dev, uint16_t x1, uint16_t x2,
                         uint16_t y1, uint16_t y2, const uint16_t *color)
{
    const monitor_t *monitor = (monitor_t *)dev;
    lv_area_t area = { .x1 = x1, .y1 = y1, .x2 = x2, .y2 = y2 };

    monitor_flush(monitor->disp_drv, &area, (lv_color_t *)color);
}

static uint16_t _monitor_height(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return LV_VER_RES;
}

static uint16_t _monitor_width(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return LV_HOR_RES;
}

static uint8_t _monitor_color_depth(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return LV_COLOR_DEPTH;
}

static void _monitor_set_invert(const disp_dev_t *disp_dev, bool invert)
{
    (void)disp_dev;
    (void)invert;
}

const disp_dev_driver_t monitor_disp_dev_driver = {
    .map = _monitor_map,
    .height = _monitor_height,
    .width = _monitor_width,
    .color_depth = _monitor_color_depth,
    .set_invert = _monitor_set_invert,
};
