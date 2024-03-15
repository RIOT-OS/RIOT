/*
 * Copyright (C) 2023 TU Braunschweig Institut für Betriebssysteme und Rechnerverbund
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sen5x Sensirion Embedded I2C SEN5x Driver
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for I2C communication to SEN5x devices.
 *
 * @{
 *
 * @file
 *
 * @author      Daniel Prigoshij <prigoshi@ibr.cs.tu-bs.de>
 */

#ifndef SEN5X_H
#define SEN5X_H

/* Add header includes here */

#include "periph/i2c.h"
#include <stdint.h>
#include "saul.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Declare the API of the driver */

/**
 * @brief   Wrapper for measured values
 */
typedef struct {
    uint16_t mass_concentration_pm1p0;
    uint16_t mass_concentration_pm2p5;
    uint16_t mass_concentration_pm4p0;
    uint16_t mass_concentration_pm10p0;
    uint16_t number_concentration_pm0p5;
    uint16_t number_concentration_pm1p0;
    uint16_t number_concentration_pm2p5;
    uint16_t number_concentration_pm4p0;
    uint16_t number_concentration_pm10p0;
    uint16_t typical_particle_size;
    int16_t ambient_humidity;
    int16_t ambient_temperature;
    int16_t voc_index;
    int16_t nox_index;
} sen5x_measurement_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                  /**< I2C device which is used */
    uint8_t i2c_addr;               /**< I2C address */
} sen5x_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    sen5x_params_t params;                  /**< Device initialization parameters */
} sen5x_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 * 
 * @return 0 on success
 * @return < 0 on error
 */
int sen5x_init(sen5x_t *dev, const sen5x_params_t *params);

/**
 * @brief   Execute a reset on the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * 
 * @return 0 on success
 * @return < 0 on error
 */
int sen5x_reset(const sen5x_t *dev);

/**
 * @brief   Starts a continuous measurement
 *
 * @param[inout] dev        Device descriptor of the driver
 */
void sen5x_wake(const sen5x_t *dev);

/**
 * @brief   Stops the measurement and returns to idle mode
 *
 * @param[inout] dev        Device descriptor of the driver
 */
void sen5x_sleep(const sen5x_t *dev);

/**
 * @brief   Sets the fan to maximum speed, to clean it within 10 seconds
 *
 * @param[inout] dev        Device descriptor of the driver
 */
void sen5x_clean_fan(const sen5x_t *dev);

/**
 * @brief   Read measured mass concentration, humidity and temperature values
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[out]   values     Pointer to wrapper containing all measured values
 */
void sen5x_read_values(const sen5x_t *dev, sen5x_measurement_t *values);

/**
 * @brief   Read measured particle matter values
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[out]   values     Pointer to wrapper containing all measured values
 */
void sen5x_read_pm_values(const sen5x_t *dev, sen5x_measurement_t *values);


#ifdef __cplusplus
}
#endif

#endif /* SEN5X_H */
/** @} */
