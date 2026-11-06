/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_qmi8658 QMI8658 6D MEMS IMU
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the QMI8658 6D MEMS inertial measurement unit (IMU)
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * ## About
 * This module provides a device driver for the QMI8658 interial measurement unit
 * (IMU) by QST. This driver exposes accelerometer, gyroscope and temperature sensor
 * data in several operating modes of the device (qmi8658_mode_t).
 * This driver also supports the tap detection feature of the QMI8658.
 * Currently, only the I2C interface is supported (the sensor also has SPI and I3C).
 * Programmable interrupts via the INT1 and INT2 pins are currently also not supported.
 *
 * ## Configuration
 * This driver is configurable via the parameters in qmi8658_params.h:
 * - Acceleromter output data rate (in normal and low power mode)
 * - Acceleromter value full scale
 * - Gyroscope output data rate
 * - Gyroscope value full scale
 * - Tap detection parameters
 *
 * ## Usage
 * Use the @ref qmi8658_init function to initialize the device. This will configure the
 * device but not enable any sensors. Use @ref qmi8658_set_mode to select an operating mode
 * for the device, which enables the corresponding sensors in the chosen power mode.
 * Now the sensor values can be polled using the functions @ref qmi8658_read_acc,
 * @ref qmi8658_read_gyro and @ref qmi8658_read_temp.
 *
 * ### Tap detection
 * Tap detection only works while the accelerometer is enabled. After initializing the device,
 * tap detection can be enabled with @ref qmi8658_enable_tap. Tap data can then be polled using
 * @ref qmi8658_read_tap. See qmi8658_tap_data_t for a description of the tap data fields.
 *
 *
 * @{
 *
 * @file
 *
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 */

#include "periph/i2c.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       QMI8658 IMU driver compile configuration
 * @{
 */
#ifndef CONFIG_QMI8658_PARAM_I2C
/* I2C slave address. Set to 0x6A if SDO/SA0 is pulled high */
#  define CONFIG_QMI8658_PARAM_I2C    0x6B
#endif
/** @} */

/**
 * @brief   Data rate settings
 *
 * Actual data rates are slightly lower when gyroscope is enabled.
 * If the selected accelerometer ODR is higher than 1kHz, enabling
 * the accelerometer will implicitly enable the gyroscope as required
 * by hardware design.
 */
typedef enum {
    QMI8658_DATA_RATE_8KHZ      = 0x00,
    QMI8658_DATA_RATE_4KHZ      = 0x01,
    QMI8658_DATA_RATE_2KHZ      = 0x02,
    QMI8658_DATA_RATE_1KHZ      = 0x03,
    QMI8658_DATA_RATE_500HZ     = 0x04,
    QMI8658_DATA_RATE_250HZ     = 0x05,
    QMI8658_DATA_RATE_125HZ     = 0x06,
    QMI8658_DATA_RATE_62_5HZ    = 0x07,
    QMI8658_DATA_RATE_31_25HZ   = 0x08,
} qmi8658_odr_t;

/**
 * @brief   Accelerometer low power data rate settings
 *
 * Available Accelerometer ODRs when low power mode is activated.
 */
typedef enum {
    QMI8658_ACC_LOWPWR_DATA_RATE_128HZ  = 0x0C,
    QMI8658_ACC_LOWPWR_DATA_RATE_21HZ   = 0x0D,
    QMI8658_ACC_LOWPWR_DATA_RATE_11HZ   = 0x0E,
    QMI8658_ACC_LOWPWR_DATA_RATE_3HZ    = 0x0F
} qmi8658_acc_lowpwr_odr_t;

/**
 * @brief   Accelerometer full scale
 */
typedef enum {
    QMI8658_ACC_FS_2G   = 0x00,
    QMI8658_ACC_FS_4G   = 0x01,
    QMI8658_ACC_FS_8G   = 0x02,
    QMI8658_ACC_FS_16G  = 0x03
} qmi8658_acc_fs_t;

/**
 * @brief   Gyroscope full scale
 */
typedef enum {
    QMI8658_GYRO_FS_16DPS   = 0x00,
    QMI8658_GYRO_FS_32DPS   = 0x01,
    QMI8658_GYRO_FS_64DPS   = 0x02,
    QMI8658_GYRO_FS_128DPS  = 0x03,
    QMI8658_GYRO_FS_256DPS  = 0x04,
    QMI8658_GYRO_FS_512DPS  = 0x05,
    QMI8658_GYRO_FS_1024DPS = 0x06,
    QMI8658_GYRO_FS_2048DPS = 0x07
} qmi8658_gyro_fs_t;

/**
 * @brief   Tap axis priorities
 *
 * Which axis to prefer in case of an ambiguous tap.
 */
typedef enum {
    QMI8658_TAP_PRIO_XYZ    = 0,    /**< X > Y > Z */
    QMI8658_TAP_PRIO_XZY    = 1,    /**< X > Z > Y */
    QMI8658_TAP_PRIO_YXZ    = 2,    /**< Y > X > Z */
    QMI8658_TAP_PRIO_YZX    = 3,    /**< Y > Z > > */
    QMI8658_TAP_PRIO_ZXY    = 4,    /**< Z > X > Y */
    QMI8658_TAP_PRIO_ZYX    = 5,    /**< Z > Y > X */
} qmi8658_tap_prio_t;

/**
 * @brief   QMI8658 operating modes
 */
typedef enum {
    QMI8658_POWER_DOWN,         /**< All functional blocks off, minimal power consumption */
    QMI8658_NORMAL_ACC,         /**< Accelerometer only mode (if ODR <= 1kHz, otherwise Gyro is also enabled)*/
    QMI8658_NORMAL_GYRO,        /**< Gyroscope only mode */
    QMI8658_NORMAL_ACC_GYRO,    /**< IMU mode (Accelerometer + Gyroscope) */
    QMI8658_LOWPWR_ACC          /**< Low power mode (Accelerometer only) */
} qmi8658_mode_t;

/**
 * @brief   QMI8658 axis identifiers
 */
typedef enum {
    QMI8658_X_AXIS  = 1,        /**< Sensor X axis */
    QMI8658_Y_AXIS  = 2,        /**< Sensor Y axis */
    QMI8658_Z_AXIS  = 3,        /**< Sensor Z axis */
} qmi8658_axis_t;

/**
 * @brief   Enable sensor flags
 */
typedef enum {
    QMI8658_DISABLE_ALL = 0,
    QMI8658_ENABLE_ACC  = 1,
    QMI8658_ENABLE_GYRO = 2
} qmi8658_enable_flag_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c;                                  /**< i2c bus */
    uint8_t addr;                               /**< i2c address */
    qmi8658_odr_t acc_odr;                      /**< accelerometer output data rate */
    qmi8658_acc_lowpwr_odr_t acc_lowpwr_odr;    /**< accelerometer low power output data rate */
    qmi8658_odr_t gyro_odr;                     /**< gyroscope output data rate */
    qmi8658_acc_fs_t acc_fs;                    /**< accelerometer full scale */
    qmi8658_gyro_fs_t gyro_fs;                  /**< gyroscope full scale */
} qmi8658_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    qmi8658_params_t params;            /**< Device initialization parameters */
    bool initialized;                   /**< Device is initialized */
    qmi8658_enable_flag_t enable_flags; /**< Currently enabled sensors */
} qmi8658_t;

/**
 * @brief   3D output data
 */
typedef struct {
    int16_t x;  /**< X axis */
    int16_t y;  /**< Y axis */
    int16_t z;  /**< Z axis */
} qmi8658_3d_data_t;

/**
 * @brief   Tap detection data
 */
typedef struct {
    /** Number of taps detected: 0 (no tap), 1 (single tap) or 2 (double tap)*/
    uint8_t num;
    /** Axis on which the tap was detected */
    qmi8658_axis_t axis;
    /** Tap was detected in the negative direction of tap axis (otherwise positive direction) */
    bool neg_polarity;
} qmi8658_tap_data_t;

/**
 * @brief   Initialize the given device
 *
 * @param[in,out]    dev        Device descriptor of the driver
 * @param[in]       params     Initialization parameters
 *
 * @retval  0 on success
 * @retval  -EIO on error
 */
int qmi8658_init(qmi8658_t *dev, const qmi8658_params_t *params);

/**
 * @brief   Set the operating mode of the device
 *
 * @param[in,out]    dev         Device descriptor of the driver
 * @param[in]       mode        New mode
 *
 * @retval  0 on success
 * @retval  -EPERM if driver was never initialized
 * @retval  -EINVAL if invalid mode is given
 * @retval  -EIO on i2c communication error
 */
int qmi8658_set_mode(qmi8658_t *dev, qmi8658_mode_t mode);

/**
 * @brief   Enable the tap detection engine
 *
 * @note    Tap detection requires the accelerometer to be enabled in normal mode
 *          (@ref QMI8658_NORMAL_ACC or @ref QMI8658_NORMAL_ACC_GYRO).<br>
 *          This function can be called before or after @ref qmi8658_set_mode, tap
 *          data is only generated once the accelerometer is active.<br>
 *          Accelerometer ODR is recommended to be set higher than 200Hz.
 *
 * @param[in]   dev         Device descriptor of the driver
 *
 * @retval  0 on success
 * @retval  -EPERM if driver was never initialized
 * @retval  -EIO on i2c communication error
 */
int qmi8658_enable_tap(const qmi8658_t *dev);

/**
 * @brief   Disable the tap detection engine
 *
 * @param[in]   dev         Device descriptor of the driver
 *
 * @retval  0 on success
 * @retval  -EIO on i2c communication error
 */
int qmi8658_disable_tap(const qmi8658_t *dev);

/**
 * @brief   Read accelerometer data in mg
 *
 * @param[in]   dev     Device descriptor of the driver
 * @param[out]  data    Accelerometer data buffer
 *
 * @retval 0 on success
 * @retval -EIO on i2c communication error
 */
int qmi8658_read_acc(const qmi8658_t *dev, qmi8658_3d_data_t *data);

/**
 * @brief   Read gyroscope data in dps
 *
 * @param[in]   dev     Device descriptor of the driver
 * @param[out]  data    Gyroscope data buffer
 *
 * @retval 0 on success
 * @retval -EIO on i2c communication error
 */
int qmi8658_read_gyro(const qmi8658_t *dev, qmi8658_3d_data_t *data);

/**
 * @brief   Read temperature data in degC x 100
 *
 * @param[in]   dev     Device descriptor of the driver
 * @param[out]  data    Temperature data buffer
 *
 * @retval 0 on success
 * @retval -EIO on i2c communication error
 */
int qmi8658_read_temp(const qmi8658_t *dev, int16_t *data);

/**
 * @brief   Read tap detection data
 *
 * @param[in]   dev     Device descriptor of the driver
 * @param[out]  data    Tap detection data buffer
 *
 * @retval 0 on success
 * @retval -EIO on i2c communication error
 */
int qmi8658_read_tap(const qmi8658_t *dev, qmi8658_tap_data_t *data);

#ifdef __cplusplus
}
#endif

/** @} */
