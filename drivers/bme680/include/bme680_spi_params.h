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

#ifndef BME680_PARAMS_H
#define BME680_PARAMS_H

#include "board.h"
#include "bme680_spi.h"
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

#ifndef BME680_PARAM_SPI_DEV
#define BME680_PARAM_SPI_DEV        (SPI_DEV(0))
#endif

#ifndef BME680_PARAM_SPI_NSS_PIN
#define BME680_PARAM_SPI_NSS_PIN    (GPIO_PIN(0,4))
#endif

/**
 * @brief   Defaults SPI parameters if none provided
 */

#define BME680_PARAMS_SPI                                                      \
{                                                                              \
        .common_params.ambient_temp               = 25,                        \
        .common_params.temp_os                    = OVERSAMPLING_8,            \
        .common_params.press_os                   = OVERSAMPLING_8,            \
        .common_params.hum_os                     = OVERSAMPLING_2,            \
        .common_params.meas_gas                   = true,                      \
        .common_params.gas_heating_time           = 320,                       \
        .common_params.gas_heating_temp           = 150,                       \
        .common_params.filter                     = FILTER_COEFFICIENT_3,      \
        .spi_params.dev                           = BME680_PARAM_SPI_DEV,      \
        .spi_params.nss_pin                       = BME680_PARAM_SPI_NSS_PIN,  \
}

/**
 * @brief   Default SAUL meta information
 */

#ifndef BME680_SPI_SAUL_INFO
#define BME680_SPI_SAUL_INFO    { .name = "bme680spi" }
#endif /* BME680_SAUL_INFO */
/**@}*/

/**
 * @brief   Configure params for BME680
 */

static const bme680_spi_params_t bme680_spi_params[] =
{
    BME680_PARAMS_SPI
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t bme680_spi_saul_info[] =
{
    BME680_SPI_SAUL_INFO
};

#define BME680_SPI_NUMOF    ARRAY_SIZE(bme680_spi_params)


#ifdef __cplusplus
}
#endif

#endif /* BME680_PARAMS_H */
/** @} */
