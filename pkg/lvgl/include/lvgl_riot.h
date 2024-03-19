/*
 * Copyright (C) 2019 Inria
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
 * @brief       Definition for the LVGL engine initialization
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LVGL_RIOT_H
#define LVGL_RIOT_H

#include "screen_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the lvgl display engine
 *
 * @param[in] screen_dev   Pointer to the generic screen device
 */
void lvgl_init(screen_dev_t *screen_dev);

/**
 * @brief   Run the lvgl task handler
 *
 * In order to run the lvgl internal task handler in an endless loop, this
 * function must be called manually either from the main thread or from a
 * custom thread.
 * In case of CONFIG_LVGL_INACTIVITY_PERIOD_MS ms of inactivity, the loop stops
 * the thread running the lvgl task handler until @ref lvgl_wakeup is called.
 */
void lvgl_run(void);

/**
 * @brief   Wakeup lvgl when inactive
 *
 * This function unblocks the lvgl task handler thread and will indirectly
 * trigger an activity. After calling this function, lvgl remains awake during
 * the next LVGL_INACTIVITY_PERIOD_MS ms.
 */
void lvgl_wakeup(void);

#ifdef __cplusplus
}
#endif

#endif /* LVGL_RIOT_H */
/** @} */
