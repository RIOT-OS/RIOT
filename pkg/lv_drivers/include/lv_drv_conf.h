/*
 * SPDX-FileCopyrightText: 2022 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_lv_drivers
 * @brief       LVGL Drivers SDL2 configuration headers
 *
 * @note        Based on upstream lv_drv_conf_template.h.
 * @{
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "kernel_defines.h"
#include "lvgl_riot_conf.h"

/*********************
*  DISPLAY DRIVERS
*********************/

/*-------------------
 *  SDL
 *-------------------*/

/**
 * @brief   Internal flag to enable LVGL SDL2 based drivers for display, mouse,
 *          mousewheel and keyboard
 */
#define USE_SDL IS_USED(MODULE_LV_DRIVERS_SDL)
/**
 * @brief   LVGL SDL2 display horizontal resolution
 */
#ifndef SDL_HOR_RES
#define SDL_HOR_RES                 320
#endif
/**
 * @brief   LVGL SDL2 display vertical resolution
 */
#ifndef SDL_VER_RES
#define SDL_VER_RES                 240
#endif
/**
 * @brief   LVGL display horizontal resolution
 */
#define LV_HOR_RES                  SDL_HOR_RES
/**
 * @brief   LVGL display vertical resolution
 */
#define LV_VER_RES                  SDL_VER_RES
/**
 * @brief   Enable Scale window by this factor (useful when simulating small screens)
 */
#ifndef SDL_ZOOM
#define SDL_ZOOM                    1
#endif
/**
 * @brief   Used to test true double buffering with only address changing.
 *          Use 2 draw buffers, both with SDL_HOR_RES x SDL_VER_RES size
 */
#define SDL_DOUBLE_BUFFERED         0
/**
 * @brief   Open two windows to test multi display support
 */
#define SDL_DUAL_DISPLAY            0

#ifdef __cplusplus
}
#endif

/** @} */
