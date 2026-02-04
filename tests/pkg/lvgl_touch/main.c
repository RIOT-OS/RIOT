/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       LVGL example application with clickable button
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "lvgl/lvgl.h"
#include "lvgl_riot.h"

#include "disp_dev.h"
#include "random.h"

static lv_obj_t *btn;
static const lv_coord_t x_size = 100;
static const lv_coord_t y_size = 50;

static void btn_event_cb(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_CLICKED) {
        puts("Button clicked!");

        lv_coord_t x_pos = random_uint32_range(0, lv_obj_get_width(lv_scr_act()) - x_size);
        lv_coord_t y_pos = random_uint32_range(0, lv_obj_get_height(lv_scr_act()) - y_size);

        lv_obj_set_pos(btn, x_pos, y_pos);
    }
}

int main(void)
{
    /* Enable backlight */
    disp_dev_backlight_on();

    /* Add a button to the current screen */
    btn = lv_btn_create(lv_scr_act());

    /* Set the button position and size */
    lv_coord_t x_pos = (lv_obj_get_width(lv_scr_act()) - x_size) / 2;
    lv_coord_t y_pos = (lv_obj_get_height(lv_scr_act()) - y_size) / 2;
    lv_obj_set_pos(btn, x_pos, y_pos);
    lv_obj_set_size(btn, 100, 50);

    /*Assign a callback to the button*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);

    /* Add a label to the button */
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "Click me");
    lv_obj_center(label);

    lvgl_run();

    return 0;
}
