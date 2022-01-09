/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       initializes ft5x06 touch panel device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <stddef.h>

#include "log.h"

#include "touch_dev.h"

#include "ft5x06.h"
#include "ft5x06_params.h"
#include "ft5x06_touch_dev.h"

#define FT5X06_NUMOF           ARRAY_SIZE(ft5x06_params)

ft5x06_t ft5x06_devs[FT5X06_NUMOF];
static touch_dev_reg_t touch_dev_entries[FT5X06_NUMOF];

void auto_init_ft5x06(void)
{
    assert(FT5X06_NUMOF == ARRAY_SIZE(ft5x06_screen_ids));

    for (size_t i = 0; i < FT5X06_NUMOF; i++) {
        LOG_DEBUG("[auto_init_screen] initializing ft5x06 #%u\n", i);
        if (ft5x06_init(&ft5x06_devs[i], &ft5x06_params[i], NULL, NULL) < 0) {
            LOG_ERROR("[auto_init_screen] error initializing ft5x06 #%u\n", i);
            continue;
        }

        touch_dev_entries[i].dev = (touch_dev_t *)&ft5x06_devs[i];
        touch_dev_entries[i].screen_id = ft5x06_screen_ids[i];
        touch_dev_entries[i].dev->driver = &ft5x06_touch_dev_driver;

        /* add to touch_dev registry */
        touch_dev_reg_add(&(touch_dev_entries[i]));
    }
}
