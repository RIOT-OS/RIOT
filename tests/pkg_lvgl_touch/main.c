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

#include "disp_dev.h"


static void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    (void)btn;
    if (event == LV_EVENT_CLICKED) {
        puts("Button clicked!");
    }
}

int main(void)
{
    /* Enable backlight */
    disp_dev_backlight_on();

    /* Add a button to the current screen */
    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);

    /* Set the button position and size */
    lv_coord_t x_size = 100;
    lv_coord_t y_size = 50;
    lv_coord_t x_pos = (lv_obj_get_width(lv_scr_act()) - x_size) / 2;
    lv_coord_t y_pos = (lv_obj_get_height(lv_scr_act()) - y_size) / 2;
    lv_obj_set_pos(btn, x_pos, y_pos);
    lv_obj_set_size(btn, 100, 50);

    /*Assign a callback to the button*/
    lv_obj_set_event_cb(btn, btn_event_cb);

    /* Add a label to the button */
    lv_obj_t * label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Click me");

    lvgl_start();

    return 0;
}
