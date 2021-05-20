/*
 * Copyright (C) 2019 Inria
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
 * @brief       LittlevGL example application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "xtimer.h"

#include "lvgl/lvgl.h"
#include "lvgl_riot.h"
#include "disp_dev.h"

#define CPU_LABEL_COLOR     "FF0000"
#define MEM_LABEL_COLOR     "0000FF"
#define CHART_POINT_NUM     100

/* Must be lower than LVGL_INACTIVITY_PERIOD_MS for autorefresh */
#define REFR_TIME           200

static lv_obj_t *win;
static lv_obj_t *chart;
static lv_chart_series_t * cpu_ser;
static lv_chart_series_t *mem_ser;
static lv_obj_t *info_label;
static lv_task_t *refr_task;

static void sysmon_task(lv_task_t *param)
{
    (void)param;

    /* Get CPU and memory information */
    uint8_t cpu_busy = 100 - lv_task_get_idle();

    lv_mem_monitor_t mem_mon;
    lv_mem_monitor(&mem_mon);

    uint8_t mem_used_pct = mem_mon.used_pct;

    /* Add the CPU and memory data to the chart */
    lv_chart_set_next(chart, cpu_ser, cpu_busy);
    lv_chart_set_next(chart, mem_ser, mem_used_pct);

    /* Set the text info */
    lv_label_set_text_fmt(info_label,
                          "%s%s CPU: %d %%%s\n\n"
                          LV_TXT_COLOR_CMD"%s MEMORY: %d %%"LV_TXT_COLOR_CMD"\n"
                          "Total: %d bytes\n"
                          "Used: %d bytes\n"
                          "Free: %d bytes\n"
                          "Frag: %d %%",
                          LV_TXT_COLOR_CMD,
                          CPU_LABEL_COLOR,
                          cpu_busy,
                          LV_TXT_COLOR_CMD,
                          MEM_LABEL_COLOR,
                          mem_used_pct,
                          (int)mem_mon.total_size,
                          (int)mem_mon.total_size - mem_mon.free_size,
                          mem_mon.free_size, mem_mon.frag_pct);

    /* Force a wakeup of lvgl when each task is called: this ensures an activity
       is triggered and wakes up lvgl during the next LVGL_INACTIVITY_PERIOD ms */
    lvgl_wakeup();
}

void sysmon_create(void)
{
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    win = lv_win_create(lv_disp_get_scr_act(NULL), NULL);
    lv_win_set_title(win, "System monitor");

    /* Make the window content responsive */
    lv_win_set_layout(win, LV_LAYOUT_PRETTY_MID);

    /* Create a chart with two data lines */
    chart = lv_chart_create(win, NULL);
    lv_obj_set_size(chart, hres / 2.5, vres / 2);
    lv_obj_set_pos(chart, LV_DPI / 10, LV_DPI / 10);
    lv_chart_set_point_count(chart, CHART_POINT_NUM);
    lv_chart_set_range(chart, 0, 100);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    cpu_ser = lv_chart_add_series(chart, LV_COLOR_RED);
    mem_ser = lv_chart_add_series(chart, LV_COLOR_BLUE);

    /* Set the data series to zero */
    uint16_t i;
    for(i = 0; i < CHART_POINT_NUM; i++) {
        lv_chart_set_next(chart, cpu_ser, 0);
        lv_chart_set_next(chart, mem_ser, 0);
    }

    /* Create a label for the details of Memory and CPU usage */
    info_label = lv_label_create(win, NULL);
    lv_label_set_recolor(info_label, true);

    /* Refresh the chart and label manually at first */
    refr_task = lv_task_create(sysmon_task, REFR_TIME, LV_TASK_PRIO_LOW, NULL);
    sysmon_task(NULL);
}

int main(void)
{
    /* Enable backlight */
    disp_dev_backlight_on();

    /* Create the system monitor widget */
    sysmon_create();

    lvgl_run();

    return 0;
}
