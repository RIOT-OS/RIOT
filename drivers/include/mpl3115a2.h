/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_mpl3115a2 MPL3115A2 Pressure Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the Freescale MPL3115A2 pressure sensor.
 *
 * The driver will initialize the sensor for pressure measurement. The
 * conversion duration depends on oversample ratio. After initialization
 * the sensor can be set active to run periodic measurements. The oversample
 * ratio can be configured during sensor initialization.
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Interface definition for the MPL3115A2 sensor driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include <stdint.h>
#include <stdbool.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Named return values
 */
enum {
    MPL3115A2_OK,               /**< all good */
    MPL3115A2_ERROR_I2C,        /**< I2C communication failed */
    MPL3115A2_ERROR_DEV,        /**< Device MPL3115A2 not found */
    MPL3115A2_ERROR_CNF,        /**< Device configuration failed */
};

/**
 * @brief   MPL3115A2 Default Address
 */
#define MPL3115A2_I2C_ADDRESS   (0x60)

/**
 * @name    Oversample Ratio configuration
 * @{
 */
enum {
    MPL3115A2_OS_RATIO_1 = 0,   /**< Oversample Ratio 1, conversion 6ms */
    MPL3115A2_OS_RATIO_2,       /**< Oversample Ratio 2, conversion 10ms */
    MPL3115A2_OS_RATIO_4,       /**< Oversample Ratio 4, conversion 18ms */
    MPL3115A2_OS_RATIO_8,       /**< Oversample Ratio 8, conversion 34ms */
    MPL3115A2_OS_RATIO_16,      /**< Oversample Ratio 16, conversion 66ms */
    MPL3115A2_OS_RATIO_32,      /**< Oversample Ratio 32, conversion 130ms */
    MPL3115A2_OS_RATIO_64,      /**< Oversample Ratio 64, conversion 258ms */
    MPL3115A2_OS_RATIO_128,     /**< Oversample Ratio 128, conversion 514ms */
};
#define MPL3115A2_OS_RATIO_DEFAULT  MPL3115A2_OS_RATIO_16 /**< Default Ratio */
/** @} */

/**
 * @defgroup drivers_mpl3115a2_config     MPL3115A2 Pressure Sensor driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief   Maximum Conversion Time in microseconds [us]
 *
 * @note    Conversion time is: ((oversampling ratio * 4) + 2) * 1000 us
 */
#ifndef MPL3115A2_CONVERSION_TIME
#define MPL3115A2_CONVERSION_TIME   (514000UL)
#endif
/** @} */

/**
 * @brief   Configuration parameters
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus the device is connected to */
    uint8_t addr;               /**< I2C bus address of the device */
    uint8_t ratio;              /**< MPL3115A2 oversampling ratio */
} mpl3115a2_params_t;

/**
 * @brief   Device descriptor for MPL3115A2 sensors.
 */
typedef struct {
    mpl3115a2_params_t params;  /**< device configuration parameters */
} mpl3115a2_t;

/**
 * @brief   Initialize the MPL3115A2 sensor driver.
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  MPL3115A2_OK on success
 * @return                  -MPL3115A2_ERROR_I2C on I2C bus error
 * @return                  -MPL3115A2_ERROR_DEV on device error
 * @return                  -MPL3115A2_ERROR_CNF on config error
 */
int mpl3115a2_init(mpl3115a2_t *dev, const mpl3115a2_params_t *params);

/**
 * @brief   Reset the MPL3115A2 sensor. After that, the sensor should be reinitialized.
 *
 * @param[in] dev           device descriptor of sensor
 *
 * @return                  MPL3115A2_OK on success
 * @return                  -MPL3115A2_ERROR_I2C on error
 */
int mpl3115a2_reset(const mpl3115a2_t *dev);

/**
 * @brief   Set active mode, this enables periodic measurements.
 *
 * @param[in] dev           device descriptor of sensor
 *
 * @return                  MPL3115A2_OK on success
 * @return                  -MPL3115A2_ERROR_I2C on error
 */
int mpl3115a2_set_active(const mpl3115a2_t *dev);

/**
 * @brief   Set standby mode.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  MPL3115A2_OK on success
 * @return                  -MPL3115A2_ERROR_I2C on error
 */
int mpl3115a2_set_standby(const mpl3115a2_t *dev);

/**
 * @brief   Check for new set of measurement data.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  >0 if new data sample is ready
 * @return                  MPL3115A2_OK measurement in progress
 * @return                  -MPL3115A2_ERROR_I2C on error
 */
int mpl3115a2_is_ready(const mpl3115a2_t *dev);

/**
 * @brief   Read sensor's data in pressure mode.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] pres         pressure in Pascals
 * @param[out] status       sensor status register
 *
 * @return                  MPL3115A2_OK on success,
 * @return                  -MPL3115A2_ERROR_I2C on error
 */
int mpl3115a2_read_pressure(const mpl3115a2_t *dev, uint32_t *pres, uint8_t *status);

/**
 * @brief   Read sensor's temperature.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] temp         temperature in \f$^\circ C \cdot 10\f$
 *
 * @return                  MPL3115A2_OK on success
 * @return                  -MPL3115A2_ERROR_I2C on error
 */
int mpl3115a2_read_temp(const mpl3115a2_t *dev, int16_t *temp);

#ifdef __cplusplus
}
#endif

/** @} */
