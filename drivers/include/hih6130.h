/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_hih6130 HIH6130 humidity and temperature sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver for Honeywell HumidIcon Digital
 *              Humidity/Temperature Sensors: HIH-6130/6131 Series
 * @{
 *
 * @file
 * @brief       Device driver for Honeywell HumidIcon Digital
 *              Humidity/Temperature Sensors: HIH-6130/6131 Series
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef HIH6130_H
#define HIH6130_H

#include <stdint.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device descriptor for HIH6130/HIH6131 sensors
 */
typedef struct {
    i2c_t i2c;              /**< I2C device the sensor is connected to */
    uint8_t addr;           /**< the slave address of the sensor on the I2C bus */
} hih6130_t;

/**
 * @brief   Initialize a sensor
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  i2c          I2C bus the sensor is connected to
 * @param[in]  address      I2C slave address of the sensor
 */
void hih6130_init(hih6130_t *dev, i2c_t i2c, uint8_t address);

/**
 * @brief   Read humidity and temperature
 *
 * @param[in]  dev                          Sensor device descriptor
 * @param[out] humidity_milli_percent       Relative humidity in E-03 %
 * @param[out] temperature_milli_celsius    Temperature in m°C
 *
 * @retval  0       success
 * @retval  -1      error
 * @retval 1        data is stale
 */
int hih6130_get_humidity_temperature(const hih6130_t *dev,
        int32_t *humidity_milli_percent, int32_t *temperature_milli_celsius);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* HIH6130_H */
