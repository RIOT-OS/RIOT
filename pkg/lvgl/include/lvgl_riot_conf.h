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
 * @brief       Definitions specific to RIOT for the LVGL engine
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LVGL_RIOT_CONF_H
#define LVGL_RIOT_CONF_H

#include "kernel_defines.h"
#include "board.h"
#if IS_USED(MODULE_LV_DRIVERS_SDL)
#include "lv_drv_conf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Maximal horizontal resolution to support by the library.*/
#ifndef LV_HOR_RES_MAX
#ifdef LCD_SCREEN_WIDTH
#define LV_HOR_RES_MAX          (LCD_SCREEN_WIDTH)
#else
#define LV_HOR_RES_MAX          (320)
#endif
#endif

/** Maximal vertical resolution to support by the library.*/
#ifndef LV_VER_RES_MAX
#ifdef LCD_SCREEN_HEIGHT
#define LV_VER_RES_MAX          (LCD_SCREEN_HEIGHT)
#else
#define LV_VER_RES_MAX          (240)
#endif
#endif

/** Will be added where memories needs to be aligned (with -Os data might not be aligned to boundary by default).
 * E.g. __attribute__((aligned(4)))*/
#define LV_ATTRIBUTE_MEM_ALIGN  __attribute__((aligned(4)))

/* Type of coordinates. Should be `int16_t` (or `int32_t` for extreme cases) */
typedef int16_t lv_coord_t;

#define LV_TICK_CUSTOM     1
#define LV_TICK_CUSTOM_INCLUDE  "ztimer.h"       /*Header for the sys time function*/
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (ztimer_now(ZTIMER_MSEC))     /*Expression evaluating to current systime in ms*/

#ifdef __cplusplus
}
#endif

#endif /* LVGL_RIOT_CONF_H */
/** @} */
