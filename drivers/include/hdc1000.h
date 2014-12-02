/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_hdc1000 HDC1000 Humidity and Temperature Sensor
 * @ingroup     drivers
 * @brief       Driver for the Texas Instruments HDC1000
 *              Humidity and Temperature Sensor.
 *              The driver will initialize the sensor for best
 *              resolution (14 bit). Currently the driver doesn't use the heater.
 *              Temperature and humidity are acquired in sequence.
 *              The sensor is always in sleep mode. The measurement must
 *              be started by a write access to the address 0x00
 *              (HDC1000_TEMPERATURE). After completing the measurement
 *              the sensor will return to sleep mode. Typical
 *              Conversion Time by 14 bit resolution is 6.50ms.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the HDC1000 sensor driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef HDC1000_H
#define HDC1000_H

#include <stdint.h>
#include <stdbool.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef HDC1000_I2C_ADDRESS
#define HDC1000_I2C_ADDRESS           0x41
#endif

#ifndef HDC1000_CONVERSION_TIME
#define HDC1000_CONVERSION_TIME       6500
#endif

/**
 * @brief Device descriptor for HDC1000 sensors.
 */
typedef struct {
    i2c_t i2c;              /**< I2C device the sensor is connected to */
    uint8_t addr;           /**< the sensor's slave address on the I2C bus */
    bool initialized;       /**< sensor status, true if sensor is initialized */
} hdc1000_t;

/**
 * @brief HDC1000 sensor test.
 * This function looks for Manufacturer ID of the HDC1000 sensor.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int hdc1000_test(hdc1000_t *dev);

/**
 * @brief Initialise the HDC1000 sensor driver.
 * 14 bit resolution, heater off, temperature and humidity
 * are acquired in sequence.
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  i2c          I2C bus the sensor is connected to
 * @param[in]  address      sensor's I2C slave address
 *
 * @return                  0 on success
 * @return                  -1 if initialization of I2C bus failed
 * @return                  -2 if sensor test failed
 * @return                  -3 if sensor configuration failed
 */
int hdc1000_init(hdc1000_t *dev, i2c_t i2c, uint8_t address);

/**
 * @brief Reset the HDC1000 sensor. After that sensor should be reinitialized.
 *
 * @param[out] dev          device descriptor of sensor to reset
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int hdc1000_reset(hdc1000_t *dev);

/**
 * @brief Trigger the measurements.
 * Conversion Time by 14 bit resolution is 6.50ms.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int hdc1000_startmeasure(hdc1000_t *dev);

/**
 * @brief Read sensor's data.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int hdc1000_read(hdc1000_t *dev, uint16_t *rawtemp, uint16_t *rawhum);

/**
 * @brief Convert raw sensor values to temperature and humidity.
 *
 * @param[in]  rawtemp      raw temperature value
 * @param[in]  rawhum       raw humidity value
 * @param[out] temp         converted temperature*100
 * @param[out] hum          converted humidity*100
 */
void hdc1000_convert(uint16_t rawtemp, uint16_t rawhum,  int *temp, int *hum);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
