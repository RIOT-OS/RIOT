/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       initializes cst816s display device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <stddef.h>

#include "log.h"

#include "touch_dev.h"

#include "cst816s.h"
#include "cst816s_params.h"
#include "cst816s_touch_dev.h"

cst816s_t cst816s_devs[CST816S_NUMOF];
static touch_dev_reg_t touch_dev_entries[CST816S_NUMOF];

void auto_init_cst816s(void)
{
    assert(CST816S_NUMOF == ARRAY_SIZE(cst816s_screen_ids));

    for (size_t i = 0; i < CST816S_NUMOF; i++) {
        LOG_DEBUG("[auto_init_screen] initializing cst816s #%u\n", i);
        if (cst816s_init(&cst816s_devs[i], &cst816s_params[i], NULL, NULL) < 0) {
            LOG_ERROR("[auto_init_screen] error initializing cst816s #%u\n", i);
            continue;
        }

        touch_dev_entries[i].dev = (touch_dev_t *)&cst816s_devs[i];
        touch_dev_entries[i].screen_id = cst816s_screen_ids[i];
        touch_dev_entries[i].dev->driver = &cst816s_touch_dev_driver;

        /* add to touch_dev registry */
        touch_dev_reg_add(&(touch_dev_entries[i]));
    }
}
