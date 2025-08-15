/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include <errno.h>

#include "touch_dev.h"

touch_dev_reg_t *touch_dev_reg = NULL;

int touch_dev_reg_add(touch_dev_reg_t *dev)
{
    touch_dev_reg_t *tmp = touch_dev_reg;

    if (dev == NULL) {
        return -ENODEV;
    }

    /* prepare new entry */
    dev->next = NULL;
    /* add to registry */
    if (touch_dev_reg == NULL) {
        touch_dev_reg = dev;
    }
    else {
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = dev;
    }
    return 0;
}

touch_dev_reg_t *touch_dev_reg_find_screen(uint8_t screen_id)
{
    touch_dev_reg_t *tmp = touch_dev_reg;

    while (tmp && tmp->screen_id != screen_id) {
        tmp = tmp->next;
    }

    return tmp;
}

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

void touch_dev_set_touch_event_callback(const touch_dev_t *dev, touch_event_cb_t cb, void *arg)
{
    assert(dev);
    dev->driver->set_event_callback(dev, cb, arg);
}
