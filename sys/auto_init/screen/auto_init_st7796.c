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
 * @brief       initializes lcd display device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>
#include <stddef.h>

#include "log.h"

#include "disp_dev.h"

#include "lcd.h"
#include "lcd_disp_dev.h"

#include "st7796.h"
#include "st7796_params.h"

#ifndef ST7796_NUMOF
#define ST7796_NUMOF                0
#endif
#ifndef ST7796_SCREEN_NUMOF
#define ST7796_SCREEN_NUMOF         0
#endif

static st7796_t st7796_devs[ST7796_NUMOF];

static disp_dev_reg_t disp_dev_entries[ST7796_NUMOF];

void auto_init_st7796(void)
{
    assert(ST7796_NUMOF == ST7796_SCREEN_NUMOF);

    for (size_t i = 0; i < ST7796_NUMOF; i++) {
        st7796_devs[i].dev.driver = &lcd_st7796_driver;
        LOG_DEBUG("[auto_init_screen] initializing st7796 #%u\n", i);
        if (lcd_init(&st7796_devs[i].dev, &st7796_params[i]) < 0) {
            LOG_ERROR("[auto_init_screen] error initializing st7796 #%u\n", i);
            continue;
        }

        disp_dev_entries[i].dev = (disp_dev_t *) &st7796_devs[i].dev;
        disp_dev_entries[i].screen_id = st7796_screen_ids[i];
        disp_dev_entries[i].dev->driver = &lcd_disp_dev_driver;

        /* add to disp_dev registry */
        disp_dev_reg_add(&(disp_dev_entries[i]));
    }
}
