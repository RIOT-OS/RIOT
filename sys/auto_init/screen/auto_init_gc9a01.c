/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "gc9a01.h"
#include "gc9a01_params.h"

#ifndef GC9A01_NUMOF
#  define GC9A01_NUMOF             0
#endif
#ifndef GC9A01_SCREEN_NUMOF
#  define GC9A01_SCREEN_NUMOF      0
#endif

static gc9a01_t gc9a01_devs[GC9A01_NUMOF];

static disp_dev_reg_t disp_dev_entries[GC9A01_NUMOF];

void auto_init_gc9a01(void)
{
    assert(GC9A01_NUMOF == GC9A01_SCREEN_NUMOF);

    for (size_t i = 0; i < GC9A01_NUMOF; i++) {
        gc9a01_devs[i].dev.driver = &lcd_gc9a01_driver;
        LOG_DEBUG("[auto_init_screen] initializing gc9a01 #%u\n", i);
        if (lcd_init(&gc9a01_devs[i].dev, &gc9a01_params[i]) < 0) {
            LOG_ERROR("[auto_init_screen] error initializing gc9a01 #%u\n", i);
            continue;
        }

        disp_dev_entries[i].dev = (disp_dev_t *) &gc9a01_devs[i].dev;
        disp_dev_entries[i].screen_id = gc9a01_screen_ids[i];
        disp_dev_entries[i].dev->driver = &lcd_disp_dev_driver;

        /* add to disp_dev registry */
        disp_dev_reg_add(&(disp_dev_entries[i]));
    }
}
