/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       initializes mouse touch device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <stddef.h>

#include "log.h"

#include "touch_dev.h"

#include "indev/mouse.h"
#include "indev/mouse_touch_dev.h"

static touch_dev_reg_t touch_dev_entry;
typedef struct {
    touch_dev_t *dev;            /**< Pointer to the generic toudch dev */
} mouse_t;
static mouse_t _mouse;

void auto_init_mouse(void)
{
    LOG_DEBUG("[auto_init_screen] initializing SDL mouse device\n");
    mouse_init();

    touch_dev_entry.dev = (touch_dev_t *)&_mouse;
    touch_dev_entry.screen_id = MOUSE_PARAM_SCREEN_ID;
    touch_dev_entry.dev->driver = &mouse_touch_dev_driver;

    /* add to touch_dev registry */
    touch_dev_reg_add(&(touch_dev_entry));
}
