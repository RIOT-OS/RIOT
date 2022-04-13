/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ili9341
 *
 * @{
 * @file
 * @brief       Default configuration for ili9341
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef ILI9341_PARAMS_H
#define ILI9341_PARAMS_H

#include "board.h"
#include "lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Default parameters for ILI9341 display */
/**
 * @name    Set default configuration parameters for the ILI9341
 * @{
 */
#ifndef ILI9341_PARAM_SPI
#define ILI9341_PARAM_SPI          SPI_DEV(0)
#endif
#ifndef ILI9341_PARAM_SPI_CLK
#define ILI9341_PARAM_SPI_CLK      SPI_CLK_5MHZ
#endif
#ifndef ILI9341_PARAM_CS
#define ILI9341_PARAM_CS           GPIO_PIN(2, 2)
#endif
#ifndef ILI9341_PARAM_DCX
#define ILI9341_PARAM_DCX          GPIO_PIN(3, 13)
#endif
#ifndef ILI9341_PARAM_RST
#define ILI9341_PARAM_RST          GPIO_UNDEF
#endif
#ifndef ILI9341_PARAM_SPI_MODE
#define ILI9341_PARAM_SPI_MODE     SPI_MODE_0
#endif
#ifndef ILI9341_PARAM_RGB
#define ILI9341_PARAM_RGB          0
#endif
#ifndef ILI9341_PARAM_INVERTED
#define ILI9341_PARAM_INVERTED     0
#endif
#ifndef ILI9341_PARAM_NUM_LINES
#define ILI9341_PARAM_NUM_LINES         320U    /**< Number of lines */
#endif
#ifndef ILI9341_PARAM_RGB_CHANNELS
#define ILI9341_PARAM_RGB_CHANNELS      240U    /**< Number of RGB channels (e.g. columns) */
#endif
#ifndef ILI9341_PARAM_ROTATION
#define ILI9341_PARAM_ROTATION          ILI9341_ROTATION_HORZ_FLIP
#endif
#ifndef ILI9341_PARAM_OFFSET_X
#define ILI9341_PARAM_OFFSET_X          0               /**< Horizontal offset */
#endif
#ifndef ILI9341_PARAM_OFFSET_Y
#define ILI9341_PARAM_OFFSET_Y          0               /**< Vertival offset */
#endif

/**
 * @brief   Default params
 */
#ifndef ILI9341_PARAMS
#define ILI9341_PARAMS              { .spi = ILI9341_PARAM_SPI, \
                                      .spi_clk = ILI9341_PARAM_SPI_CLK, \
                                      .spi_mode = ILI9341_PARAM_SPI_MODE, \
                                      .cs_pin = ILI9341_PARAM_CS, \
                                      .dcx_pin = ILI9341_PARAM_DCX, \
                                      .rst_pin = ILI9341_PARAM_RST, \
                                      .rgb = ILI9341_PARAM_RGB, \
                                      .inverted = ILI9341_PARAM_INVERTED, \
                                      .lines = ILI9341_PARAM_NUM_LINES, \
                                      .rgb_channels = ILI9341_PARAM_RGB_CHANNELS, \
                                      .rotation = ILI9341_PARAM_ROTATION, \
                                      .offset_x = ILI9341_PARAM_OFFSET_X, \
                                      .offset_y = ILI9341_PARAM_OFFSET_Y, \
}
#endif
/** @} */

/**
 * @brief   Configure ILI9341
 */
static const lcd_params_t ili9341_params[] =
{
    ILI9341_PARAMS,
};

/**
 * @brief   Default screen identifiers
 */
#ifndef ILI9341_PARAM_SCREEN_IDS
#define ILI9341_PARAM_SCREEN_IDS    0
#endif

/**
 * @brief   Configure screen identifiers
 */
static const uint8_t ili9341_screen_ids[] =
{
    ILI9341_PARAM_SCREEN_IDS,
};

/**
 * @brief   Define the number of configured displays
 */
#define ILI9341_NUMOF           ARRAY_SIZE(ili9341_params)
/**
 * @brief   Define the number screens this display driver is attached to
 */
#define ILI9341_SCREEN_NUMOF    ARRAY_SIZE(ili9341_screen_ids)


#ifdef __cplusplus
}
#endif

#endif /* ILI9341_PARAMS_H */
/** @} */
