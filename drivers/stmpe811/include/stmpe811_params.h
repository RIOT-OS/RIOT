/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_stmpe811
 *
 * @{
 * @file
 * @brief       Default configuration for STMPE811
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef STMPE811_PARAMS_H
#define STMPE811_PARAMS_H

#include "kernel_defines.h"
#include "board.h"
#include "stmpe811.h"
#include "stmpe811_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the STMPE811
 * @{
 *
 * These default values are adapted for the @ref boards_stm32f429i-disc1 board
 */
#if IS_USED(MODULE_STMPE811_SPI)
/* SPI configuration */
#ifndef STMPE811_PARAM_SPI_DEV
#define STMPE811_PARAM_SPI_DEV        SPI_DEV(0)
#endif
#ifndef STMPE811_PARAM_CLK
#define STMPE811_PARAM_CLK            SPI_CLK_1MHZ
#endif
#ifndef STMPE811_PARAM_CS
#define STMPE811_PARAM_CS             GPIO_PIN(0, 0)
#endif
#else
/* I2C configuration */
#ifndef STMPE811_PARAM_I2C_DEV
#define STMPE811_PARAM_I2C_DEV          I2C_DEV(0)
#endif
#ifndef STMPE811_PARAM_ADDR
#define STMPE811_PARAM_ADDR             (STMPE811_I2C_ADDR_DEFAULT)
#endif
#endif

#ifndef STMPE811_PARAM_INT_PIN
#define STMPE811_PARAM_INT_PIN          GPIO_PIN(0, 15)
#endif
#ifndef STMPE811_PARAM_XMAX
#define STMPE811_PARAM_XMAX             (240U)
#endif
#ifndef STMPE811_PARAM_YMAX
#define STMPE811_PARAM_YMAX             (320U)
#endif

#ifndef STMPE811_PARAMS
#if IS_USED(MODULE_STMPE811_SPI)
#define STMPE811_PARAMS                { .spi = STMPE811_PARAM_SPI_DEV,     \
                                         .clk = STMPE811_PARAM_CLK,         \
                                         .cs = STMPE811_PARAM_CS,           \
                                         .int_pin = STMPE811_PARAM_INT_PIN, \
                                         .xmax = STMPE811_PARAM_XMAX,       \
                                         .ymax = STMPE811_PARAM_YMAX,       \
                                         }
#else
#define STMPE811_PARAMS                { .i2c = STMPE811_PARAM_I2C_DEV,     \
                                         .addr = STMPE811_PARAM_ADDR,       \
                                         .int_pin = STMPE811_PARAM_INT_PIN, \
                                         .xmax = STMPE811_PARAM_XMAX,       \
                                         .ymax = STMPE811_PARAM_YMAX,       \
                                         }
#endif
#endif
/**@}*/

/**
 * @brief   Configure STMPE811
 */
static const stmpe811_params_t stmpe811_params[] =
{
    STMPE811_PARAMS
};

/**
 * @brief   Default screen identifiers
 */
#ifndef STMPE811_PARAM_SCREEN_IDS
#define STMPE811_PARAM_SCREEN_IDS       0
#endif

/**
 * @brief   Configure screen identifiers
 */
static const uint8_t stmpe811_screen_ids[] =
{
    STMPE811_PARAM_SCREEN_IDS,
};

#ifdef __cplusplus
}
#endif

#endif /* STMPE811_PARAMS_H */
/** @} */
