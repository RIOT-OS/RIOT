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
 * @brief       initializes lvgl high level GUI api
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include "log.h"
#include "kernel_defines.h"

#include "lvgl_riot.h"

#include "screen_dev.h"
#include "disp_dev.h"

#if IS_USED(MODULE_TOUCH_DEV)
#include "touch_dev.h"
#endif

static screen_dev_t s_screen;
extern disp_dev_reg_t *disp_dev_reg;

#if IS_USED(MODULE_TOUCH_DEV)
static void _touch_event_callback(void *arg)
{
    (void)arg;
    lvgl_wakeup();
}
#endif

#ifndef CONFIG_LVGL_SCREEN_DEFAULT
#define CONFIG_LVGL_SCREEN_DEFAULT  0   /**< Default screen ID used by LVGL */
#endif

void auto_init_lvgl(void)
{
    LOG_DEBUG("[auto_init_screen] initializing lvgl\n");

    /* Only a single screen is supported by lvgl */
#if !IS_USED(MODULE_LV_DRIVERS_SDL)
    disp_dev_reg_t *disp_dev = disp_dev_reg_find_screen(CONFIG_LVGL_SCREEN_DEFAULT);
    s_screen.display = disp_dev->dev;
#endif

#if IS_USED(MODULE_TOUCH_DEV)
    touch_dev_reg_t *touch_dev = touch_dev_reg_find_screen(CONFIG_LVGL_SCREEN_DEFAULT);
    if (touch_dev) {
        s_screen.touch = touch_dev->dev;
        touch_dev_set_touch_event_callback(touch_dev->dev, _touch_event_callback, NULL);
    }
#endif

    /* Initialize lvgl with the generic screen */
    lvgl_init(&s_screen);
}
