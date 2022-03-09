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
 * @brief       initializes ili9341 display device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>
#include <stddef.h>

#include "log.h"

#include "disp_dev.h"

#include "ili9341.h"
#include "ili9341_params.h"
#include "ili9341_disp_dev.h"

#define ILI9341_NUMOF           ARRAY_SIZE(ili9341_params)

static ili9341_t ili9341_devs[ILI9341_NUMOF];
static disp_dev_reg_t disp_dev_entries[ILI9341_NUMOF];

void auto_init_ili9341(void)
{
    assert(ILI9341_NUMOF == ARRAY_SIZE(ili9341_screen_ids));

    for (size_t i = 0; i < ILI9341_NUMOF; i++) {
        LOG_DEBUG("[auto_init_screen] initializing ili9341 #%u\n", i);
        if (ili9341_init(&ili9341_devs[i], &ili9341_params[i]) < 0) {
            LOG_ERROR("[auto_init_screen] error initializing ili9341 #%u\n", i);
            continue;
        }

        disp_dev_entries[i].dev = (disp_dev_t *)&ili9341_devs[i];
        disp_dev_entries[i].screen_id = ili9341_screen_ids[i];
        disp_dev_entries[i].dev->driver = &ili9341_disp_dev_driver;

        /* add to disp_dev registry */
        disp_dev_reg_add(&(disp_dev_entries[i]));
    }
}
