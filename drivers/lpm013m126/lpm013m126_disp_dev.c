/*
 * Copyright (C) 2020 Inria
 * Copyright (c) 2022 Digitaluhr Manufaktur
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lpm013m126
 * @{
 *
 * @file
 * @brief       Driver adaption to disp_dev generic interface
 *
 * @author      Nicole Faerber <nicole.faerber@digitaluhr-maunfaktur.de>
 * @}
 */

#include <assert.h>
#include <stdint.h>

#define ENABLE_DEBUG 1
#include "debug.h"

#include "lpm013m126.h"
#include "lpm013m126_disp_dev.h"

#ifndef LPM013M126_DISP_DEV_HEIGHT
#define LPM013M126_DISP_DEV_HEIGHT     (176U)
#endif

#ifndef LPM013M126_DISP_DEV_WIDTH
#define LPM013M126_DISP_DEV_WIDTH     (176U)
#endif

/*
 * in fact the LCD is 3bpp but smallest LVGL supports is 8bpp
 * results in RGB 332
*/
#ifndef LPM013M126_DISP_COLOR_DEPTH
#define LPM013M126_DISP_COLOR_DEPTH    (8U)
#endif

static void _lpm013m126_map(const disp_dev_t *dev, const disp_dev_area_t *area, const uint16_t *color)
{
    lpm013m126_t *lpm013m126 = (lpm013m126_t *)dev;
    assert(dev);

    lpm013m126_pixmap(lpm013m126, area->x1, area->x2, area->y1, area->y2, color);
}

static uint16_t _lpm013m126_height(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return LPM013M126_DISP_DEV_HEIGHT;
}

static uint16_t _lpm013m126_width(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return LPM013M126_DISP_DEV_WIDTH;
}

static uint8_t _lpm013m126_color_depth(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return LPM013M126_DISP_COLOR_DEPTH;
}

static void _lpm013m126_set_invert(const disp_dev_t *disp_dev, bool invert)
{
    const lpm013m126_t *dev = (lpm013m126_t *)disp_dev;

    assert(dev);

    if (invert) {
        lpm013m126_invert(dev);
    } else {
        lpm013m126_noupdate(dev);
    }
}

const disp_dev_driver_t lpm013m126_disp_dev_driver = {
    .map            = _lpm013m126_map,
    .height         = _lpm013m126_height,
    .width          = _lpm013m126_width,
    .color_depth    = _lpm013m126_color_depth,
    .set_invert     = _lpm013m126_set_invert,
};
