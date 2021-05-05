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

#if IS_USED(MODULE_ILI9341)
#include "ili9341.h"
#include "ili9341_params.h"
#endif

#if IS_USED(MODULE_ST7735)
#include "st7735.h"
#include "st7735_params.h"
#endif

#ifndef ST7735_NUMOF
#define ST7735_NUMOF                0
#endif
#ifndef ST7735_SCREEN_NUMOF
#define ST7735_SCREEN_NUMOF         0
#endif

#ifndef ILI9341_NUMOF
#define ILI9341_NUMOF               0
#endif
#ifndef ILI9341_SCREEN_NUMOF
#define ILI9341_SCREEN_NUMOF        0
#endif

#define LCD_NUMOF           (ILI9341_NUMOF + ST7735_NUMOF)
#define LCD_SCREEN_NUMOF    (ILI9341_SCREEN_NUMOF + ST7735_SCREEN_NUMOF)

#if IS_USED(MODULE_ILI9341)
static ili9341_t ili9341_devs[ILI9341_NUMOF];
#endif
#if IS_USED(MODULE_ST7735)
static st7735_t st7735_devs[ILI9341_NUMOF];
#endif

static disp_dev_reg_t disp_dev_entries[LCD_NUMOF];

void auto_init_lcd(void)
{
    assert(LCD_NUMOF == LCD_SCREEN_NUMOF);

#if IS_USED(MODULE_ILI9341)
    for (size_t i = 0; i < ILI9341_NUMOF; i++) {
        ili9341_devs[i].dev.driver = &lcd_ili9341_driver;
        LOG_DEBUG("[auto_init_screen] initializing lcd #%u\n", i);
        if (lcd_init(&ili9341_devs[i].dev, &ili9341_params[i]) < 0) {
            LOG_ERROR("[auto_init_screen] error initializing lcd #%u\n", i);
            continue;
        }

        disp_dev_entries[i].dev = (disp_dev_t *) &ili9341_devs[i].dev;
        disp_dev_entries[i].screen_id = ili9341_screen_ids[i];
        disp_dev_entries[i].dev->driver = &lcd_disp_dev_driver;

        /* add to disp_dev registry */
        disp_dev_reg_add(&(disp_dev_entries[i]));
    }
#endif

#if IS_USED(MODULE_ST7735)
    for (size_t i = 0; i < ST7735_NUMOF; i++) {
        st7735_devs[i].dev.driver = &lcd_st7735_driver;
        LOG_DEBUG("[auto_init_screen] initializing lcd #%u\n", i);
        if (lcd_init(&st7735_devs[i].dev, (lcd_params_t*) &st7735_params[i].params) < 0) {
            LOG_ERROR("[auto_init_screen] error initializing lcd #%u\n", i);
            continue;
        }

        disp_dev_entries[i + ILI9341_NUMOF].dev = (disp_dev_t *) &st7735_devs[i].dev;
        disp_dev_entries[i + ILI9341_NUMOF].screen_id = st7735_screen_ids[i];
        disp_dev_entries[i + ILI9341_NUMOF].dev->driver = &lcd_disp_dev_driver;

        /* add to disp_dev registry */
        disp_dev_reg_add(&(disp_dev_entries[i + ILI9341_NUMOF]));
    }
#endif
}
