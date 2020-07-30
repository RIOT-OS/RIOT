/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       LittlevGL example application with clickable button
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "lvgl/lvgl.h"
#include "lvgl_riot.h"

#include "ili9341.h"
#include "ili9341_params.h"
#include "disp_dev.h"
#include "ili9341_disp_dev.h"

#include "stmpe811.h"
#include "stmpe811_params.h"
#include "touch_dev.h"
#include "stmpe811_touch_dev.h"

#include "screen_dev.h"

static screen_dev_t s_screen;
static ili9341_t s_disp_dev;
static stmpe811_t s_touch_dev;

static void _stmpe811_event_cb(void *arg)
{
    (void)arg;
    lvgl_wakeup();
}

static void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    (void)btn;
    if (event == LV_EVENT_CLICKED) {
        puts("Button clicked!");
    }
}

int main(void)
{
    /* Configure the generic display driver interface */
    s_screen.display = (disp_dev_t *)&s_disp_dev;
    s_screen.display->driver = &ili9341_disp_dev_driver;

    /* Initialize the concrete display driver */
    ili9341_init(&s_disp_dev, &ili9341_params[0]);

    /* Configure the generic touch driver interface */
    s_screen.touch = (touch_dev_t *)&s_touch_dev;
    s_screen.touch->driver = &stmpe811_touch_dev_driver;

    /* Initialize the concrete touch driver */
    stmpe811_init(&s_touch_dev, &stmpe811_params[0], _stmpe811_event_cb, NULL);

    /* Initialize lvgl with the generic screen */
    lvgl_init(&s_screen);

    /* Add a button to the current screen */
    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);

    /* Set the button position and size */
    lv_coord_t x_size = 100;
    lv_coord_t y_size = 50;
    lv_coord_t x_pos = (disp_dev_width(s_screen.display) - x_size) / 2;
    lv_coord_t y_pos = (disp_dev_height(s_screen.display) - y_size) / 2;
    lv_obj_set_pos(btn, x_pos, y_pos);
    lv_obj_set_size(btn, 100, 50);

    /*Assign a callback to the button*/
    lv_obj_set_event_cb(btn, btn_event_cb);

    /* Add a label to the button */
    lv_obj_t * label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Click me");

    return 0;
}
