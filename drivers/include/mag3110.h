/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mag3110 MAG3110 3-Axis Digital Magnetometer
 * @ingroup     drivers_sensors
 * @brief       Driver for the Freescale MAG3110 magnetometer.
 *              After initialization and set activ the magnetometer
 *              will make measurements at periodic times.
 *              The output rate and over sample ratio
 *              can be determined by magnetometer initialization.
 *              The measured values of magnetic field strength and
 *              die temperature have uncalibrated offsets.
 *              To get correct measuremend values, the individual
 *              offsets must be measured and set accordingly.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the MAG3110 magnetometer driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
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

#ifndef MAG3110_I2C_ADDRESS
#define MAG3110_I2C_ADDRESS             0x0E /**< Magnetometer Default Address */
#endif

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

/**
 * @brief Device descriptor for MAG3110 magnetometer.
 */
typedef struct {
    i2c_t i2c;              /**< I2C device, the magnetometer is connected to */
    uint8_t addr;           /**< the magnetometer's slave address on the I2C bus */
    bool initialized;       /**< magnetometer status, true if magnetometer is initialized */
} mag3110_t;

/**
 * @brief MAG3110 magnetometer test.
 * This function looks for Device ID of the MAG3110 magnetometer.
 *
 * @param[in]  dev          device descriptor of magnetometer
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mag3110_test(mag3110_t *dev);

/**
 * @brief Initialise the MAG3110 magnetometer driver.
 *
 * @param[out] dev          device descriptor of magnetometer to initialize
 * @param[in]  i2c          I2C bus the magnetometer is connected to
 * @param[in]  address      magnetometer's I2C slave address
 * @param[in]  dros         data rate and over sampling selection
 *
 * @return                  0 on success
 * @return                  -1 if dros parameter is wrong
 * @return                  -2 if initialization of I2C bus failed
 * @return                  -3 if magnetometer test failed
 * @return                  -4 if magnetometer configuration failed
 */
int mag3110_init(mag3110_t *dev, i2c_t i2c, uint8_t address, uint8_t dros);

/**
 * @brief Set user offset correction.
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
int mag3110_set_user_offset(mag3110_t *dev, int16_t x, int16_t y, int16_t z);

/**
 * @brief Set active mode, this enables periodic measurements.
 *
 * @param[out] dev          device descriptor of magnetometer
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mag3110_set_active(mag3110_t *dev);

/**
 * @brief Set standby mode.
 *
 * @param[in]  dev          device descriptor of magnetometer
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mag3110_set_standby(mag3110_t *dev);

/**
 * @brief Check for new set of measurement data.
 *
 * @param[in]  dev          device descriptor of magnetometer
 *
 * @return                  >0 if x-, y- and z-axis new sample is ready
 * @return                  0 if measurement is in progress
 * @return                  -1 on error
 */
int mag3110_is_ready(mag3110_t *dev);

/**
 * @brief Read magnetometer's data.
 * To get the actual values for the magnetic field in \f$\mu T\f$,
 * one have to divide the returned values from the magnetometer by 10.
 *
 * @param[in]  dev          device descriptor of magnetometer
 * @param[out] x            x-axis magnetic field strength
 * @param[out] y            y-axis magnetic field strength
 * @param[out] z            z-axis magnetic field strength
 * @param[out] status       magnetometer status register
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mag3110_read(mag3110_t *dev, int16_t *x, int16_t *y, int16_t *z, uint8_t *status);

/**
 * @brief Read die temperature.
 *
 * @param[in]  dev          device descriptor of magnetometer
 * @param[out] dtemp        die temperature
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mag3110_read_dtemp(mag3110_t *dev, int8_t *dtemp);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
