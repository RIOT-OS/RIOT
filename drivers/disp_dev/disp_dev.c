/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include <stddef.h>
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
                  const disp_dev_area_t *area,
                  const uint16_t *color)
{
    assert(dev);

    dev->driver->map(dev, area, color);
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
