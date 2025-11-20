/*
 * Copyright (C) 2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_fxos8700 FXOS8700 3-axis accelerometer/magnetometer
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the FXOS8700 3-axis accelerometer/magnetometer
 *
 * The connection between the MCU and the FXOS8700 is based on the
 * I2C-interface.
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Interface definition for the FXOS8700 sensor driver
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 */

#include <stdint.h>
#include "periph/i2c.h"
#include "fxos8700_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_fxos8700_config     FXOS8700 accelerometer/magnetometer driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief   Default raw value mode for accelerator
 *
 * Set this to 1 to return raw ADC readings. Otherwise measurements
 * will be converted to mg.
 */
#ifdef DOXYGEN
#define CONFIG_FXOS8700_USE_ACC_RAW_VALUES
#endif
/** @} */

/**
 * @brief   FXOS8700 specific return values
 */
enum {
    FXOS8700_OK      = 0,     /**< everything went as expected */
    FXOS8700_ADDRERR = -1,    /**< no FXOS8700 device found on the bus */
    FXOS8700_NOBUS   = -2,    /**< errors while initializing the I2C bus */
    FXOS8700_NODEV   = -3,    /**< no FXOS8700 device found on the bus */
    FXOS8700_BUSERR  = -4     /**< error during I2C communication */
};

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    i2c_t i2c;                /**< I2C device that sensor is connected to */
    uint8_t addr;             /**< I2C address of this particular sensor */
    uint8_t acc_range;        /**< Accelerator full-scale range */
    uint32_t renew_interval;  /**< Interval for cache renewal */
} fxos8700_params_t;

/**
 * @brief   Individual 3-axis measurement
 */
typedef struct {
    int16_t x;   /**< x axis of 3-axis measurement */
    int16_t y;   /**< y axis of 3-axis measurement */
    int16_t z;   /**< z axis of 3-axis measurement */
} fxos8700_measurement_t;

/**
 * @brief   Device descriptor for a FXOS8700 device
 */
typedef struct {
    uint8_t config;                    /**< sensor configuration including active mode */
    fxos8700_measurement_t acc_cached; /**< cached 3-axis acceleration */
    fxos8700_measurement_t mag_cached; /**< cached 3-axis magnetic field */
    uint32_t last_read_time;           /**< last time when cached data was refreshed */
    fxos8700_params_t p;               /**< configuration parameters */
} fxos8700_t;

/**
 * @brief   Initialize an FXOS8700 device
 *
 * @param[out] dev          device descriptor
 * @param[in] params        parameters for device
 *
 * @return                  FXOS8700_OK on success
 * @return                  FXOS8700_NOBUS on I2C initialization error
 * @return                  FXOS8700_BUSERR on I2C communication error
 * @return                  FXOS8700_ADDRERR on invalid address
 * @return                  FXOS8700_NODEV if no FXOS8700 device found on bus
 */
int fxos8700_init(fxos8700_t* dev, const fxos8700_params_t* params);

/**
 * @brief   Convenience function for turning on the FXOS8700
 *
 * This function will trigger a new conversion, wait for the conversion to be
 * finished and the get the results from the device.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  FXOS8700_OK on success
 * @return                  FXOS8700_BUSERR on I2C communication failures
 */
int fxos8700_set_active(const fxos8700_t* dev);

/**
 * @brief   Convenience function for turning off the FXOS8700
 *
 * This function will trigger a new conversion, wait for the conversion to be
 * finished and the get the results from the device.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  FXOS8700_OK on success
 * @return                  FXOS8700_BUSERR on I2C communication failures
 */
int fxos8700_set_idle(const fxos8700_t* dev);

/**
 * @brief   Convenience function for reading acceleration and magnetic field
 *
 * This function will trigger a new conversion, wait for the conversion to be
 * finished and the get the results from the device.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] acc          3-axis acceleration [in milli grativy acceleration (mg) ]
 * @param[out] mag          3-axis magnetic field [in degree milli Gauss (mGs) ]
 *
 * @return                  FXOS8700_OK on success
 * @return                  FXOS8700_BUSERR on I2C communication failures
 */
int fxos8700_read(const fxos8700_t* dev, fxos8700_measurement_t* acc, fxos8700_measurement_t* mag);

/**
 * @brief   Extended read function including caching capability
 *
 * This function will return cached values if they are within the sampling
 * period (FXOS8700_RENEW_INTERVAL), or will trigger a new conversion, wait for
 * the conversion to be finished and the get the results from the device.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] acc          3-axis acceleration [in milli grativy acceleration (mg) ]
 * @param[out] mag          3-axis magnetic field [in degree milli Gauss (mGs) ]
 *
 * @return                  FXOS8700_OK on success
 * @return                  FXOS8700_BUSERR on I2C communication failures
 */
int fxos8700_read_cached(const void* dev, fxos8700_measurement_t* acc, fxos8700_measurement_t* mag);
#ifdef __cplusplus
}
#endif

/** @} */
