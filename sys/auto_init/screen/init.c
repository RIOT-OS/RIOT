/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       automatically initializes screen display and touch devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <stdio.h>

#define ENABLE_DEBUG    0
#include "debug.h"

void auto_init_screen(void)
{
    if (IS_USED(MODULE_DISP_DEV)) {
        DEBUG("auto_init_screen: init display drivers\n");
        if (IS_USED(MODULE_ILI9341)) {
            extern void auto_init_ili9341(void);
            auto_init_ili9341();
        }
        if (IS_USED(MODULE_PERIPH_LTDC)) {
            extern void auto_init_periph_ltdc(void);
            auto_init_periph_ltdc();
        }
    }

    if (IS_USED(MODULE_TOUCH_DEV)) {
        DEBUG("auto_init_screen: init touch drivers\n");
        if (IS_USED(MODULE_STMPE811)) {
            extern void auto_init_stmpe811(void);
            auto_init_stmpe811();
        }
        if (IS_USED(MODULE_FT5X06)) {
            extern void auto_init_ft5x06(void);
            auto_init_ft5x06();
        }
    }

    if (IS_USED(MODULE_LVGL)) {
        extern void auto_init_lvgl(void);
        auto_init_lvgl();
    }
}
