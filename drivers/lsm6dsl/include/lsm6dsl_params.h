/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_lsm6dsl
 * @{
 *
 * @file
 * @brief       Default configuration for LSM6DSL devices
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 */

#ifndef LSM6DSL_PARAMS_H
#define LSM6DSL_PARAMS_H

#include "board.h"
#include "lsm6dsl.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef LSM6DSL_PARAM_I2C
#define LSM6DSL_PARAM_I2C            I2C_DEV(0)
#endif
#ifndef LSM6DSL_PARAM_ADDR
#define LSM6DSL_PARAM_ADDR           (0x6B) /* (0x6A) */
#endif
#ifndef LSM6DSL_PARAM_ACC_ODR
#define LSM6DSL_PARAM_ACC_ODR        (LSM6DSL_DATA_RATE_52HZ)
#endif
#ifndef LSM6DSL_PARAM_GYRO_ODR
#define LSM6DSL_PARAM_GYRO_ODR       (LSM6DSL_DATA_RATE_52HZ)
#endif
#ifndef LSM6DSL_PARAM_ACC_FS
#define LSM6DSL_PARAM_ACC_FS         (LSM6DSL_ACC_FS_2G)
#endif
#ifndef LSM6DSL_PARAM_GYRO_FS
#define LSM6DSL_PARAM_GYRO_FS        (LSM6DSL_GYRO_FS_245DPS)
#endif
#ifndef LSM6DSL_PARAM_ACC_FIFO_DEC
#define LSM6DSL_PARAM_ACC_FIFO_DEC   (LSM6DSL_DECIMATION_NO)
#endif
#ifndef LSM6DSL_PARAM_GYRO_FIFO_DEC
#define LSM6DSL_PARAM_GYRO_FIFO_DEC  (LSM6DSL_DECIMATION_NO)
#endif

#ifndef LSM6DSL_PARAMS
#define LSM6DSL_PARAMS               { .i2c             = LSM6DSL_PARAM_I2C,          \
                                       .addr            = LSM6DSL_PARAM_ADDR,         \
                                       .acc_odr         = LSM6DSL_PARAM_ACC_ODR,      \
                                       .gyro_odr        = LSM6DSL_PARAM_GYRO_ODR,     \
                                       .acc_fs          = LSM6DSL_PARAM_ACC_FS,       \
                                       .gyro_fs         = LSM6DSL_PARAM_GYRO_FS,      \
                                       .acc_decimation  = LSM6DSL_PARAM_ACC_FIFO_DEC, \
                                       .gyro_decimation = LSM6DSL_PARAM_GYRO_FIFO_DEC }
#endif
#ifndef LSM6DSL_SAUL_INFO
#define LSM6DSL_SAUL_INFO            { .name = "lsm6dsl" }
#endif
/** @} */

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const lsm6dsl_params_t lsm6dsl_params[] =
{
    LSM6DSL_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lsm6dsl_saul_info[] =
{
    LSM6DSL_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* LSM6DSL_PARAMS_H */
/** @} */
