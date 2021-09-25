/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_lvgl_drivers_mouse LVGL mouse driver
 * @ingroup     pkg_lvgl
 * @{
 *
 * @file
 * @brief       Definition of the driver for the touch_dev generic interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef INDEV_MOUSE_TOUCH_DEV_H
#define INDEV_MOUSE_TOUCH_DEV_H

#include "touch_dev.h"
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default screen identifiers
 */
#ifndef MOUSE_PARAM_SCREEN_ID
#define MOUSE_PARAM_SCREEN_ID       0
#endif

/**
 * @brief   Reference to the touch device driver struct
 */
extern const touch_dev_driver_t mouse_touch_dev_driver;

#ifdef __cplusplus
}
#endif

#endif /* INDEV_MOUSE_TOUCH_DEV_H */
/** @} **/
