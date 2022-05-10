/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lcd
 * @{
 *
 * @file
 * @brief       Driver adaption to disp_dev generic interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "lcd.h"
#include "lcd_disp_dev.h"

#ifndef LCD_DISP_COLOR_DEPTH
#define LCD_DISP_COLOR_DEPTH    (16U)
#endif

static void _lcd_map(const disp_dev_t *dev, const disp_dev_area_t *area, const uint16_t *color)
{
    lcd_t *lcd = (lcd_t *)dev;
    lcd_pixmap(lcd, area->x1, area->x2, area->y1, area->y2, color);
}

static uint16_t _lcd_height(const disp_dev_t *disp_dev)
{
    const lcd_t *dev = (lcd_t *)disp_dev;
    assert(dev);

    return dev->params->rgb_channels;
}

static uint16_t _lcd_width(const disp_dev_t *disp_dev)
{
    const lcd_t *dev = (lcd_t *)disp_dev;
    assert(dev);

    return dev->params->lines;
}

static uint8_t _lcd_color_depth(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return LCD_DISP_COLOR_DEPTH;
}

static void _lcd_set_invert(const disp_dev_t *disp_dev, bool invert)
{
    const lcd_t *dev = (lcd_t *)disp_dev;

    assert(dev);

    if (invert) {
        lcd_invert_on(dev);
    }
    else {
        lcd_invert_off(dev);
    }
}

const disp_dev_driver_t lcd_disp_dev_driver = {
    .map            = _lcd_map,
    .height         = _lcd_height,
    .width          = _lcd_width,
    .color_depth    = _lcd_color_depth,
    .set_invert     = _lcd_set_invert,
};
