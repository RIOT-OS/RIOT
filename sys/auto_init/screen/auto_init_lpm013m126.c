/*
 * Copyright (C) 2020 Inria
 * Copyright (C) 2022 Digitaluhr Manufaktur
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       initializes lpm013m126 display device
 *
 * @author      Nicole Faerber <nicole.faerber@digitaluhr-manufaktur.de>
 * @}
 */

#include <assert.h>
#include <stddef.h>

#include "log.h"

#include "disp_dev.h"

#include "lpm013m126.h"
#include "lpm013m126_params.h"
#include "lpm013m126_disp_dev.h"

#define LPM013M126_NUMOF           ARRAY_SIZE(lpm013m126_params)

static lpm013m126_t lpm013m126_devs[LPM013M126_NUMOF];
static disp_dev_reg_t disp_dev_entries[LPM013M126_NUMOF];

void auto_init_lpm013m126(void)
{
    assert(LPM013M126_NUMOF == ARRAY_SIZE(lpm013m126_screen_ids));

    for (size_t i = 0; i < LPM013M126_NUMOF; i++) {
        LOG_DEBUG("[auto_init_screen] initializing lpm013m126 #%u\n", i);
        if (lpm013m126_init(&lpm013m126_devs[i], &lpm013m126_params[i]) < 0) {
            LOG_ERROR("[auto_init_screen] error initializing lpm013m126 #%u\n", i);
            continue;
        }

        disp_dev_entries[i].dev = (disp_dev_t *)&lpm013m126_devs[i];
        disp_dev_entries[i].screen_id = lpm013m126_screen_ids[i];
        disp_dev_entries[i].dev->driver = &lpm013m126_disp_dev_driver;

        /* add to disp_dev registry */
        disp_dev_reg_add(&(disp_dev_entries[i]));
    }
}
