/*
 * Copyright (C) 2026 Technische Universität Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
 * # About
 * This module provides a device driver for the QMI8658 interial measurement unit
 * (IMU) by QST. This driver exposes accelerometer, gyroscope and temperature sensor
 * data in several operating modes of the device (qmi8658_mode_t).
 *
 * # Configuration
 * This driver is configurable via the parameters in qmi8658_params.h:
 * - Acceleromter output data rate (in normal and low power mode)
 * - Acceleromter value full scale
 * - Gyroscope output data rate
 * - Gyroscope value full scale
 *
 * # Usage
 * Use the qmi8658_init() function to initialize the device. This will configure the
 * device but not enable any sensors. Use qmi8658_set_mode() to select an operating
 * for the device, which enables the corresponding sensors in the chosen power mode.
 * Now the sensor values can be polled using the functions qmi8658_read_acc(),
 * qmi8658_read_gyro() and qmi8658_read_temp().
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
 * @defgroup drivers_qmi8658_config     QMI8658 IMU driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
#ifndef CONFIG_QMI8658_PARAM_I2C
#define CONFIG_QMI8658_PARAM_I2C                    0x6B    /**< I2C slave address. Set to 0x6A if SDO/SA0 is pulled high */
#endif

/**
 * @brief   Data rate settings
 *
 * Actual data rates are slightly lower when gyroscope is enabled.
 * If the selected accelerometer ODR is higher than 1kHz, enabling
 * the accelerometer will implicitly enable the gyroscope as required
 * by hardware design.
 */
enum {
    QMI8658_DATA_RATE_8KHZ      = 0x00,
    QMI8658_DATA_RATE_4KHZ      = 0x01,
    QMI8658_DATA_RATE_2KHZ      = 0x02,
    QMI8658_DATA_RATE_1KHZ      = 0x03,
    QMI8658_DATA_RATE_500HZ     = 0x04,
    QMI8658_DATA_RATE_250HZ     = 0x05,
    QMI8658_DATA_RATE_125HZ     = 0x06,
    QMI8658_DATA_RATE_62_5HZ    = 0x07,
    QMI8658_DATA_RATE_31_25HZ   = 0x08,
};

/**
 * @brief   Accelerometer low power data rate settings
 *
 * Available Accelerometer ODRs when low power mode is activated.
 */
enum {
    QMI8658_ACC_LOWPWR_DATA_RATE_128HZ  = 0x0C,
    QMI8658_ACC_LOWPWR_DATA_RATE_21HZ   = 0x0D,
    QMI8658_ACC_LOWPWR_DATA_RATE_11HZ   = 0x0E,
    QMI8658_ACC_LOWPWR_DATA_RATE_3HZ    = 0x0F
};

/**
 * @brief   Accelerometer full scale
 */
enum {
    QMI8658_ACC_FS_2G = 0,
    QMI8658_ACC_FS_4G,
    QMI8658_ACC_FS_8G,
    QMI8658_ACC_FS_16G
};

/**
 * @brief   Gyroscope full scale
 */
enum {
    QMI8658_GYRO_FS_16DPS = 0,
    QMI8658_GYRO_FS_32DPS,
    QMI8658_GYRO_FS_64DPS,
    QMI8658_GYRO_FS_128DPS,
    QMI8658_GYRO_FS_256DPS,
    QMI8658_GYRO_FS_512DPS,
    QMI8658_GYRO_FS_1024DPS,
    QMI8658_GYRO_FS_2048DPS
};

/**
 * @brief   QMI8658 operating modes
 */
typedef enum {
    QMI8658_POWER_DOWN,         /**< All functional blocks off, minimal power consumption */
    QMI8658_NORMAL_ACC,         /**< Accelerometer only mode */
    QMI8658_NORMAL_GYRO,        /**< Gyroscope only mode */
    QMI8658_NORMAL_ACC_GYRO,    /**< IMU mode (Accelerometer + Gyroscope) */
    QMI8658_LOWPWR_ACC          /**< Low power mode (Accelerometer only) */
} qmi8658_mode_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c;                  /**< i2c bus */
    uint8_t addr;               /**< i2c address */
    uint8_t acc_odr;            /**< accelerometer output data rate */
    uint8_t acc_lowpwr_odr;     /**< accelerometer low power output data rate */
    uint8_t gyro_odr;           /**< gyroscope output data rate */
    uint8_t acc_fs;             /**< accelerometer full scale */
    uint8_t gyro_fs;            /**< gyroscope full scale */
} qmi8658_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    /** Device initialization parameters */
    qmi8658_params_t params;
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
 * @brief   Initialize the given device
 *
 * @param[in]    dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @retval  0 on success
 * @retval  -EIO on error
 */
int qmi8658_init(qmi8658_t *dev, const qmi8658_params_t *params);

/**
 * @brief   Set the operating mode of the device
 *
 * @param[in]   dev         Device descriptor of the driver
 * @param[in]   mode        New mode
 *
 * @retval  0 on success
 * @retval  -EIO on on i2c communication error
 */
int qmi8658_set_mode(const qmi8658_t *dev, qmi8658_mode_t mode);

/**
 * @brief   Read accelerometer data in mg
 *
 * @param[in]   dev     Device descriptor of the driver
 * @param[out]  data    Accelerometer data buffer
 *
 * @retval 0 on success
 * @retval -EIO on i2c communication error
 * @retval -ENODATA if no new data available
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

#ifdef __cplusplus
}
#endif

/** @} */
