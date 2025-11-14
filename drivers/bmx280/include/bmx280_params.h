/*
 * SPDX-FileCopyrightText: 2016 Kees Bakker, SODAQ
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_bmx280
 *
 * @{
 * @file
 * @brief       Default configuration for BMX280
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "board.h"
#include "bmx280.h"
#include "kernel_defines.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the BMX280
 * @{
 */
#ifdef BMX280_USE_SPI
/* SPI configuration */
#ifndef BMX280_PARAM_SPI
#define BMX280_PARAM_SPI            SPI_DEV(0)
#endif
#ifndef BMX280_PARAM_CLK
#define BMX280_PARAM_CLK            SPI_CLK_5MHZ
#endif
#ifndef BMX280_PARAM_CS
#define BMX280_PARAM_CS             GPIO_PIN(0, 0)
#endif
#else
/* I2C configuration */
#ifndef BMX280_PARAM_I2C_DEV
#define BMX280_PARAM_I2C_DEV        I2C_DEV(0)
#endif
#ifndef BMX280_PARAM_I2C_ADDR
#define BMX280_PARAM_I2C_ADDR       (0x77)
#endif
#endif

#define BMX280_PARAM_MISC                   \
        .t_sb = BMX280_SB_0_5,              \
        .filter = BMX280_FILTER_OFF,        \
        .run_mode = BMX280_MODE_FORCED,     \
        .temp_oversample = BMX280_OSRS_X1,  \
        .press_oversample = BMX280_OSRS_X1, \
        .humid_oversample = BMX280_OSRS_X1, \

/* Defaults for Weather Monitoring */
#ifndef BMX280_PARAMS
#ifdef BMX280_USE_SPI
#define BMX280_PARAMS                       \
    {                                       \
        .spi = BMX280_PARAM_SPI,            \
        .clk = BMX280_PARAM_CLK,            \
        .cs  = BMX280_PARAM_CS,             \
        BMX280_PARAM_MISC                   \
    }
#else
#define BMX280_PARAMS                       \
    {                                       \
        .i2c_dev  = BMX280_PARAM_I2C_DEV,   \
        .i2c_addr = BMX280_PARAM_I2C_ADDR,  \
        BMX280_PARAM_MISC                   \
    }
#endif
#endif
/**@}*/

/**
 * @brief   Configure BMX280
 */
static const bmx280_params_t bmx280_params[] =
{
    BMX280_PARAMS
};

/**
 * @brief   The number of configured sensors
 */
#define BMX280_NUMOF    ARRAY_SIZE(bmx280_params)

/**
 * @brief   Configuration details of SAUL registry entries
 *
 * This two dimensional array contains static details of the sensors
 * for each device. Please be awar that the indexes are used in
 * auto_init_bmx280, so make sure the indexes match.
 */
static const saul_reg_info_t bmx280_saul_reg_info[BMX280_NUMOF] =
{
#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
        { .name = "bme280" }
#else
        { .name = "bmp280" }
#endif
};

#ifdef __cplusplus
}
#endif

/** @} */
