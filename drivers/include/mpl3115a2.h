/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mpl3115a2 MPL3115A2 Pressure Sensor
 * @ingroup     drivers
 * @brief       Driver for the Freescale MPL3115A2 pressure sensor.
 *              The driver will initialize the sensor for 
 *              pressure measurement. The conversion duration
 *              depends on oversample ratio.
 *              After initialization and set activ the sensor
 *              will make measurements at periodic times.
 *              The oversample ratio can be determined
 *              by sensor initialization.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the MPL3115A2 sensor driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef MPL3115A2_H
#define MPL3115A2_H

#include <stdint.h>
#include <stdbool.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef MPL3115A2_I2C_ADDRESS
#define MPL3115A2_I2C_ADDRESS           0x60 /**< Pressure Sensor Default Address */
#endif

#define MPL3115A2_OS_RATIO_1            0 /**< Oversample Ratio 1, conversion time 6 ms */
#define MPL3115A2_OS_RATIO_2            1 /**< Oversample Ratio 2, conversion time 10 ms */
#define MPL3115A2_OS_RATIO_4            2 /**< Oversample Ratio 4, conversion time 18 ms */
#define MPL3115A2_OS_RATIO_8            3 /**< Oversample Ratio 8, conversion time 34 ms */
#define MPL3115A2_OS_RATIO_16           4 /**< Oversample Ratio 16, conversion time 66 ms */
#define MPL3115A2_OS_RATIO_32           5 /**< Oversample Ratio 32, conversion time 130 ms */
#define MPL3115A2_OS_RATIO_64           6 /**< Oversample Ratio 64, conversion time 258 ms */
#define MPL3115A2_OS_RATIO_128          7 /**< Oversample Ratio 128, conversion time 512 ms */
#define MPL3115A2_OS_RATIO_DEFAULT      MPL3115A2_OS_RATIO_128 /**< Default Ratio for testing */

#ifndef MPL3115A2_CONVERSION_TIME
#define MPL3115A2_CONVERSION_TIME       512000 /**< Maximum Conversion Time in us */
#endif


/**
 * @brief Device descriptor for MPL3115A2 sensors.
 */
typedef struct {
    i2c_t i2c;              /**< I2C device, the sensor is connected to */
    uint8_t addr;           /**< the sensor's slave address on the I2C bus */
    bool initialized;       /**< sensor status, true if sensor is initialized */
} mpl3115a2_t;

/**
 * @brief MPL3115A2 sensor test.
 * This function looks for Device ID of the MPL3115A2 sensor.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mpl3115a2_test(mpl3115a2_t *dev);

/**
 * @brief Initialize the MPL3115A2 sensor driver.
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  i2c          I2C bus the sensor is connected to
 * @param[in]  address      sensor's I2C slave address
 * @param[in]  os_ratio     oversample rate selection
 *
 * @return                  0 on success
 * @return                  -1 if os_ratio parameter is wrong
 * @return                  -2 if initialization of I2C bus failed
 * @return                  -3 if sensor test failed
 * @return                  -4 if sensor configuration failed
 */
int mpl3115a2_init(mpl3115a2_t *dev, i2c_t i2c, uint8_t address, uint8_t os_ratio);

/**
 * @brief Reset the MPL3115A2 sensor. After that, the sensor should be reinitialized.
 *
 * @param[out] dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mpl3115a2_reset(mpl3115a2_t *dev);

/**
 * @brief Set active mode, this enables periodic measurements.
 *
 * @param[out] dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mpl3115a2_set_active(mpl3115a2_t *dev);

/**
 * @brief Set standby mode.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mpl3115a2_set_standby(mpl3115a2_t *dev);

/**
 * @brief Check for new set of measurement data.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  >0 if new data sample is ready
 * @return                  0 measurement in progress
 * @return                  -1 on error
 */
int mpl3115a2_is_ready(mpl3115a2_t *dev);

/**
 * @brief Read sensor's data in pressure mode.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] pres         pressure in Pascals
 * @param[out] status       sensor status register
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mpl3115a2_read_pressure(mpl3115a2_t *dev, uint32_t *pres, uint8_t *status);

/**
 * @brief Read sensor's temperature.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] temp         temperature in \f$^\circ C \cdot 10\f$
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mpl3115a2_read_temp(mpl3115a2_t *dev, int16_t *temp);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
