/*
 * Copyright (C) 2017 Frits Kuipers
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ds18 DS18 temperature sensor driver
 * @ingroup     drivers_sensors
 *
 * @brief       ds18 temperature sensor driver
 *
 * ## Description
 *
 * TODO
 *
 * @{
 *
 * @file
 * @brief       Driver for Maxim Integrated DS1822 and DS18B20 temperature sensors.
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
 */

#ifndef DS18_TEMP_H
#define DS18_TEMP_H

#include "periph/gpio.h"
#include "saul.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name ds18 status return codes
 * @{
 */
#define DS18_OK                       (0)
#define DS18_ERROR                    (-1)
/** @} */


/**
 * @brief   Device descriptor for a ds18 device
 */
typedef struct {
    gpio_t pin;             /**< Pin the sensor is connected to */
    gpio_mode_t in_mode;    /**< Pin output mode */
} ds18_t;

/**
 * @brief Device initialization parameters
 */
typedef struct {
    gpio_t pin;             /**< Pin the sensor is connected to */
    gpio_mode_t in_mode;    /**< Pin output mode */
} ds18_params_t;

/**
 * @brief export SAUL endpoint
 */
extern const saul_driver_t ds18_temperature_saul_driver;

/**
 * @brief   Initialize a ds18 device
 *
 * @param[out] dev          device descriptor
 * @param[in]  params       ds18 initialization struct
 *
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int ds18_init(ds18_t *dev, ds18_params_t *params);

/**
 * @brief   Get measured temperature
 *
 * @param[in] dev           device descriptor
 * @param[out] temperature  uint16_t buffer where temperature will be written to in centi-degrees
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int ds18_get_temperature(ds18_t *dev, int16_t *temperature);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* DS18_TEMP_H */
