/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *               2021 Francisco Molina
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_st7735
 *
 * @{
 * @file
 * @brief       Default configuration for st7735
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef ST7735_PARAMS_H
#define ST7735_PARAMS_H

#include "board.h"
#include "lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Default parameters for ST7735 display */
/**
 * @name    Set default configuration parameters for the ST7735
 * @{
 */
#ifndef ST7735_PARAM_SPI
#define ST7735_PARAM_SPI          SPI_DEV(0)        /**< SPI device */
#endif
#ifndef ST7735_PARAM_SPI_CLK
#define ST7735_PARAM_SPI_CLK      SPI_CLK_5MHZ      /**< SPI clock frequency */
#endif
#ifndef ST7735_PARAM_CS
#define ST7735_PARAM_CS           GPIO_PIN(2, 2)    /**< Chip Select pin */
#endif
#ifndef ST7735_PARAM_DCX
#define ST7735_PARAM_DCX          GPIO_PIN(3, 13)   /**< DCX pin */
#endif
#ifndef ST7735_PARAM_RST
#define ST7735_PARAM_RST          GPIO_UNDEF        /**< Reset pin */
#endif
#ifndef ST7735_PARAM_SPI_MODE
#define ST7735_PARAM_SPI_MODE     SPI_MODE_0        /**< SPI mode */
#endif
#ifndef ST7735_PARAM_RGB
#define ST7735_PARAM_RGB          0                 /**< RGB mode enable */
#endif
#ifndef ST7735_PARAM_INVERTED
#define ST7735_PARAM_INVERTED     0                 /**< Inverted mode enable */
#endif
#ifndef ST7735_PARAM_NUM_LINES
#define ST7735_PARAM_NUM_LINES      160U            /**< Number of lines */
#endif
#ifndef ST7735_PARAM_RGB_CHANNELS
#define ST7735_PARAM_RGB_CHANNELS   128U            /**< Number of RGB channels (e.g. columns) */
#endif
#ifndef ST7735_PARAM_ROTATION
#define ST7735_PARAM_ROTATION       ST7735_ROTATION_HORZ    /**< Rotation mode */
#endif
#ifndef ST7735_PARAM_OFFSET_X
#define ST7735_PARAM_OFFSET_X       0               /**< Horizontal offset */
#endif
#ifndef ST7735_PARAM_OFFSET_Y
#define ST7735_PARAM_OFFSET_Y       0               /**< Vertival offset */
#endif

/**
 * @brief   Default params
 */
#ifndef ST7735_PARAMS
#define ST7735_PARAMS              {  .spi = ST7735_PARAM_SPI, \
                                      .spi_clk = ST7735_PARAM_SPI_CLK, \
                                      .spi_mode = ST7735_PARAM_SPI_MODE, \
                                      .cs_pin = ST7735_PARAM_CS, \
                                      .dcx_pin = ST7735_PARAM_DCX, \
                                      .rst_pin = ST7735_PARAM_RST, \
                                      .rgb = ST7735_PARAM_RGB, \
                                      .inverted = ST7735_PARAM_INVERTED, \
                                      .lines = ST7735_PARAM_NUM_LINES, \
                                      .rgb_channels = ST7735_PARAM_RGB_CHANNELS, \
                                      .rotation = ST7735_PARAM_ROTATION, \
                                      .offset_x = ST7735_PARAM_OFFSET_X, \
                                      .offset_y = ST7735_PARAM_OFFSET_Y, \
                                    }
#endif
/** @} */

/**
 * @brief   Default screen identifiers
 */
#ifndef ST7735_PARAM_SCREEN_IDS
#define ST7735_PARAM_SCREEN_IDS    0
#endif

/**
 * @brief   Configure LCD
 */
static const lcd_params_t st7735_params[] =
{
    ST7735_PARAMS,
};

/**
 * @brief   Configure screen identifiers
 */
static const uint8_t st7735_screen_ids[] =
{
    ST7735_PARAM_SCREEN_IDS,
};

/**
 * @brief   Define the number of configured displays
 */
#define ST7735_NUMOF           ARRAY_SIZE(st7735_params)
/**
 * @brief   Define the number screens this display driver is attached to
 */
#define ST7735_SCREEN_NUMOF    ARRAY_SIZE(st7735_screen_ids)

#ifdef __cplusplus
}
#endif

#endif /* ST7735_PARAMS_H */
/** @} */
