/*
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       initializes display devices controlled by the u8g2 library
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include <assert.h>
#include <stddef.h>

#include "log.h"

#include "disp_dev.h"

#include "u8g2_display.h"
#include "u8g2_display_disp_dev.h"
#include "u8g2_display_params.h"

#ifndef U8G2_DISPLAY_NUMOF
#  define U8G2_DISPLAY_NUMOF                1
#endif

static u8g2_display_t u8g2_display_devs[U8G2_DISPLAY_NUMOF];

static disp_dev_reg_t disp_dev_entries[U8G2_DISPLAY_NUMOF];

void auto_init_u8g2_display(void)
{
    LOG_DEBUG("[auto_init_screen] initializing u8g2 displays\n");

    for (size_t i = 0; i < U8G2_DISPLAY_NUMOF; i++) {
        (void)disp_dev_entries;
        (void)u8g2_display_devs;

        LOG_DEBUG("[auto_init_screen] initializing u8g2 monochrome display #%u\n", i);
        if (u8g2_display_init(&u8g2_display_devs[i], &u8g2_display_params[i]) < 0) {
            LOG_ERROR("[auto_init_screen] error initializing u8g2 monochrome display #%u\n", i);
            continue;
        }

        disp_dev_entries[i].dev = (disp_dev_t *)&u8g2_display_devs[i].dev;
        disp_dev_entries[i].screen_id = u8g2_display_screen_ids[i];
        disp_dev_entries[i].dev->driver = &u8g2_display_disp_dev_driver;

        /* add to disp_dev registry */
        disp_dev_reg_add(&(disp_dev_entries[i]));
    }
}
