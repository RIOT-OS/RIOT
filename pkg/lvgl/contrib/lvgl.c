/*
 * SPDX-FileCopyrightText: 2019-2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_lvgl
 * @{
 *
 * @file
 * @brief       LVGL glue code
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>

#include "kernel_defines.h"
#include "thread.h"

#include "timex.h"
#include "ztimer.h"
#include "log.h"

#include "lvgl/lvgl.h"
#include "lvgl_riot.h"

#include "screen_dev.h"

/* Size of the I1 palette placed by LVGL at the beginning of the draw buffer */
#define LVGL_I1_PALETTE_SIZE                (8)

/* Size of the draw buffer in bytes */
#ifndef LVGL_COLOR_BUF_SIZE
#  if IS_USED(MODULE_U8G2_DISP_DEV) || IS_USED(MODULE_EPD_BW_SPI)
/* Monochrome displays are refreshed in full: the buffer must contain the whole
 * screen, with one bit per pixel */
#    define LVGL_COLOR_BUF_SIZE     (LVGL_I1_PALETTE_SIZE + \
                                     ((LV_HOR_RES_MAX + 7) / 8) * LV_VER_RES_MAX)
#  else
#    define LVGL_COLOR_BUF_SIZE     (LV_HOR_RES_MAX * 10 * sizeof(lv_color16_t))
#  endif
#endif

#ifndef CONFIG_LVGL_INACTIVITY_PERIOD_MS
#define CONFIG_LVGL_INACTIVITY_PERIOD_MS    (5 * MS_PER_SEC)    /* 5s */
#endif

#ifndef CONFIG_LVGL_TASK_HANDLER_DELAY_MS
#define CONFIG_LVGL_TASK_HANDLER_DELAY_MS   (5)                 /* 5ms */
#endif

#ifndef LVGL_THREAD_FLAG
#define LVGL_THREAD_FLAG                    (1 << 7)
#endif

static kernel_pid_t _task_thread_pid;

#if !IS_USED(MODULE_LVGL_SDL)
static LV_ATTRIBUTE_MEM_ALIGN uint8_t _draw_buf[LVGL_COLOR_BUF_SIZE];

static screen_dev_t *_screen_dev = NULL;

/* LVGL renders 1-bit pixels with the most significant bit first while the
 * RIOT monochrome display drivers expect the least significant bit first */
static void _monochrome_1bit_convert(uint8_t *buf, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        uint8_t byte = buf[i];
        byte = ((byte & 0xf0) >> 4) | ((byte & 0x0f) << 4);
        byte = ((byte & 0xcc) >> 2) | ((byte & 0x33) << 2);
        byte = ((byte & 0xaa) >> 1) | ((byte & 0x55) << 1);
        buf[i] = byte;
    }
}

static void _disp_map(lv_display_t *display, const lv_area_t *area, uint8_t *px_map)
{
    if (!_screen_dev->display) {
        return;
    }

    if (!area) {
        return;
    }

    const disp_dev_area_t disp_area = {
        area->x1, area->x2, area->y1, area->y2
    };

    if (lv_display_get_color_format(display) == LV_COLOR_FORMAT_I1) {
        px_map += LVGL_I1_PALETTE_SIZE;
        size_t stride = (lv_area_get_width(area) + 7) / 8;
        _monochrome_1bit_convert(px_map, stride * lv_area_get_height(area));
    }
    else if (IS_USED(MODULE_LCD)) {
        /* The LCD driver expects RGB565 pixels in big endian order */
        lv_draw_rgb565_swap(px_map, lv_area_get_size(area));
    }

    disp_dev_map(_screen_dev->display, &disp_area, (const uint16_t *)px_map);

    LOG_DEBUG("[lvgl] flush display\n");

    lv_display_flush_ready(display);
}
#endif

#if IS_USED(MODULE_TOUCH_DEV) && !IS_USED(MODULE_LVGL_SDL)
static void _touch_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (!_screen_dev->touch) {
        return;
    }

    if (!data) {
        return;
    }

    (void)indev;
    static int32_t last_x = 0;
    static int32_t last_y = 0;

    touch_t positions[1];
    uint8_t touches = touch_dev_touches(_screen_dev->touch, positions, 1);

    /* Save the state and save the pressed coordinates */
    data->state = (touches > 0) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    if (data->state == LV_INDEV_STATE_PRESSED) {
        last_x = positions[0].x;
        last_y = positions[0].y;
    }

    /* Set the coordinates (if released use the last pressed coordinates) */
    data->point.x = last_x;
    data->point.y = last_y;

    return;
}
#endif

static uint32_t _tick_get(void)
{
    return ztimer_now(ZTIMER_MSEC);
}

void lvgl_init(screen_dev_t *screen_dev)
{
    lv_init();
    lv_tick_set_cb(_tick_get);

#if IS_USED(MODULE_LVGL_SDL)
    (void)screen_dev;

    /* Use SDL driver which creates window on PC's monitor to simulate a display */
    lv_sdl_window_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);
#if IS_USED(MODULE_TOUCH_DEV)
    /* Add the mouse as input device, use SDL driver which reads the PC's mouse */
    lv_sdl_mouse_create();
#endif
#else
    _screen_dev = screen_dev;
    assert(screen_dev->display);

    /* Configure horizontal and vertical resolutions based on the
       underlying display device parameters */
    lv_display_t *display = lv_display_create(disp_dev_width(screen_dev->display),
                                              disp_dev_height(screen_dev->display));
    lv_display_set_flush_cb(display, _disp_map);

    if (disp_dev_color_depth(screen_dev->display) == 1) {
        lv_display_set_color_format(display, LV_COLOR_FORMAT_I1);
        lv_display_set_buffers(display, _draw_buf, NULL, sizeof(_draw_buf),
                               LV_DISPLAY_RENDER_MODE_FULL);
    }
    else {
        lv_display_set_buffers(display, _draw_buf, NULL, sizeof(_draw_buf),
                               LV_DISPLAY_RENDER_MODE_PARTIAL);
    }

#if IS_USED(MODULE_TOUCH_DEV)
    if (screen_dev->touch) {
        lv_indev_t *indev = lv_indev_create();
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, _touch_read);
    }
#endif
#endif
}

void lvgl_run(void)
{
    _task_thread_pid = thread_getpid();

    while (1) {
        /* Normal operation (no sleep) in < CONFIG_LVGL_INACTIVITY_PERIOD_MS msec
           inactivity */
        if (lv_display_get_inactive_time(NULL) < CONFIG_LVGL_INACTIVITY_PERIOD_MS) {
            lv_timer_handler();
        }
        else {
            /* Block after LVGL_ACTIVITY_PERIOD msec inactivity */
            thread_flags_wait_one(LVGL_THREAD_FLAG);
            /* trigger an activity so the task handler is called on the next loop */
            lv_display_trigger_activity(NULL);
        }

        ztimer_sleep(ZTIMER_MSEC, CONFIG_LVGL_TASK_HANDLER_DELAY_MS);
    }
}

void lvgl_wakeup(void)
{
    thread_t *tcb = thread_get(_task_thread_pid);

    thread_flags_set(tcb, LVGL_THREAD_FLAG);
}
