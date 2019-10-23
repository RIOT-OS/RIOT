/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mag3110 MAG3110 3-Axis Digital Magnetometer
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the Freescale MAG3110 magnetometer.
 *
 * After initialization and set active the magnetometer will make measurements
 * at periodic times.
 * The output rate and over sample ratio can be determined by magnetometer
 * initialization.
 * The measured values of magnetic field strength and die temperature have
 * uncalibrated offsets.
 * To get correct measurement values, the individual offsets must be measured
 * and set accordingly.
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Interface definition for the MAG3110 magnetometer driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef MAG3110_H
#define MAG3110_H

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
    MAG3110_OK,                 /**< all good */
    MAG3110_ERROR_I2C,          /**< I2C communication failed */
    MAG3110_ERROR_DEV,          /**< Device MAG3110 not found */
    MAG3110_ERROR_CNF,          /**< Device configuration failed */
};

#ifndef MAG3110_I2C_ADDRESS
#define MAG3110_I2C_ADDRESS             0x0E /**< Magnetometer Default Address */
#endif

/**
 * @name    Output data rate macros
 * @{
 */
#define MAG3110_DROS_8000_16    0 /**< Output Rate 80 Hz, Over Sample Ratio 16 */
#define MAG3110_DROS_4000_32    1 /**< Output Rate 40 Hz, Over Sample Ratio 32 */
#define MAG3110_DROS_2000_64    2 /**< Output Rate 20 Hz, Over Sample Ratio 64 */
#define MAG3110_DROS_1000_128   3 /**< Output Rate 10 Hz, Over Sample Ratio 128 */
#define MAG3110_DROS_4000_16    4 /**< Output Rate 40 Hz, Over Sample Ratio 16 */
#define MAG3110_DROS_2000_32    5 /**< Output Rate 20 Hz, Over Sample Ratio 32 */
#define MAG3110_DROS_1000_64    6 /**< Output Rate 10 Hz, Over Sample Ratio 64 */
#define MAG3110_DROS_0500_128   7 /**< Output Rate 5 Hz, Over Sample Ratio 128 */
#define MAG3110_DROS_2000_16    8 /**< Output Rate 20 Hz, Over Sample Ratio 16 */
#define MAG3110_DROS_1000_32    9 /**< Output Rate 10 Hz, Over Sample Ratio 32 */
#define MAG3110_DROS_0500_64    10 /**< Output Rate 5 Hz, Over Sample Ratio 64 */
#define MAG3110_DROS_0250_128   11 /**< Output Rate 2.5 Hz, Over Sample Ratio 128 */
#define MAG3110_DROS_1000_16    12 /**< Output Rate 10 Hz, Over Sample Ratio 16 */
#define MAG3110_DROS_0500_32    13 /**< Output Rate 5 Hz, Over Sample Ratio 32 */
#define MAG3110_DROS_0250_64    14 /**< Output Rate 2.5 Hz, Over Sample Ratio 64 */
#define MAG3110_DROS_0125_128   15 /**< Output Rate 1.25 Hz, Over Sample Ratio 128 */
#define MAG3110_DROS_0500_16    16 /**< Output Rate 5 Hz, Over Sample Ratio 16 */
#define MAG3110_DROS_0250_32    17 /**< Output Rate 2.5 Hz, Over Sample Ratio 32 */
#define MAG3110_DROS_0125_64    18 /**< Output Rate 1.25 Hz, Over Sample Ratio 64 */
#define MAG3110_DROS_0063_128   19 /**< Output Rate 0.63 Hz, Over Sample Ratio 128 */
#define MAG3110_DROS_0250_16    20 /**< Output Rate 2.5 Hz, Over Sample Ratio 16 */
#define MAG3110_DROS_0125_32    21 /**< Output Rate 1.25 Hz, Over Sample Ratio 32 */
#define MAG3110_DROS_0063_64    22 /**< Output Rate 0.63 Hz, Over Sample Ratio 64 */
#define MAG3110_DROS_0031_128   23 /**< Output Rate 0.31 Hz, Over Sample Ratio 128 */
#define MAG3110_DROS_0125_16    24 /**< Output Rate 1.25 Hz, Over Sample Ratio 16 */
#define MAG3110_DROS_0063_32    25 /**< Output Rate 0.63 Hz, Over Sample Ratio 32 */
#define MAG3110_DROS_0031_64    26 /**< Output Rate 0.31 Hz, Over Sample Ratio 64 */
#define MAG3110_DROS_0016_128   27 /**< Output Rate 0.16 Hz, Over Sample Ratio 128 */
#define MAG3110_DROS_0063_16    28 /**< Output Rate 0.63 Hz, Over Sample Ratio 16 */
#define MAG3110_DROS_0031_32    29 /**< Output Rate 0.31 Hz, Over Sample Ratio 32 */
#define MAG3110_DROS_0016_64    30 /**< Output Rate 0.16 Hz, Over Sample Ratio 64 */
#define MAG3110_DROS_0008_128   31 /**< Output Rate 0.08 Hz, Over Sample Ratio 128 */
#define MAG3110_DROS_DEFAULT    MAG3110_DROS_0125_128 /**< Default Setting for testing */
/** @} */

/**
 * @brief   Configuration parameters
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus the device is connected to */
    uint8_t addr;               /**< I2C bus address of the device */
    uint8_t type;               /**< device type */
    uint8_t dros;               /**< sampling rate to use */
    int16_t offset[3];          /**< data offset in X, Y, and Z direction */
} mag3110_params_t;

/**
 * @brief   Device descriptor for MAG3110 magnetometer.
 */
typedef struct {
    mag3110_params_t params;    /**< device configuration parameters */
} mag3110_t;

/**
 * @brief   Data type for the result data
 */
typedef struct {
    int16_t x;                  /**< acceleration in X direction */
    int16_t y;                  /**< acceleration in Y direction */
    int16_t z;                  /**< acceleration in Z direction */
} mag3110_data_t;

/**
 * @brief   Initialise the MAG3110 magnetometer driver.
 *
 * @param[out] dev          device descriptor of magnetometer to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  0 on success
 * @return                  -1 if I2C communication failed
 * @return                  -2 if magnetometer test failed
 * @return                  -3 if magnetometer configuration failed
 */
int mag3110_init(mag3110_t *dev, const mag3110_params_t *params);

/**
 * @brief   Set user offset correction.
 *
 * Offset correction register will be erased after accelerometer reset.
 *
 * @param[out] dev          device descriptor of magnetometer
 * @param[in]  x            offset offset correction value for x-axis
 * @param[in]  y            offset offset correction value for y-axis
 * @param[in]  z            offset offset correction value for z-axis
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mag3110_set_user_offset(const mag3110_t *dev, int16_t x, int16_t y, int16_t z);

/**
 * @brief   Set active mode, this enables periodic measurements.
 *
 * @param[out] dev          device descriptor of magnetometer
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mag3110_set_active(const mag3110_t *dev);

/**
 * @brief   Set standby mode.
 *
 * @param[in]  dev          device descriptor of magnetometer
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mag3110_set_standby(const mag3110_t *dev);

/**
 * @brief   Check for new set of measurement data.
 *
 * @param[in]  dev          device descriptor of magnetometer
 *
 * @return                  >0 if x-, y- and z-axis new sample is ready
 * @return                  0 if measurement is in progress
 * @return                  -1 on error
 */
int mag3110_is_ready(const mag3110_t *dev);

/**
 * @brief   Read magnetometer's data.
 *
 * To get the actual values for the magnetic field in \f$\mu T\f$,
 * one have to divide the returned values from the magnetometer by 10.
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[out] data         the current magnetic field strength
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mag3110_read(const mag3110_t *dev, mag3110_data_t *data);

/**
 * @brief   Read die temperature.
 *
 * @param[in]  dev          device descriptor of magnetometer
 * @param[out] dtemp        die temperature
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mag3110_read_dtemp(const mag3110_t *dev, int8_t *dtemp);

#ifdef __cplusplus
}
#endif

#endif /* MAG3110_H */
/** @} */
