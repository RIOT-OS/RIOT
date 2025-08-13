/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_bmx055
 *
 * @{
 * @file
 * @brief       Default configuration for bmx055 devices
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"

/**
 * @name   Default configuration parameters for device BMX055
 * @{
 */
#ifndef BMX055_PARAM_I2C
#define BMX055_PARAM_I2C        I2C_DEV(0)
#endif
#ifndef BMX055_PARAM_MAG_ADDR
#define BMX055_PARAM_MAG_ADDR   CONFIG_BMX055_MAG_ADDR_DEFAULT
#endif
#ifndef BMX055_PARAM_ACC_ADDR
#define BMX055_PARAM_ACC_ADDR   CONFIG_BMX055_ACC_ADDR_DEFAULT
#endif
#ifndef BMX055_PARAM_GYRO_ADDR
#define BMX055_PARAM_GYRO_ADDR  CONFIG_BMX055_GYRO_ADDR_DEFAULT
#endif
#ifndef BMX055_PARAM_INT1
#define BMX055_PARAM_INT1       GPIO_PIN(0, 0)
#endif
#ifndef BMX055_PARAM_INT2
#define BMX055_PARAM_INT2       GPIO_PIN(0, 1)
#endif
#ifndef BMX055_PARAM_MAG_RATE
#define BMX055_PARAM_MAG_RATE   BMX055_MAG_DRATE_DEFAULT
#endif
#ifndef BMX055_PARAM_ACC_RANGE
#define BMX055_PARAM_ACC_RANGE  BMX055_ACC_RANGE_2G
#endif
#ifndef BMX055_PARAM_GYRO_SCALE
#define BMX055_PARAM_GYRO_SCALE BMX055_GYRO_SCALE_2000DPS
#endif

#ifndef BMX055_PARAMS
#define BMX055_PARAMS                           \
    {   .i2c        = BMX055_PARAM_I2C,         \
        .addr_mag   = BMX055_PARAM_MAG_ADDR,    \
        .addr_acc   = BMX055_PARAM_ACC_ADDR,    \
        .addr_gyro  = BMX055_PARAM_GYRO_ADDR,   \
        .int1_pin   = BMX055_PARAM_INT1,        \
        .int2_pin   = BMX055_PARAM_INT2,        \
        .mag_rate   = BMX055_PARAM_MAG_RATE,    \
        .acc_range  = BMX055_PARAM_ACC_RANGE,   \
        .gyro_scale = BMX055_PARAM_GYRO_SCALE,  \
    }
#endif

#ifndef BMX055_SAULINFO
#define BMX055_SAULINFO                         \
    {   { .name = "Magnetometer (bmx055)"   },  \
        { .name = "Accelerometer (bmx055)"  },  \
        { .name = "Gyroscope (bmx055)"      },  \
    }
#endif
/** @} */

/**
 * @brief   BMX055 configuration
 */
static const bmx055_params_t bmx055_params[] = {
    BMX055_PARAMS
};

/**
 * @brief   SAUL registry entries
 */
static const saul_reg_info_t bmx055_saul_info[][3] = {
    BMX055_SAULINFO
};

#ifdef __cplusplus
}
#endif

/** @} */
