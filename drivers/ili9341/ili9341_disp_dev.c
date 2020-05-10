/*
 * Copyright (C) 2020 Inria
 *               2020 Philipp-Alexander Blum
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ili9341
 * @{
 *
 * @file
 * @brief       Driver adaption to disp_dev generic interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "ili9341.h"
#include "ili9341_disp_dev.h"

#ifndef ILI9341_DISP_DEV_HEIGHT
#define ILI9341_DISP_DEV_HEIGHT     (240U)
#endif

#ifndef ILI9341_DISP_COLOR_DEPTH
#define ILI9341_DISP_COLOR_DEPTH    (16U)
#endif

static void _ili9341_map(disp_dev_t *dev, disp_dev_coordinates_t *coordinates,
                         const uint16_t *color)
{
    ili9341_t *ili9341 = (ili9341_t *)dev;
    ili9341_pixmap(ili9341, (ili9341_coordinates_t *)coordinates, color);
}

static uint16_t _ili9341_height(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return ILI9341_DISP_DEV_HEIGHT;
}

static uint16_t _ili9341_width(const disp_dev_t *disp_dev)
{
    const ili9341_t *dev = (ili9341_t *)disp_dev;
    assert(dev);

    return dev->params->lines;
}

static uint8_t _ili9341_color_depth(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return ILI9341_DISP_COLOR_DEPTH;
}

static void _ili9341_set_invert(const disp_dev_t *disp_dev, bool invert)
{
    const ili9341_t *dev = (ili9341_t *)disp_dev;

    assert(dev);

    if (invert) {
        ili9341_invert_on(dev);
    }
    else {
        ili9341_invert_off(dev);
    }
}

const disp_dev_driver_t ili9341_disp_dev_driver = {
    .map            = _ili9341_map,
    .height         = _ili9341_height,
    .width          = _ili9341_width,
    .color_depth    = _ili9341_color_depth,
    .set_invert     = _ili9341_set_invert,
};
