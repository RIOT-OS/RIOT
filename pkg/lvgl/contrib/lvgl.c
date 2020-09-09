/*
 * Copyright (C) 2019-2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_lvgl
 * @{
 *
 * @file
 * @brief       LittlevGL glue code
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>

#include "thread.h"

#include "xtimer.h"
#include "log.h"

#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include "lvgl_riot.h"

#include "screen_dev.h"

#ifndef LVGL_TASK_THREAD_PRIO
#define LVGL_TASK_THREAD_PRIO       (THREAD_PRIORITY_MAIN + 1)
#endif

#ifndef LVGL_COLOR_BUF_SIZE
#define LVGL_COLOR_BUF_SIZE         (LV_HOR_RES_MAX * 5)
#endif

#ifndef LVGL_INACTIVITY_PERIOD_MS
#define LVGL_INACTIVITY_PERIOD_MS   (1 * MS_PER_SEC)    /* 1s */
#endif

#ifndef LVGL_TASK_HANDLER_DELAY_US
#define LVGL_TASK_HANDLER_DELAY_US  (5 * US_PER_MS)     /* 5ms */
#endif

#ifndef LVGL_THREAD_FLAG
#define LVGL_THREAD_FLAG            (1 << 7)
#endif

static char _task_thread_stack[THREAD_STACKSIZE_LARGE];
static kernel_pid_t _task_thread_pid;

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LVGL_COLOR_BUF_SIZE];

static screen_dev_t *_screen_dev = NULL;


static void *_task_thread(void *arg)
{
    (void)arg;

    while (1) {
        /* Normal operation (no sleep) in < LVGL_INACTIVITY_PERIOD_MS msec
           inactivity */
        if (lv_disp_get_inactive_time(NULL) < LVGL_INACTIVITY_PERIOD_MS) {
            lv_task_handler();
        }
        else {
            /* Block after LVGL_ACTIVITY_PERIOD msec inactivity */
            thread_flags_wait_one(LVGL_THREAD_FLAG);

            /* trigger an activity so the task handler is called on the next loop */
            lv_disp_trig_activity(NULL);
        }

        xtimer_usleep(LVGL_TASK_HANDLER_DELAY_US);
    }

    return NULL;
}

static void _disp_map(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    if (!_screen_dev->display) {
        return;
    }

    disp_dev_map(_screen_dev->display, area->x1, area->x2, area->y1, area->y2,
                 (const uint16_t *)color_p);

    LOG_DEBUG("[lvgl] flush display\n");

    lv_disp_flush_ready(drv);
}

#ifdef MODULE_TOUCH_DEV
/* adapted from https://github.com/lvgl/lvgl/tree/v6.1.2#add-littlevgl-to-your-project */
static bool _touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    if (!_screen_dev->touch) {
        return false;
    }

    (void)indev_driver;
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    touch_t positions[1];
    uint8_t touches = touch_dev_touches(_screen_dev->touch, positions, 1);

    /* Save the state and save the pressed coordinates */
    data->state = (touches > 0) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    if (data->state == LV_INDEV_STATE_PR) {
        last_x = positions[0].x;
        last_y = positions[0].y;
    }

    /* Set the coordinates (if released use the last pressed coordinates) */
    data->point.x = last_x;
    data->point.y = last_y;

    return false;
}
#endif

void lvgl_init(screen_dev_t *screen_dev)
{
    lv_init();
    _screen_dev = screen_dev;
    assert(screen_dev->display);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /* Configure horizontal and vertical resolutions based on the
       underlying display device parameters */
    disp_drv.hor_res = disp_dev_width(screen_dev->display);
    disp_drv.ver_res = disp_dev_height(screen_dev->display);

    disp_drv.flush_cb = _disp_map;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);
    lv_disp_buf_init(&disp_buf, buf, NULL, LVGL_COLOR_BUF_SIZE);

#ifdef MODULE_TOUCH_DEV
    assert(screen_dev->touch);
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = _touch_read;
    lv_indev_drv_register(&indev_drv);
#endif

    lv_task_handler();
    _task_thread_pid = thread_create(_task_thread_stack, sizeof(_task_thread_stack),
                                     LVGL_TASK_THREAD_PRIO, THREAD_CREATE_STACKTEST,
                                     _task_thread, NULL, "_task_thread");
}

void lvgl_wakeup(void)
{
    thread_t *tcb = (thread_t *)sched_threads[_task_thread_pid];
    thread_flags_set(tcb, LVGL_THREAD_FLAG);
}
