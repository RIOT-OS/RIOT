/*
 * Copyright (C) 2026 Technische Universität Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_qmi8658 QMI8658 IMU
 * @ingroup     drivers_sensors
 * @brief       Device driver for the QMI8658 6D MEMS inertial measurement unit (IMU)
 *
 * @{
 *
 * @file
 *
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 */

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Enable sensor flags
 */
enum {
    QMI8658_DISABLE_ALL = 0,
    QMI8658_ENABLE_ACC  = 1,
    QMI8658_ENABLE_GYRO = 2
};

/**
 * @brief   Data rate settings
 * Actual data rates are slightly lower when gyroscope is enabled. 
 * If the selected accelerometer ODR is higher than 1kHz, enabling
 * the accelerometer will implicitly enable the gyroscope as required 
 * by hardware design.
 */
enum {
    QMI8658_DATA_RATE_8KHZ     = 0x00,
    QMI8658_DATA_RATE_4KHZ     = 0x01,
    QMI8658_DATA_RATE_2KHZ     = 0x02,
    QMI8658_DATA_RATE_1KHZ     = 0x03,
    QMI8658_DATA_RATE_500HZ    = 0x04,
    QMI8658_DATA_RATE_250HZ    = 0x05,
    QMI8658_DATA_RATE_125HZ    = 0x06,
    QMI8658_DATA_RATE_62_5HZ   = 0x07,
    QMI8658_DATA_RATE_31_25HZ  = 0x08,
};

/**
 * @brief   Accelerometer full scale
 */
enum {
    QMI8658_ACC_FS_2G  = 0,
    QMI8658_ACC_FS_4G,
    QMI8658_ACC_FS_8G,
    QMI8658_ACC_FS_16G
};

/**
 * @brief   Gyroscope full scale
 */
enum {
    QMI8658_GYRO_FS_16DPS    = 0,
    QMI8658_GYRO_FS_32DPS,
    QMI8658_GYRO_FS_64DPS,
    QMI8658_GYRO_FS_128DPS,
    QMI8658_GYRO_FS_256DPS,
    QMI8658_GYRO_FS_512DPS,
    QMI8658_GYRO_FS_1024DPS,
    QMI8658_GYRO_FS_2048DPS
};

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c;                  /**< i2c bus */
    uint8_t addr;               /**< i2c address */
    uint8_t acc_odr;            /**< accelerometer output data rate */
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
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @return                  0 on success
 * @return                  -EIO on error
 */
int qmi8658_init(qmi8658_t *dev, const qmi8658_params_t *params);

/**
 * @brief   Enable or disable the sensors of the device
 *
 * @param[inout] dev                Device descriptor of the driver
 * @param[in]    sensor_flags       Bitstring containing flags of all sensors that should be enabled 
 *                                  Options: (QMI8658_ENABLE_ACC, QMI8658_ENABLE_GYRO, QMI8658_DISABLE_ALL)
 *
 * @warning                         Sensors that are not explicity given in @p sensor_flags will be disabled
 *
 * @return                          0 on success
 * @return                          -EIO on success
 */
int qmi8658_enable_sensors(qmi8658_t* dev, uint8_t sensor_flags);

/**
 * @brief   Read accelerometer data in mg
 *
 * @param[in] dev    Device descriptor of the driver
 * @param[out] data  Accelerometer data buffer
 *
 * @return 0 on success
 * @return -EIO on i2c communication error
 * @return -ENODATA if no new data available
 */
int qmi8658_read_acc(const qmi8658_t *dev, qmi8658_3d_data_t *data);

/**
 * @brief   Read gyroscope data in dps
 *
 * @param[in] dev    Device descriptor of the driver
 * @param[out] data  Gyroscope data buffer
 *
 * @return 0 on success
 * @return -EIO on i2c communication error
 * @return -ENODATA if no new data available
 */
int qmi8658_read_gyro(const qmi8658_t *dev, qmi8658_3d_data_t *data);

/**
 * @brief   Read temperature data in degC x 100
 *
 * @param[in] dev    Device descriptor of the driver
 * @param[out] data  Temperature data buffer
 *
 * @return 0 on success
 * @return -EIO on i2c communication error
 * @return -ENODATA if no new data available
 */
int qmi8658_read_temp(const qmi8658_t *dev, int16_t *data);

#ifdef __cplusplus
}
#endif

/** @} */
