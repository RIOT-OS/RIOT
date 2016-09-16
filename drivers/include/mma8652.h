/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mma8652 MMA8652 Accelerometer
 * @ingroup     drivers_sensors
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
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 */

#ifndef MMA8652_H
#define MMA8652_H

#include <stdint.h>
#include <stdbool.h>
#include "periph/gpio.h"
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

enum {
    MMA8x5x_TYPE_MMA8652 = 0,
    MMA8x5x_TYPE_MMA8653,
    MMA8x5x_TYPE_MMA8451,
    MMA8x5x_TYPE_MMA8452,
    MMA8x5x_TYPE_MMA8453,
    MMA8x5x_TYPE_MAX,
};

#ifdef MODULE_MMA8652_INT
typedef struct {
    gpio_t pin;         /**< mma8x5x pin interrupt */
    uint8_t source;     /**< mma8x5x interrupt source */
    gpio_cb_t cb;       /**< mma8x5x interrupt callback */
    void *arg;          /**< mma8x5x interrupt argument */
} mma8652_int_t;
#endif

/**
 * @brief Device descriptor for MMA8652 accelerometer.
 */
typedef struct {
    i2c_t i2c;              /**< I2C device, the accelerometer is connected to */
    uint8_t addr;           /**< the accelerometer's slave address on the I2C bus */
    bool initialized;       /**< accelerometer status, true if accelerometer is initialized */
    int16_t scale;          /**< each count corresponds to (1/scale) g */
    uint8_t type;           /**< mma8x5x type */
#ifdef MODULE_MMA8652_INT
    mma8652_int_t int1;     /**< mma8x5x interrupt struct 1 */
    mma8652_int_t int2;     /**< mma8x5x interrupt struct 2 */
#endif
} mma8652_t;

/**
 * @brief Data structure holding all the information needed for initialization
 */
typedef struct {
    i2c_t i2c;              /**< I2C bus used */
    uint8_t addr;           /**< accelerometer's I2C address */
    uint8_t rate;           /**< accelerometer's sampling rate */
    uint8_t scale;          /**< accelerometer's scale factor */
    uint8_t type;           /**< mma8x5x type */
#ifdef MODULE_MMA8652_INT
    mma8652_int_t *int1;    /**< mma8x5x interrupt struct 1 */
    mma8652_int_t *int2;    /**< mma8x5x interrupt struct 2 */
#endif
} mma8652_params_t;

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
 * @param[in]  type         mma8x5x type
 *
 * @return                  0 on success
 * @return                  -1 if parameters are wrong
 * @return                  -2 if initialization of I2C bus failed
 * @return                  -3 if accelerometer test failed
 * @return                  -4 if setting to STANDBY mode failed
 * @return                  -5 if accelerometer configuration failed
 */
int mma8652_init(mma8652_t *dev, i2c_t i2c, uint8_t address, uint8_t dr, uint8_t range, uint8_t type);

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
 * Acceleration will be calculated as:<br>
 *     \f$ a = \frac{value \cdot 1000}{1024} \cdot mg \f$ if full scale is set to 2g<br>
 *     \f$ a = \frac{value \cdot 1000}{512} \cdot mg \f$ if full scale is set to 4g<br>
 *     \f$ a = \frac{value \cdot 1000}{256} \cdot mg \f$ if full scale is set to 8g<br>
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[out] x            x-axis value in mg
 * @param[out] y            y-axis value in mg
 * @param[out] z            z-axis value in mg
 * @param[out] status       accelerometer status register
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mma8652_read(mma8652_t *dev, int16_t *x, int16_t *y, int16_t *z, uint8_t *status);

/**
 * @brief Read one byte from a register at the MMA8652 device.
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[in]  reg          the register address on the targeted MMA8652 device
 * @param[out] data         the result that was read
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mma8682_read_reg(mma8652_t *dev, uint8_t reg, uint8_t *data);

/**
 * @brief Read the interruption status byte from the MMA8652 device.
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[out] data         the result that was read from the interruption status
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mma8682_read_interruption_status(mma8652_t *dev, uint8_t *data);

/**
 * @brief Read the Portrait/Landscape status byte from the MMA8652 device.
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[out] data         the result that was read from the Portrait/Landscape status
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mma8682_read_pl_status(mma8652_t *dev, uint8_t *data);

/**
 * @brief Read the freefall/motion status byte from the MMA8652 device.
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[out] data         the result that was read from the freefall/motion status
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int mma8682_read_freefall_motion_status(mma8652_t *dev, uint8_t *data);

#ifdef MODULE_MMA8652_INT
/**
 * @brief Initialize the MMA8652 accelerometer driver with interruptions.
 *
 * @param[out] dev          device descriptor of accelerometer to initialize
 * @param[in]  i2c          I2C bus the accelerometer is connected to
 * @param[in]  address      accelerometer's I2C slave address
 * @param[in]  dr           output data rate selection in WAKE mode
 * @param[in]  range        full scale range
 * @param[in]  type         mma8x5x type
 * @param[in]  int1         interruption device descriptor of the int1 pin
 * @param[in]  int2         interruption device descriptor of the int2 pin
 *
 * @return                  0 on success
 * @return                  -1 if parameters are wrong
 * @return                  -2 if initialization of I2C bus failed
 * @return                  -3 if accelerometer test failed
 * @return                  -4 if setting to STANDBY mode failed
 * @return                  -5 if accelerometer configuration failed
 * @return                  -6 if int1 pins configuration failed
 * @return                  -7 if int2 pins configuration failed
 * @return                  -13 if int1 and int2 pins configuration failed
 */
int mma8652_init_int(mma8652_t *dev, i2c_t i2c, uint8_t address, uint8_t dr,
           uint8_t range, uint8_t type, mma8652_int_t *int1, mma8652_int_t *int2);

/**
 * @brief Set the int1 pins interruption to the desired source.
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[in]  int_source   the int_source to interrupt
 *
 * @return                  0 on success
 * @return                  any other negative number in case of an error. For
 *                          portability implementations should draw inspiration
 *                          of the errno values from the POSIX' bind() function
 *                          specification.
 */
int mma8652_set_int1(mma8652_t *dev, uint8_t int_source);

/**
 * @brief Set the int2 pins interruption to the desired source.
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[in]  int_source   the int_source to interrupt
 *
 * @return                  0 on success
 * @return                  any other negative number in case of an error. For
 *                          portability implementations should draw inspiration
 *                          of the errno values from the POSIX' bind() function
 *                          specification.
 */
int mma8652_set_int2(mma8652_t *dev, uint8_t int_source);
#endif

#ifdef __cplusplus
}
#endif

#endif
/** @} */
