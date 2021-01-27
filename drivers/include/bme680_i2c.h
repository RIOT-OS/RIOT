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
 * @defgroup    drivers_BME680 BME680
 * @ingroup     drivers_sensors
 * @brief       Driver for the Bosch BME680 sensor.
 *
 * @{
 *
 * @file
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Jana Eisoldt <jana.eisoldt@ovgu.de>

 */

#ifndef BME680_I2C_H
#define BME680_I2C_H

#include <stdbool.h>

#include "periph/i2c.h"
#include "bme680_common.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Declare the API of the driver */

/**
 * @brief    I2C address when SDO pin is LOW
 */
#define BME680_I2C_ADDR_1                   (0x76)

/**
 * @brief    I2C address when SDO pin is HIGH
 */
#define BME680_I2C_ADDR_2                   (0x77)

/**
 * @brief   BME680 I2C parameters
 */
typedef struct {
    i2c_t dev;                    /**< I2C device which is used */
    uint8_t addr;                 /**< I2C address */
} bme680_intf_i2c_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    bme680_common_params_t common_params;
    bme680_intf_i2c_t i2c_params;
} bme680_i2c_params_t;

/**
 * @brief   BME680 I2C parameters
 */
typedef struct {
    bme680_common_t common;
    bme680_intf_i2c_t i2c_intf;
} bme680_i2c_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Initialized device descriptor of BME680 device
 * @param[in]    params     Initialization parameters
 *
 * @return                  0 on success
 * @return -EIO             Error in reading/writing data
 */
int bme680_i2c_init(bme680_i2c_t *dev, const bme680_i2c_params_t *params);

/**
 * @brief   Reads temperature, humidity, pressure and gas. Reading gas can be skipped by setting meas_gas = 0
 *
 * @param[inout] dev        Initialized device descriptor of BME680 device
 * @param[out]   data       read temperature (Celsius), humidity (%), pressure (Pa) and gas (Ohm)
 *
 * @return                  0 on success
 * @return -EIO             Error in reading/writing data
 * @return -ERANGE          Error in calculating result data
 */
int bme680_i2c_read(const bme680_i2c_t *dev, bme680_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* BME680_I2C_H */
/** @} */
