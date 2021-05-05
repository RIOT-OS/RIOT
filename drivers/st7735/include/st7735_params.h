/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
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
 */

#ifndef ST7735_PARAMS_H
#define ST7735_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Default parameters for ST7735 display */
/**
 * @name    Set default configuration parameters for the ST7735
 * @{
 */
#ifndef ST7735_PARAM_SPI
#define ST7735_PARAM_SPI          SPI_DEV(0)
#endif
#ifndef ST7735_PARAM_SPI_CLK
#define ST7735_PARAM_SPI_CLK      SPI_CLK_5MHZ
#endif
#ifndef ST7735_PARAM_CS
#define ST7735_PARAM_CS           GPIO_PIN(2, 2)
#endif
#ifndef ST7735_PARAM_DCX
#define ST7735_PARAM_DCX          GPIO_PIN(3, 13)
#endif
#ifndef ST7735_PARAM_RST
#define ST7735_PARAM_RST          GPIO_UNDEF
#endif
#ifndef ST7735_PARAM_SPI_MODE
#define ST7735_PARAM_SPI_MODE     SPI_MODE_0
#endif
#ifndef ST7735_PARAM_RGB
#define ST7735_PARAM_RGB          0
#endif
#ifndef ST7735_PARAM_INVERTED
#define ST7735_PARAM_INVERTED     0
#endif
#ifndef ST7735_PARAM_NUM_LINES
#define ST7735_PARAM_NUM_LINES      160U
#endif
#ifndef ST7735_PARAM_RGB_CHANNELS
#define ST7735_PARAM_RGB_CHANNELS   128U
#endif
#ifndef ST7735_OFFSET_X
#define ST7735_OFFSET_X             0
#endif
#ifndef ST7735_OFFSET_Y
#define ST7735_OFFSET_Y             0
#endif

#ifndef ST7735_PARAMS
#define ST7735_PARAMS              {  .params.spi = ST7735_PARAM_SPI, \
                                      .params.spi_clk = ST7735_PARAM_SPI_CLK, \
                                      .params.spi_mode = ST7735_PARAM_SPI_MODE, \
                                      .params.cs_pin = ST7735_PARAM_CS, \
                                      .params.dcx_pin = ST7735_PARAM_DCX, \
                                      .params.rst_pin = ST7735_PARAM_RST, \
                                      .params.rgb = ST7735_PARAM_RGB, \
                                      .params.inverted = ST7735_PARAM_INVERTED, \
                                      .params.lines = ST7735_PARAM_NUM_LINES, \
                                      .params.rgb_channels = ST7735_PARAM_RGB_CHANNELS, \
                                      .offset_x = ST7735_OFFSET_X, \
                                      .offset_y = ST7735_OFFSET_Y, \
                                    }
#endif
/**@}*/

/**
 * @brief   Default screen identifiers
 */
#ifndef ST7735_PARAM_SCREEN_IDS
#define ST7735_PARAM_SCREEN_IDS    1
#endif

/**
 * @brief   Configure LCD
 */
static const st7735_params_t st7735_params[] =
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
#define ST7735_SCREEN_NUMOF    ARRAY_SIZE(st7735_screen_ids)

#ifdef __cplusplus
}
#endif

#endif /* ST7735_PARAMS_H */
