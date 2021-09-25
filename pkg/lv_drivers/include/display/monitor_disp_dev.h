/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_lvgl_drivers_monitor LVGL SDL display driver
 * @ingroup     pkg_lvgl
 * @{
 *
 * @file
 * @brief       Definition of the driver for the disp_dev generic interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef DISPLAY_MONITOR_DISP_DEV_H
#define DISPLAY_MONITOR_DISP_DEV_H

#include "disp_dev.h"
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default screen identifiers
 */
#ifndef MONITOR_PARAM_SCREEN_ID
#define MONITOR_PARAM_SCREEN_ID     0
#endif

/**
 * @brief   Device descriptor for a monitor
 */
typedef struct {
    disp_dev_t *dev;            /**< Pointer to the generic display device */
    lv_disp_drv_t* disp_drv;    /**< pointer to the display driver */
} monitor_t;

/**
 * @brief   Reference to the display device driver struct
 */
extern const disp_dev_driver_t monitor_disp_dev_driver;

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_MONITOR_DISP_DEV_H */
/** @} **/
