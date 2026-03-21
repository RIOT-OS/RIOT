/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "qmi8658.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef QMI8658_PARAM_I2C
/** I2C device */
#  define QMI8658_PARAM_I2C                 I2C_DEV(0)
#endif
#ifndef QMI8658_PARAM_ADDR
/** I2C slave address. Set to 0x6A if SDO/SA0 is pulled high */
#  define QMI8658_PARAM_ADDR                CONFIG_QMI8658_PARAM_I2C
#endif
#ifndef QMI8658_PARAM_ACC_ODR
/** Acceleromter output data rate.*/
#  define QMI8658_PARAM_ACC_ODR             QMI8658_DATA_RATE_1KHZ
#endif
#ifndef QMI8658_PARAM_ACC_LOWPWR_ODR
/** Acceleromter output data rate in low power mode.*/
#  define QMI8658_PARAM_ACC_LOWPWR_ODR      QMI8658_ACC_LOWPWR_DATA_RATE_21HZ
#endif
#ifndef QMI8658_PARAM_ACC_FS
/** Acceleromter full scale */
#  define QMI8658_PARAM_ACC_FS              QMI8658_ACC_FS_2G
#endif
#ifndef QMI8658_PARAM_GYRO_ODR
/** Gyroscope output data rate */
#  define QMI8658_PARAM_GYRO_ODR            QMI8658_DATA_RATE_1KHZ
#endif
#ifndef QMI8658_PARAM_GYRO_FS
/** Gyroscope full scale */
#  define QMI8658_PARAM_GYRO_FS             QMI8658_GYRO_FS_256DPS
#endif

#ifndef QMI8658_PARAMS
/** Configuration parameter set */
#  define QMI8658_PARAMS      {   .i2c = QMI8658_PARAM_I2C,            \
                                .addr = QMI8658_PARAM_ADDR,           \
                                .acc_odr = QMI8658_PARAM_ACC_ODR,        \
                                .acc_lowpwr_odr = QMI8658_PARAM_ACC_LOWPWR_ODR, \
                                .gyro_odr = QMI8658_PARAM_GYRO_ODR,       \
                                .acc_fs = QMI8658_PARAM_ACC_FS,         \
                                .gyro_fs = QMI8658_PARAM_GYRO_FS, }
#endif
#ifndef QMI8658_SAUL_INFO
/** Information for the SAUL registry */
#  define QMI8658_SAUL_INFO            { .name = "qmi8658" }
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const qmi8658_params_t qmi8658_params[] =
{
    QMI8658_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t qmi8658_saul_info[] =
{
    QMI8658_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
