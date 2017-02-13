/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bme280
 *
 * @{
 * @file
 * @brief       Default configuration for BME280
 *
 * @author      Kees Bakker <kees@sodaq.com>
 */

#ifndef BME280_PARAMS_H
#define BME280_PARAMS_H

#include "bme280.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the BME280
 * @{
 */
#ifndef BME280_PARAM_I2C_DEV
#define BME280_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef BME280_PARAM_I2C_ADDR
#define BME280_PARAM_I2C_ADDR        (0x77)
#endif

/* Defaults for Weather Monitoring */
#define BME280_PARAMS_DEFAULT              \
    {                                      \
        .i2c_dev = BME280_PARAM_I2C_DEV,   \
        .i2c_addr = BME280_PARAM_I2C_ADDR, \
        .t_sb = BME280_SB_0_5,             \
        .filter = BME280_FILTER_OFF,       \
        .run_mode = BME280_MODE_FORCED,     \
        .temp_oversample = BME280_OSRS_X1,  \
        .press_oversample = BME280_OSRS_X1, \
        .humid_oversample = BME280_OSRS_X1, \
    }
/**@}*/

/**
 * @brief   Configure BME280
 */
static const bme280_params_t bme280_params[] =
{
#ifdef BME280_PARAMS_BOARD
    BME280_PARAMS_BOARD,
#else
    BME280_PARAMS_DEFAULT,
#endif
};

/**
 * @brief   The number of configured sensors
 */
#define BME280_NUMOF    (sizeof(bme280_params) / sizeof(bme280_params[0]))

/**
 * @brief   Configuration details of SAUL registry entries
 *
 * This two dimensional array contains static details of the sensors
 * for each device. Please be awar that the indexes are used in
 * auto_init_bme280, so make sure the indexes match.
 */
static const saul_reg_info_t bme280_saul_reg_info[BME280_NUMOF][3] =
{
    {
        { .name = "bme280-temp" },
        { .name = "bme280-humidity" },
        { .name = "bme280-press" },
    },
};

#ifdef __cplusplus
}
#endif

#endif /* BME280_PARAMS_H */
/** @} */
