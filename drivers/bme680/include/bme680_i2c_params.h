/*
 * Copyright (C) 2019 Mesotic SAS
 *               2020 Gunar Schorcht
 *               2020 OVGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bme680
 *
 * @{
 * @file
 * @brief       Default configuration for BME680 device driver
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Jana Eisoldt <jana.eisoldt@ovgu.de>
 */

#ifndef BME680_I2C_PARAMS_H
#define BME680_I2C_PARAMS_H

#include "board.h"
#include "bme680_i2c.h"
#include "bme680_common.h"
#include "bme680_internals.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the BME680
 * @{
 */

#ifndef BME680_PARAM_I2C_DEV
#define BME680_PARAM_I2C_DEV        (I2C_DEV(1))
#endif

#ifndef BME680_PARAM_I2C_ADDR
#define BME680_PARAM_I2C_ADDR       (BME680_I2C_ADDR_2)
#endif

/**
 * @brief   Defaults I2C parameters if none provided
 */

#define BME680_PARAMS_I2C                                                      \
{                                                                              \
        .common_params.ambient_temp               = 25,                        \
        .common_params.temp_os                    = OVERSAMPLING_8,            \
        .common_params.press_os                   = OVERSAMPLING_4,            \
        .common_params.hum_os                     = OVERSAMPLING_2,            \
        .common_params.meas_gas                   = true,                      \
        .common_params.gas_heating_time           = 320,                       \
        .common_params.gas_heating_temp           = 150,                       \
        .common_params.filter                     = FILTER_COEFFICIENT_3,      \
        .i2c_params.dev                           = BME680_PARAM_I2C_DEV,      \
        .i2c_params.addr                          = BME680_PARAM_I2C_ADDR,     \
}

/**
 * @brief   Default SAUL meta information
 */
#ifndef BME680_I2C_SAUL_INFO
#define BME680_I2C_SAUL_INFO    { .name = "bme680i2c" }
#endif /* BME680_SAUL_INFO */
/**@}*/


/**
 * @brief   Configure params for BME680
 */

static const bme680_i2c_params_t bme680_i2c_params[] =
{
    BME680_PARAMS_I2C
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t bme680_i2c_saul_info[] =
{
    BME680_I2C_SAUL_INFO
};

#ifndef BME680_I2C_NUMOF
#define BME680_I2C_NUMOF    ARRAY_SIZE(bme680_i2c_params)
#endif

#ifdef __cplusplus
}
#endif

#endif /* BME680_PARAMS_H */
/** @} */
