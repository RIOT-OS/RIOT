/*
 * Copyright (C) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_uc8151d
 *
 * @{
 * @file
 * @brief       Default configuration for the uc8151d
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef UC8151D_PARAMS_H
#define UC8151D_PARAMS_H

#include "board.h"
#include "uc8151d.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UC8151D_DISPLAY_X
/**
 * @brief Width of the display in pixels.
 */
#define UC8151D_DISPLAY_X        (200)
#endif

#ifndef UC8151D_DISPLAY_Y
/**
 * @brief Height of the display in pixels.
 */
#define UC8151D_DISPLAY_Y        (200)
#endif

#ifndef UC8151D_PARAM_SPI
/**
 * @brief SPI device the display is connected to.
 */
#define UC8151D_PARAM_SPI        (SPI_DEV(0))
#endif

#ifndef UC8151D_PARAM_SPI_CLK
/**
 * @brief SPI device clock speed.
 */
#define UC8151D_PARAM_SPI_CLK    (SPI_CLK_5MHZ)
#endif

/**
 * @brief SPI Chip select pin.
 */
#ifndef UC8151D_PARAM_CS
#define UC8151D_PARAM_CS         (SPI_CS_UNDEF)
#endif

/**
 * @brief Data/command pin of the display.
 */
#ifndef UC8151D_PARAM_DC
#define UC8151D_PARAM_DC         (GPIO_UNDEF)
#endif

/**
 * @brief Reset pin of the display.
 */
#ifndef UC8151D_PARAM_RST
#define UC8151D_PARAM_RST        (GPIO_UNDEF)
#endif

#ifndef UC8151D_PARAM_BUSY
/**
 * @brief Busy pin of the display.
 */
#define UC8151D_PARAM_BUSY       (GPIO_UNDEF)
#endif

#ifndef UC8151D_PARAMS
/**
 * @brief Parameters to initialize the display with.
 */
#define UC8151D_PARAMS           { .spi = UC8151D_PARAM_SPI, \
                                   .spi_clk = UC8151D_PARAM_SPI_CLK, \
                                   .cs_pin = UC8151D_PARAM_CS, \
                                   .dc_pin = UC8151D_PARAM_DC, \
                                   .rst_pin = UC8151D_PARAM_RST, \
                                   .busy_pin = UC8151D_PARAM_BUSY, \
                                   .size_x = UC8151D_DISPLAY_X, \
                                   .size_y = UC8151D_DISPLAY_Y, \
}
#endif

/**
 * @brief   Configuration struct
 */
static const uc8151d_params_t uc8151d_params[] =
{
    UC8151D_PARAMS,
};

#ifdef __cplusplus
}
#endif

#endif /* UC8151D_PARAMS_H */
/** @} */
