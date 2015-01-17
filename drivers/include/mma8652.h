/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mma8652 MMA8652 Accelerometer
 * @ingroup     drivers
 * @brief       Driver for the Freescale MMA8652 3-Axis accelerometer.
 *              The driver will initialize the accelerometer for
 *              best resolution (12 bit).
 *              After initialization and set activ the accelerometer
 *              will make measurements at periodic times.
 *              The measurements period and scale range can be determined by
 *              accelerometer initialization.
 *              This driver only implements basic functionality.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the MMA8652 accelerometer driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef MMA8652_H
#define MMA8652_H

#include <stdint.h>
#include <stdbool.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef MMA8652_I2C_ADDRESS
#define MMA8652_I2C_ADDRESS           0x1D /**< Accelerometer Default Address */
#endif

#define MMA8652_DATARATE_800HZ        0 /**< 800 Hz Ouput Data Rate in WAKE mode */
#define MMA8652_DATARATE_400HZ        1 /**< 400 Hz Ouput Data Rate in WAKE mode */
#define MMA8652_DATARATE_200HZ        2 /**< 200 Hz Ouput Data Rate in WAKE mode */
#define MMA8652_DATARATE_100HZ        3 /**< 100 Hz Ouput Data Rate in WAKE mode */
#define MMA8652_DATARATE_50HZ         4 /**< 50 Hz Ouput Data Rate in WAKE mode */
#define MMA8652_DATARATE_1HZ25        5 /**< 12.5 Hz Ouput Data Rate in WAKE mode */
#define MMA8652_DATARATE_6HZ25        6 /**< 6.25 Hz Ouput Data Rate in WAKE mode */
#define MMA8652_DATARATE_1HZ56        7 /**< 1.56 Hz Ouput Data Rate in WAKE mode */
#define MMA8652_DATARATE_DEFAULT      MMA8652_DATARATE_6HZ25 /**< Data Rate for testing */

#define MMA8652_FS_RANGE_2G           0 /**< +/- 2 g Full Scale Range */
#define MMA8652_FS_RANGE_4G           1 /**< +/- 4 g Full Scale Range */
#define MMA8652_FS_RANGE_8G           2 /**< +/- 8 g Full Scale Range */
#define MMA8652_FS_RANGE_DEFAULT      MMA8652_FS_RANGE_2G /**< Full-Scale Range for testing */

/**
 * @brief Device descriptor for MMA8652 accelerometer.
 */
typedef struct {
    i2c_t i2c;              /**< I2C device, the accelerometer is connected to */
    uint8_t addr;           /**< the accelerometer's slave address on the I2C bus */
    bool initialized;       /**< accelerometer status, true if accelerometer is initialized */
} mma8652_t;

/**
 * @brief MMA8652 accelerometer test.
 * This function looks for Device ID of the MMA8652 accelerometer.
 *
 * @param[in]  dev          device descriptor of accelerometer
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mma8652_test(mma8652_t *dev);

/**
 * @brief Initialize the MMA8652 accelerometer driver.
 *
 * @param[out] dev          device descriptor of accelerometer to initialize
 * @param[in]  i2c          I2C bus the accelerometer is connected to
 * @param[in]  address      accelerometer's I2C slave address
 * @param[in]  dr           output data rate selection in WAKE mode
 * @param[in]  range        full scale range
 *
 * @return                  0 on success
 * @return                  -1 if parameters are wrong
 * @return                  -2 if initialization of I2C bus failed
 * @return                  -3 if accelerometer test failed
 * @return                  -4 if accelerometer configuration failed
 */
int mma8652_init(mma8652_t *dev, i2c_t i2c, uint8_t address, uint8_t dr, uint8_t range);

/**
 * @brief Set user offset correction.
 * Offset correction registers will be erased after accelerometer reset.
 *
 * @param[out] dev          device descriptor of accelerometer to initialize
 * @param[in]  x            offset correction value for x-axis
 * @param[in]  y            offset correction value for y-axis
 * @param[in]  z            offset correction value for z-axis
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mma8652_set_user_offset(mma8652_t *dev, int8_t x, int8_t y, int8_t z);

/**
 * @brief Reset the MMA8652 accelerometer. After that accelerometer should be reinitialized.
 *
 * @param[out] dev          device descriptor of accelerometer to reset
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mma8652_reset(mma8652_t *dev);

/**
 * @brief Set active mode, this enables periodic measurements.
 *
 * @param[out] dev          device descriptor of accelerometer to reset
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mma8652_set_active(mma8652_t *dev);

/**
 * @brief Set standby mode.
 *
 * @param[in]  dev          device descriptor of accelerometer
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mma8652_set_standby(mma8652_t *dev);

/**
 * @brief Check for new set of measurement data.
 *
 * @param[in]  dev          device descriptor of accelerometer
 *
 * @return                  >0 if x- ,y- ,z-axis new sample is ready
 * @return                  0 measurement in progress
 * @return                  -1 on error
 */
int mma8652_is_ready(mma8652_t *dev);

/**
 * @brief Read accelerometer's data.
 * Acceleration can be calculated as:<br>
 *     \f$ a = \frac{value}{1024} \cdot g \f$ if full scale is set to 2g<br>
 *     \f$ a = \frac{value}{512} \cdot g \f$ if full scale is set to 4g<br>
 *     \f$ a = \frac{value}{256} \cdot g \f$ if full scale is set to 8g<br>
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[out] x            x-axis value
 * @param[out] y            y-axis value
 * @param[out] z            z-axis value
 * @param[out] status       accelerometer status register
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mma8652_read(mma8652_t *dev, int16_t *x, int16_t *y, int16_t *z, uint8_t *status);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
