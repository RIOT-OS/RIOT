/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_disp_dev
 * @{
 *
 * @file
 * @brief       Helper functions for generic API of display device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>

#include "disp_dev.h"

void disp_dev_map(const disp_dev_t *dev,
                  uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2,
                  const uint16_t *color)
{
    assert(dev);

    dev->driver->map(dev, x1, x2, y1, y2, color);
}

uint16_t disp_dev_height(const disp_dev_t *dev)
{
    assert(dev);

    return dev->driver->height(dev);
}

uint16_t disp_dev_width(const disp_dev_t *dev)
{
    assert(dev);

    return dev->driver->width(dev);
}

uint8_t disp_dev_color_depth(const disp_dev_t *dev)
{
    assert(dev);

    return dev->driver->color_depth(dev);
}

void disp_dev_set_invert(const disp_dev_t *dev, bool invert)
{
    assert(dev);

    dev->driver->set_invert(dev, invert);
}
