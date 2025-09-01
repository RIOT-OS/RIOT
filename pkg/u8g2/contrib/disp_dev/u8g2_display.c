/*
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     u8g2_disp_dev
 * @{
 *
 * @file
 * @brief       Mapping of u8g2 to disp_dev interface
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */
#include <assert.h>
#include <stdio.h>

#include "u8g2.h"
#include "u8g2_display.h"
#include "u8g2_display_params.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Color depth of monochrome displays
 */
#define U8G2_DISP_DEV_COLOR_DEPTH    (1U)

int u8g2_display_init(u8g2_display_t *dev, const u8g2_display_params_t *params)
{
    assert(dev != NULL);
    assert(params != NULL);

    dev->params = *params;

    u8g2_init_function_t init = dev->params.init_function;
    if (IS_USED(MODULE_PERIPH_SPI) && dev->params.i2c_address == 0) {
        DEBUG("u8g2_display_init: Initializing SPI display\n");
        init(&dev->u8g2, U8G2_R0, u8x8_byte_hw_spi_riotos, u8x8_gpio_and_delay_riotos);
    }
    else if (IS_USED(MODULE_PERIPH_I2C) && dev->params.i2c_address != 0) {
        DEBUG("u8g2_display_init: Initializing I2C display with addr: 0x%02x\n",
              dev->params.i2c_address);
        init(&dev->u8g2, U8G2_R0, u8x8_byte_hw_i2c_riotos, u8x8_gpio_and_delay_riotos);
        u8g2_SetI2CAddress(&dev->u8g2, dev->params.i2c_address);
    }
    else {
        DEBUG("[u8g2_disp_dev]: error: no valid peripheral configuration\n");
        return -1;
    }

    u8g2_SetUserPtr(&dev->u8g2, &dev->params.peripheral_configuration);
    u8g2_InitDisplay(&dev->u8g2);
    u8g2_SetPowerSave(&dev->u8g2, 0);
    u8g2_ClearDisplay(&dev->u8g2);
    u8g2_ClearBuffer(&dev->u8g2);

    return 0;
}

static void _u8g2_display_map(const disp_dev_t *dev, const disp_dev_area_t *area,
                              const uint16_t *color)
{
    assert(dev != NULL);
    assert(area != NULL);
    assert(color != NULL);

    u8g2_display_t *display = (u8g2_display_t *)dev;
    uint16_t width = (area->x2 - area->x1 + 1);
    uint16_t height = area->y2 - area->y1 + 1;
    uint8_t *bitmap = (uint8_t *)color;

    u8g2_DrawXBM(&display->u8g2, area->x1, area->y1, width, height, bitmap);
    u8g2_SendBuffer(&display->u8g2);
}

static uint16_t _u8g2_display_height(const disp_dev_t *disp_dev)
{
    assert(disp_dev != NULL);
    const u8g2_display_t *dev = (u8g2_display_t *)disp_dev;

    return u8g2_GetDisplayHeight(&dev->u8g2);
}

static uint16_t _u8g2_display_width(const disp_dev_t *disp_dev)
{
    assert(disp_dev != NULL);
    const u8g2_display_t *dev = (u8g2_display_t *)disp_dev;

    return u8g2_GetDisplayWidth(&dev->u8g2);
}

static uint8_t _u8g2_display_color_depth(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return U8G2_DISP_DEV_COLOR_DEPTH;
}

static void _u8g2_display_set_invert(const disp_dev_t *disp_dev, bool invert)
{
    (void)disp_dev;
    (void)invert;
}

const disp_dev_driver_t u8g2_display_disp_dev_driver = {
    .map = _u8g2_display_map,
    .height = _u8g2_display_height,
    .width = _u8g2_display_width,
    .color_depth = _u8g2_display_color_depth,
    .set_invert = _u8g2_display_set_invert,
};
