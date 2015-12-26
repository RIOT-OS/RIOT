/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_tcs37727 TCS37727 Light-To-Digital Converter
 * @ingroup     drivers_sensors
 * @brief       Driver for the AMS TCS37727 Color Light-To-Digital Converter
 *
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the TCS37727 sensor driver.
 *
 * @author      Felix Siebel <f.siebel@phytec.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef TCS37727_H
#define TCS37727_H

#include <stdint.h>
#include <stdbool.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef TCS37727_I2C_ADDRESS
#define TCS37727_I2C_ADDRESS    0x29    /**< Default Device Address */
#endif

#ifndef TCS37727_ATIME_DEFAULT
#define TCS37727_ATIME_DEFAULT  200000  /**< Default RGBC integration time */
#endif

/**
 * @brief Struct for storing TCS37727 sensor data
 */
typedef struct {
    uint32_t red;           /**< IR compensated channels red */
    uint32_t green;         /**< IR compensated channels green */
    uint32_t blue;          /**< IR compensated channels blue */
    uint32_t clear;         /**< channels clear */
    uint32_t lux;           /**< Lux */
    uint32_t ct;            /**< Color temperature */
} tcs37727_data_t;

/**
 * @brief Device descriptor for TCS37727 sensors.
 */
typedef struct {
    i2c_t i2c;              /**< I2C device the sensor is connected to */
    uint8_t addr;           /**< the sensor's slave address on the I2C bus */
    bool initialized;       /**< sensor status, true if sensor is initialized */
    int atime_us;           /**< atime value conveted to microseconds */
    int again;              /**< amount of gain */
} tcs37727_t;

/**
 * @brief Initialise the TCS37727 sensor driver.
 * Settings: Gain 4x, Proximity Detection off
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  i2c          I2C bus the sensor is connected to
 * @param[in]  address      sensor's I2C slave address
 * @param[in]  atime_us     rgbc RGBC integration time in microseconds
 *
 * @return                  0 on success
 * @return                  -1 if initialization of I2C bus failed
 * @return                  -2 if sensor test failed
 * @return                  -3 if sensor configuration failed
 */
int tcs37727_init(tcs37727_t *dev, i2c_t i2c, uint8_t address, int atime_us);

/**
 * @brief Set RGBC enable, this activates periodic RGBC measurements.
 *
 * @param[out] dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tcs37727_set_rgbc_active(tcs37727_t *dev);

/**
 * @brief Set RGBC disable, this deactivates periodic RGBC measurements.
 * Also turns off the sensor when proximity measurement is disabled.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tcs37727_set_rgbc_standby(tcs37727_t *dev);

/**
 * @brief Read sensor's data.
 * Besides an Autogain routine is called. If a maximum or minimum threshold
 * value of the channel clear is reached, then the gain will be changed
 * correspond to max or min threshold.
 *
 * @param[in]  dev         device descriptor of sensor
 * @param[out] data        device sensor data
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tcs37727_read(tcs37727_t *dev, tcs37727_data_t *data);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
