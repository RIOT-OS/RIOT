/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 * Copyright (C) 2022 Nicole Faerber <nicole.faerber@digitaluhr-manufaktur.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lpm013m126
 *
 * @{
 * @file
 * @brief       Default configuration for lpm013m126
 *
 * @author      Nicole Faerber <nicole.faerber@digitaluhr-manufaktur.de>
 */

#ifndef LPM013M126_PARAMS_H
#define LPM013M126_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Default parameters for LPM013M126 display */
/**
 * @name    Set default configuration parameters for the LPM013M126
 * @{
 */
#ifndef LPM013M126_PARAM_SPI
#define LPM013M126_PARAM_SPI          SPI_DEV(0)
#endif
#ifndef LPM013M126_PARAM_SPI_CLK
#define LPM013M126_PARAM_SPI_CLK      SPI_CLK_5MHZ
#endif
#ifndef LPM013M126_PARAM_CS
#define LPM013M126_PARAM_CS           GPIO_PIN(0, 5)
#endif
#ifndef LPM013M126_PARAM_EXTCOM
#define LPM013M126_PARAM_EXTCOM       GPIO_PIN(0, 6)
#endif
#ifndef LPM013M126_PARAM_DISP
#define LPM013M126_PARAM_DISP         GPIO_PIN(0, 7)
#endif
#ifndef LPM013M126_PARAM_SPI_MODE
#define LPM013M126_PARAM_SPI_MODE     SPI_MODE_0
#endif
#ifndef LPM013M126_PARAM_INVERTED
#define LPM013M126_PARAM_INVERTED     0
#endif

#ifndef LPM013M126_PARAM_NUM_LINES
#define LPM013M126_PARAM_NUM_LINES    176U
#endif


#ifndef LPM013M126_PARAMS
#define LPM013M126_PARAMS              { .spi = LPM013M126_PARAM_SPI, \
                                      .spi_clk = LPM013M126_PARAM_SPI_CLK, \
                                      .spi_mode = LPM013M126_PARAM_SPI_MODE, \
                                      .cs_pin = LPM013M126_PARAM_CS, \
                                      .extcom_pin = LPM013M126_PARAM_EXTCOM, \
                                      .disp_pin = LPM013M126_PARAM_DISP, \
                                      .inverted = LPM013M126_PARAM_INVERTED, \
                                      .lines = LPM013M126_PARAM_NUM_LINES, \
                                      }
#endif
/**@}*/

/**
 * @brief   Configure LPM013M126
 */
static const lpm013m126_params_t lpm013m126_params[] =
{
    LPM013M126_PARAMS,
};

/**
 * @brief   Default screen identifiers
 */
#ifndef LPM013M126_PARAM_SCREEN_IDS
#define LPM013M126_PARAM_SCREEN_IDS    0
#endif

/**
 * @brief   Configure screen identifiers
 */
static const uint8_t lpm013m126_screen_ids[] =
{
    LPM013M126_PARAM_SCREEN_IDS,
};

#ifdef __cplusplus
}
#endif

#endif /* LPM013M126_PARAMS_H */
