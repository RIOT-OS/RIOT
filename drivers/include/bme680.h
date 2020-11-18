/*
 * Copyright (C) 2020 OVGU Magdeburg
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
 *
 * @author      Jana Eisoldt <jana.eisoldt@ovgu.de>
 */

#ifndef BME680_H
#define BME680_H

#include "periph/i2c.h"

/* Add header includes here */

#ifdef __cplusplus
extern "C" {
#endif

/* Declare the API of the driver */

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    uint16_t temp_oversampling;
    uint16_t pres_oversampling;
    uint16_t hum_oversampling;
    uint16_t gas_heating_time;
    uint16_t gas_heating_temp;
    i2c_t i2c_dev;
    uint8_t i2c_addr;

    /* add initialization params here */
} bme680_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    /** Device initialization parameters */
    bme680_params_t params;
} bme680_t;

/**
 * @brief   Status and error return codes
 */
enum {
    BME680_OK = 0,                              /**< everything was fine */
    BME680_ERR_NODEV,                           /**< did not detect BMP180 */
    BME680_ERR_I2C,                             /**< error when reading/writing I2C bus */
    BME680_ERR_NOCAL,                           /**< error when reading calibration values */
};

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Initialized device descriptor of BME680 device
 * @param[in]    params     Initialization parameters
 *
 * @return                  BME680_OK on success
 */
int bme680_init(bme680_t *dev, const bme680_params_t *params);

int16_t bme680_read_tphg(const bme680_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* BME680_H */
/** @} */
