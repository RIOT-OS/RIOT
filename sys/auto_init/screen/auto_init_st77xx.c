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

#include "st77xx.h"
#include "st77xx_params.h"

#ifndef ST77XX_NUMOF
#define ST77XX_NUMOF                0
#endif
#ifndef ST77XX_SCREEN_NUMOF
#define ST77XX_SCREEN_NUMOF         0
#endif

static st77xx_t st77xx_devs[ST77XX_NUMOF];

static disp_dev_reg_t disp_dev_entries[ST77XX_NUMOF];

void auto_init_st77xx(void)
{
    assert(ST77XX_NUMOF == ST77XX_SCREEN_NUMOF);

    for (size_t i = 0; i < ST77XX_NUMOF; i++) {
        st77xx_devs[i].dev.driver = &lcd_st77xx_driver;
        LOG_DEBUG("[auto_init_screen] initializing st7735 #%u\n", i);
        if (lcd_init(&st77xx_devs[i].dev, &st77xx_params[i]) < 0) {
            LOG_ERROR("[auto_init_screen] error initializing st7735 #%u\n", i);
            continue;
        }

        disp_dev_entries[i].dev = (disp_dev_t *) &st77xx_devs[i].dev;
        disp_dev_entries[i].screen_id = st77xx_screen_ids[i];
        disp_dev_entries[i].dev->driver = &lcd_disp_dev_driver;

        /* add to disp_dev registry */
        disp_dev_reg_add(&(disp_dev_entries[i]));
    }
}
