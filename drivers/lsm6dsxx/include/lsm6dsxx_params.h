/*
 * Copyright (C) 2017 OTA keys S.A.
 * Copyright (C) 2024 HAW Hamburg.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#pragma once

/**
 * @ingroup     drivers_lsm6dsxx
 * @{
 *
 * @file
 * @brief       Default configuration for LSM6DSXX devices
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Miquel Borrell <miquel.borrell@haw-hamburg.de>
 *
 */


#include "board.h"
#include "lsm6dsxx.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef LSM6DSXX_PARAM_I2C
#define LSM6DSXX_PARAM_I2C            I2C_DEV(0)
#endif
#ifndef LSM6DSXX_PARAM_ADDR
#define LSM6DSXX_PARAM_ADDR           (0x6B) /* (0x6A) */
#endif
#ifndef LSM6DSXX_PARAM_ACC_ODR
#define LSM6DSXX_PARAM_ACC_ODR        (LSM6DSXX_DATA_RATE_52HZ)
#endif
#ifndef LSM6DSXX_PARAM_GYRO_ODR
#define LSM6DSXX_PARAM_GYRO_ODR       (LSM6DSXX_DATA_RATE_52HZ)
#endif
#ifndef LSM6DSXX_PARAM_ACC_FS
#define LSM6DSXX_PARAM_ACC_FS         (LSM6DSXX_ACC_FS_2G)
#endif
#ifndef LSM6DSXX_PARAM_GYRO_FS
#define LSM6DSXX_PARAM_GYRO_FS        (LSM6DSXX_GYRO_FS_245DPS)
#endif
#ifndef LSM6DSXX_PARAM_ACC_FIFO_DEC
#define LSM6DSXX_PARAM_ACC_FIFO_DEC   (LSM6DSXX_DECIMATION_NO)
#endif
#ifndef LSM6DSXX_PARAM_GYRO_FIFO_DEC
#define LSM6DSXX_PARAM_GYRO_FIFO_DEC  (LSM6DSXX_DECIMATION_NO)
#endif

#ifndef LSM6DSXX_PARAMS
#define LSM6DSXX_PARAMS               { .i2c             = LSM6DSXX_PARAM_I2C,          \
                                        .addr            = LSM6DSXX_PARAM_ADDR,         \
                                        .acc_odr         = LSM6DSXX_PARAM_ACC_ODR,      \
                                        .gyro_odr        = LSM6DSXX_PARAM_GYRO_ODR,     \
                                        .acc_fs          = LSM6DSXX_PARAM_ACC_FS,       \
                                        .gyro_fs         = LSM6DSXX_PARAM_GYRO_FS,      \
                                        .acc_decimation  = LSM6DSXX_PARAM_ACC_FIFO_DEC, \
                                        .gyro_decimation = LSM6DSXX_PARAM_GYRO_FIFO_DEC }
#endif
#ifndef LSM6DSXX_SAUL_INFO
#define LSM6DSXX_SAUL_INFO            { .name = "lsm6dsxx" }
#endif
/** @} */

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const lsm6dsxx_params_t lsm6dsxx_params[] =
{
    LSM6DSXX_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lsm6dsxx_saul_info[] =
{
    LSM6DSXX_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
