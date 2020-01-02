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
 * @brief       Definition for the LittlevGL engine initialization
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LVGL_RIOT_H
#define LVGL_RIOT_H

#include "disp_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the lvgl display engine
 *
 * @param[in] dev   Pointer to the generic display device
 */
void lvgl_init(disp_dev_t *dev);

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
