/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_lv_drivers
 * @brief       LVGL Drivers SDL2 configuration headers
 *
 * @note        Based on upstream lv_drv_conf_template.h.
 * @{
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef LV_DRV_CONF_H
#define LV_DRV_CONF_H

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
#endif /* LV_DRV_CONF_H */
