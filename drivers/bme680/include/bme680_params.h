/*
 * Copyright (C) 2020 OVGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_BME680
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Jana Eisoldt <jana.eisoldt@ovgu.de>
 */

#ifndef BME680_PARAMS_H
#define BME680_PARAMS_H

#include "board.h"
#include "bme680.h"
#include "bme680_internals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */

#ifndef BME680_PARAM_I2C_DEV
#define BME680_PARAM_I2C_DEV         (I2C_DEV(1))
#endif
#ifndef BME680_PARAM_I2C_ADDR
#define BME680_PARAM_I2C_ADDR       (BME680_I2C_ADDR_2)
#endif
#ifndef BME680_PARAMS
#define BME680_PARAMS                                                   \
{                                                                       \
                                .ambient_temp = 25,                     \
                                .temp_oversampling = 2,                 \
                                .pres_oversampling = 16,                \
                                .hum_oversampling = 1,                  \
                                .gas_heating_time = 200,                \
                                .gas_heating_temp = 300,                \
                                .i2c_dev    = BME680_PARAM_I2C_DEV,     \
                                .i2c_addr   = BME680_PARAM_I2C_ADDR,    \
                                }
#endif
/**@}*/

/**
 * @brief   Configure BME680
 */
static const bme680_params_t bme680_params[] =
{
    BME680_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* BME680_PARAMS_H */
/** @} */
