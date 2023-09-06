/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *               2021 Francisco Molina
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_st77xx
 *
 * @{
 * @file
 * @brief       Default configuration for ST77xx LCD controllers
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ST77XX_PARAMS_H
#define ST77XX_PARAMS_H

#include "board.h"
#include "lcd.h"
#include "st7735_params.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !DOXYGEN    /* hide from documentation */
/**
 * @name    Mapping of former ST7735 configuration parameters for backward compatibility
 *
 * If a board definition already used the ST7735 driver, `ST7735_*` macros were
 * used in the board definitions to define the default configuration parameter
 * set. For backward compatibility these symbols are mapped to the `ST77XX_*`
 * macros if they are defined.
 *
 * @{
 */
#ifdef ST7735_PARAM_SPI
#define ST77XX_PARAM_SPI            ST7735_PARAM_SPI
#endif
#ifdef ST7735_PARAM_SPI_CLK
#define ST77XX_PARAM_SPI_CLK        ST7735_PARAM_SPI_CLK
#endif
#ifdef ST7735_PARAM_CS
#define ST77XX_PARAM_CS             ST7735_PARAM_CS
#endif
#ifdef ST7735_PARAM_DCX
#define ST77XX_PARAM_DCX            ST7735_PARAM_DCX
#endif
#ifdef ST7735_PARAM_RST
#define ST77XX_PARAM_RST            ST7735_PARAM_RST
#endif
#ifdef ST7735_PARAM_SPI_MODE
#define ST77XX_PARAM_SPI_MODE       ST7735_PARAM_SPI_MODE
#endif
#ifdef ST7735_PARAM_RGB
#define ST77XX_PARAM_RGB            ST7735_PARAM_RGB
#endif
#ifdef ST7735_PARAM_INVERTED
#define ST77XX_PARAM_INVERTED       ST7735_PARAM_INVERTED
#endif
#ifdef ST7735_PARAM_NUM_LINES
#define ST77XX_PARAM_NUM_LINES      ST7735_PARAM_NUM_LINES
#endif
#ifdef ST7735_PARAM_RGB_CHANNELS
#define ST77XX_PARAM_RGB_CHANNELS   ST7735_PARAM_RGB_CHANNELS
#endif
#ifdef ST7735_PARAM_ROTATION
#define ST77XX_PARAM_ROTATION       ST7735_PARAM_ROTATION
#endif
#ifdef ST7735_PARAM_OFFSET_X
#define ST77XX_PARAM_OFFSET_X       ST7735_PARAM_OFFSET_X
#endif
#ifdef ST7735_PARAM_OFFSET_Y
#define ST77XX_PARAM_OFFSET_Y       ST7735_PARAM_OFFSET_Y
#endif
#ifdef ST7735_PARAMS
#define ST77XX_PARAMS               ST7735_PARAMS
#endif
#ifdef ST7735_PARAM_SCREEN_IDS
#define ST77XX_PARAM_SCREEN_IDS     ST7735_PARAM_SCREEN_IDS
#endif

#endif /* !DOXYGEN */

/**
 * @name    Set default configuration parameters for the ST77xx
 * @{
 */
#ifndef ST77XX_PARAM_CNTRL
#define ST77XX_PARAM_CNTRL      ST77XX_CNTRL_ST7735 /**< ST77xx controller variant */
#endif
#ifndef ST77XX_PARAM_SPI
#define ST77XX_PARAM_SPI        SPI_DEV(0)          /**< SPI device */
#endif
#ifndef ST77XX_PARAM_SPI_CLK
#define ST77XX_PARAM_SPI_CLK    SPI_CLK_5MHZ        /**< SPI clock frequency */
#endif
#ifndef ST77XX_PARAM_CS
#define ST77XX_PARAM_CS         GPIO_PIN(2, 2)      /**< Chip Select pin */
#endif
#ifndef ST77XX_PARAM_DCX
#define ST77XX_PARAM_DCX        GPIO_PIN(3, 13)     /**< DCX pin */
#endif
#ifndef ST77XX_PARAM_RST
#define ST77XX_PARAM_RST        GPIO_UNDEF          /**< Reset pin */
#endif
#ifndef ST77XX_PARAM_SPI_MODE
#define ST77XX_PARAM_SPI_MODE   SPI_MODE_0          /**< SPI mode */
#endif
#ifndef ST77XX_PARAM_RGB
#define ST77XX_PARAM_RGB        0                   /**< RGB mode enable */
#endif
#ifndef ST77XX_PARAM_INVERTED
#define ST77XX_PARAM_INVERTED   0                   /**< Inverted mode enable */
#endif
#ifndef ST77XX_PARAM_NUM_LINES
#define ST77XX_PARAM_NUM_LINES      160U            /**< Number of lines */
#endif
#ifndef ST77XX_PARAM_RGB_CHANNELS
#define ST77XX_PARAM_RGB_CHANNELS   128U            /**< Number of RGB channels (e.g. columns) */
#endif
#ifndef ST77XX_PARAM_ROTATION
#define ST77XX_PARAM_ROTATION       ST77XX_ROTATION_HORZ    /**< Rotation mode */
#endif
#ifndef ST77XX_PARAM_OFFSET_X
#define ST77XX_PARAM_OFFSET_X       0               /**< Horizontal offset */
#endif
#ifndef ST77XX_PARAM_OFFSET_Y
#define ST77XX_PARAM_OFFSET_Y       0               /**< Vertival offset */
#endif

/**
 * @brief   Default params
 */
#ifndef ST77XX_PARAMS
#define ST77XX_PARAMS              {  .cntrl = ST77XX_PARAM_CNTRL, \
                                      .spi = ST77XX_PARAM_SPI, \
                                      .spi_clk = ST77XX_PARAM_SPI_CLK, \
                                      .spi_mode = ST77XX_PARAM_SPI_MODE, \
                                      .cs_pin = ST77XX_PARAM_CS, \
                                      .dcx_pin = ST77XX_PARAM_DCX, \
                                      .rst_pin = ST77XX_PARAM_RST, \
                                      .rgb = ST77XX_PARAM_RGB, \
                                      .inverted = ST77XX_PARAM_INVERTED, \
                                      .lines = ST77XX_PARAM_NUM_LINES, \
                                      .rgb_channels = ST77XX_PARAM_RGB_CHANNELS, \
                                      .rotation = ST77XX_PARAM_ROTATION, \
                                      .offset_x = ST77XX_PARAM_OFFSET_X, \
                                      .offset_y = ST77XX_PARAM_OFFSET_Y, \
                                    }
#endif
/** @} */

/**
 * @brief   Default screen identifiers
 */
#ifndef ST77XX_PARAM_SCREEN_IDS
#define ST77XX_PARAM_SCREEN_IDS    0
#endif

/**
 * @brief   Configure LCD
 */
static const lcd_params_t st77xx_params[] =
{
    ST77XX_PARAMS,
};

/**
 * @brief   Configure screen identifiers
 */
static const uint8_t st77xx_screen_ids[] =
{
    ST77XX_PARAM_SCREEN_IDS,
};

/**
 * @brief   Define the number of configured displays
 */
#define ST77XX_NUMOF           ARRAY_SIZE(st77xx_params)
/**
 * @brief   Define the number screens this display driver is attached to
 */
#define ST77XX_SCREEN_NUMOF    ARRAY_SIZE(st77xx_screen_ids)

#ifdef __cplusplus
}
#endif

#endif /* ST77XX_PARAMS_H */
/** @} */
