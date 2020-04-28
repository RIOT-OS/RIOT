/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_touch_dev
 * @{
 *
 * @file
 * @brief       Helper functions for generic API of touch device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>

#include "touch_dev.h"

uint16_t touch_dev_height(const touch_dev_t *dev)
{
    assert(dev);

    return dev->driver->height(dev);
}

uint16_t touch_dev_width(const touch_dev_t *dev)
{
    assert(dev);

    return dev->driver->width(dev);
}

uint8_t touch_dev_touches(const touch_dev_t *dev, touch_t *touches, size_t len)
{
    assert(dev);

    return dev->driver->touches(dev, touches, len);
}
