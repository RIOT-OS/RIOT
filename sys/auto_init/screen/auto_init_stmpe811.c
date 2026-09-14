/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       initializes stmpe811 display device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <stddef.h>

#include "log.h"

#include "touch_dev.h"

#include "stmpe811.h"
#include "stmpe811_params.h"
#include "stmpe811_touch_dev.h"

#define STMPE811_NUMOF           ARRAY_SIZE(stmpe811_params)

stmpe811_t stmpe811_devs[STMPE811_NUMOF];
static touch_dev_reg_t touch_dev_entries[STMPE811_NUMOF];

void auto_init_stmpe811(void)
{
    assert(STMPE811_NUMOF == ARRAY_SIZE(stmpe811_screen_ids));

    for (size_t i = 0; i < STMPE811_NUMOF; i++) {
        LOG_DEBUG("[auto_init_screen] initializing stmpe811 #%u\n", i);
        if (stmpe811_init(&stmpe811_devs[i], &stmpe811_params[i], NULL, NULL) < 0) {
            LOG_ERROR("[auto_init_screen] error initializing stmpe811 #%u\n", i);
            continue;
        }

        touch_dev_entries[i].dev = (touch_dev_t *)&stmpe811_devs[i];
        touch_dev_entries[i].screen_id = stmpe811_screen_ids[i];
        touch_dev_entries[i].dev->driver = &stmpe811_touch_dev_driver;

        /* add to touch_dev registry */
        touch_dev_reg_add(&(touch_dev_entries[i]));
    }
}
