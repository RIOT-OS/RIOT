/*
 * Copyright (C) 2018 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup     drivers_bme680
 *
 * @{
 * @file
 * @brief       Default configuration for BME680
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com
 */

#ifndef BME680_PARAMS_H
#define BME680_PARAMS_H

#include "board.h"
#include "bme680.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the BME680
 * @{
 */
#ifndef BME680_PARAM_I2C_DEV
#define BME680_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef BME680_PARAM_I2C_ADDR
#define BME680_PARAM_I2C_ADDR        (0x76)
#endif

/* Defaults for Weather Monitoring */
#define BME680_PARAMS_DEFAULT              \
    {                                      \
        .i2c_dev = BME680_PARAM_I2C_DEV,   \
        .i2c_addr = BME680_PARAM_I2C_ADDR, \
    }
/**@}*/

/**
 * @brief   Configure BME680
 */
static const bme680_params_t bme680_params[] =
{
#ifdef BME680_PARAMS_BOARD
    BME680_PARAMS_BOARD,
#else
    BME680_PARAMS_DEFAULT
#endif
};

/**
 * @brief   The number of configured sensors
 */
#define BME680_NUMOF    (sizeof(bme680_params) / sizeof(bme680_params[0]))

/**
 * @brief   Configuration details of SAUL registry entries
 *
 * This two dimensional array contains static details of the sensors
 * for each device. Please be awar that the indexes are used in
 * auto_init_BME680, so make sure the indexes match.
 */
static const saul_reg_info_t bme680_saul_reg_info[BME680_NUMOF] =
{
        { .name = "bme680" }
};

#ifdef __cplusplus
}
#endif

#endif /* BME680_PARAMS_H */
/** @} */
