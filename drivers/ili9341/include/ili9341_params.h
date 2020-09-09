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
#define ILI9341_PARAM_NUM_LINES    320U
#endif

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
                                    }
#endif
/**@}*/

/**
 * @brief   Configure ILI9341
 */
static const ili9341_params_t ili9341_params[] =
{
    ILI9341_PARAMS,
};

#ifdef __cplusplus
}
#endif

#endif /* ILI9341_PARAMS_H */
