/*
 * Copyright (C) 2026 Technische Universität Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_qmi8658
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 */

#include "board.h"
#include "qmi8658.h"
#include "qmi8658_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef QMI8658_PARAM_I2C
#define QMI8658_PARAM_I2C                   I2C_DEV(0)                              /**< I2C device */
#endif
#ifndef QMI8658_PARAM_ADDR
#define QMI8658_PARAM_ADDR                  0x6B                                    /**< I2C slave address. Set to 0x6A if SDO/SA0 is pulled high */
#endif
#ifndef QMI8658_PARAM_ACC_ODR
#define QMI8658_PARAM_ACC_ODR               QMI8658_DATA_RATE_1KHZ                  /**< Acceleromter output data rate.*/
#endif
#ifndef QMI8658_PARAM_ACC_LOWPWR_ODR
#define QMI8658_PARAM_ACC_LOWPWR_ODR        QMI8658_ACC_LOWPWR_DATA_RATE_21HZ       /**< Acceleromter output data rate in low power mode.*/
#endif
#ifndef QMI8658_PARAM_ACC_FS
#define QMI8658_PARAM_ACC_FS                QMI8658_ACC_FS_2G                       /**< Acceleromter full scale */
#endif
#ifndef QMI8658_PARAM_GYRO_ODR
#define QMI8658_PARAM_GYRO_ODR              QMI8658_DATA_RATE_1KHZ                  /**< Gyroscope output data rate */
#endif
#ifndef QMI8658_PARAM_GYRO_FS
#define QMI8658_PARAM_GYRO_FS               QMI8658_GYRO_FS_256DPS                  /**< Gyroscope full scale */
#endif

#ifndef QMI8658_PARAMS
#define QMI8658_PARAMS      {   .i2c                = QMI8658_PARAM_I2C,            \
                                .addr               = QMI8658_PARAM_ADDR,           \
                                .acc_odr            = QMI8658_PARAM_ACC_ODR,        \
                                .acc_lowpwr_odr     = QMI8658_PARAM_ACC_LOWPWR_ODR, \
                                .gyro_odr           = QMI8658_PARAM_GYRO_ODR,       \
                                .acc_fs             = QMI8658_PARAM_ACC_FS,         \
                                .gyro_fs            = QMI8658_PARAM_GYRO_FS,}
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const qmi8658_params_t qmi8658_params[] =
{
    QMI8658_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
