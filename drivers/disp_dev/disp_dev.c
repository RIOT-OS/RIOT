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
#include <errno.h>

#include "disp_dev.h"

disp_dev_reg_t *disp_dev_reg = NULL;

int disp_dev_reg_add(disp_dev_reg_t *dev)
{
    disp_dev_reg_t *tmp = disp_dev_reg;

    if (dev == NULL) {
        return -ENODEV;
    }

    /* prepare new entry */
    dev->next = NULL;
    /* add to registry */
    if (disp_dev_reg == NULL) {
        disp_dev_reg = dev;
    }
    else {
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = dev;
    }
    return 0;
}

disp_dev_reg_t *disp_dev_reg_find_screen(uint8_t screen_id)
{
    disp_dev_reg_t *tmp = disp_dev_reg;

    while (tmp && tmp->screen_id != screen_id) {
        tmp = tmp->next;
    }

    return tmp;
}

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
