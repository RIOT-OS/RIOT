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
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the BME680
 * @{
 */

#if MODULE_PERIPH_I2C || DOXYGEN
#ifndef BME680_PARAM_I2C_DEV
#define BME680_PARAM_I2C_DEV        (I2C_DEV(1))
#endif

#ifndef BME680_PARAM_I2C_ADDR
#define BME680_PARAM_I2C_ADDR       (BME680_I2C_ADDR_2)
#endif
#endif /* MODULE_PERIPH_I2C */

//#if MODULE_PERIPH_SPI || DOXYGEN
#ifndef BME680_PARAM_SPI_DEV
#define BME680_PARAM_SPI_DEV        (SPI_DEV(0))
#endif

#ifndef BME680_PARAM_SPI_NSS_PIN
#define BME680_PARAM_SPI_NSS_PIN    (GPIO_PIN(0,4))
#endif
//#endif /* MODULE_PERIPH_SPI */

/**
 * @brief   Defaults I2C parameters if none provided
 */

#define BME680_PARAMS_I2C                                                       \
{                                                                               \
                                .ambient_temp = 25,                             \
                                .temp_os = OVERSAMPLING_8,                      \
                                .press_os = OVERSAMPLING_16,                    \
                                .hum_os = OVERSAMPLING_2,                       \
                                .meas_gas = true,                               \
                                .gas_heating_time = 320,                        \
                                .gas_heating_temp = 150,                        \
                                .filter = FILTER_COEFFICIENT_3,                 \
                                .intf.i2c.dev    = BME680_PARAM_I2C_DEV,        \
                                .intf.i2c.addr   = BME680_PARAM_I2C_ADDR,       \
                                }

#define BME680_PARAMS_SPI                                                       \
{                                                                               \
                                .ambient_temp = 25,                             \
                                .temp_os = OVERSAMPLING_8,                      \
                                .press_os = OVERSAMPLING_8,                    \
                                .hum_os = OVERSAMPLING_2,                       \
                                .meas_gas = true,                               \
                                .gas_heating_time = 320,                        \
                                .gas_heating_temp = 100,                        \
                                .filter = FILTER_COEFFICIENT_3,                 \
                                .intf.spi.dev    = BME680_PARAM_SPI_DEV,        \
                                .intf.spi.nss_pin   = BME680_PARAM_SPI_NSS_PIN, \
                                }

/**
 * @brief   Default SAUL meta information
 */
#ifndef BME680_SAUL_INFO
#if MODULE_BME680_I2C && MODULE_BME680_SPI
#define BME680_SAUL_INFO    { .name = "bme680:0" },                             \
                            { .name = "bme680:1" },
#else /* MODULE_BME680_I2C && MODULE_BME680_SPI */
#define BME680_SAUL_INFO    { .name = "bme680" }
#endif /* MODULE_BME680_I2C && MODULE_BME680_SPI */
#endif /* BME680_SAUL_INFO */
/**@}*/

/**
 * @brief   Configure params for BME680
 */
static const bme680_params_t bme680_params[] =
{
#if MODULE_BME680_I2C || DOXYGEN
    BME680_PARAMS_I2C,
#endif
#if MODULE_BME680_SPI || DOXYGEN
    BME680_PARAMS_SPI
#endif
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t bme680_saul_info[] =
{
    BME680_SAUL_INFO
};

/**
 * @brief   The number of configured sensors
 */
#define BME680_NUMOF    ARRAY_SIZE(bme680_params)


#ifdef __cplusplus
}
#endif

#endif /* BME680_PARAMS_H */
/** @} */
