/*
 * Copyright (C) 2021 Inria
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
 * @brief       Definitions specific to RIOT for the LittlevGL engine
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LVGL_RIOT_CONF_H
#define LVGL_RIOT_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Type of coordinates. Should be `int16_t` (or `int32_t` for extreme cases) */
typedef int16_t lv_coord_t;

#define LV_TICK_CUSTOM     1
#if LV_TICK_CUSTOM == 1
#define LV_TICK_CUSTOM_INCLUDE  "ztimer.h"       /*Header for the sys time function*/
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (ztimer_now(ZTIMER_MSEC))     /*Expression evaluating to current systime in ms*/
#endif   /*LV_TICK_CUSTOM*/

#ifdef __cplusplus
}
#endif

#endif /* LVGL_RIOT_CONF_H */
