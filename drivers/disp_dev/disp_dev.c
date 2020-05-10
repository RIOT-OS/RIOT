/*
 * Copyright (C) 2020 Inria
 *               2020 Philipp-Alexander Blum <philipp-blum@jakiku.de>
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
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>

#include "disp_dev.h"

void disp_dev_map(disp_dev_t *dev, disp_dev_coordinates_t *coordinates,
                 const uint16_t *color)
{
    assert(dev);

    dev->driver->map(dev, coordinates, color);
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
