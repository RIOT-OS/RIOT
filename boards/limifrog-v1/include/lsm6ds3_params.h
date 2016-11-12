/*
 * Copyright (C) 2016 Hamburg University of Applied Science
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_limifrog-v1
 * @{
 *
 * @file
 * @brief     LSM6DS3 board specific configuration
 *
 * @author    René Herthel <rene.herthel@haw-hamburg.de>
 */

 #ifndef LSM6DS3_PARAMS_H
 #define LSM6DS3_PARAMS_H

 #include "board.h"

 #ifdef __cplusplus
 extern "C" {
 #endif

/**
 * @brief    LSM6DS3 parameter configuration
 */
static const lsm6ds3_params_t lsm6ds3_params[] =
{
    {
          .i2c = LSM6DS3_I2C,
          .addr = LSM6DS3_ADDR,
          .acc_sample_rate = LSM6DS3_ACC_SAMPLE_RATE_416HZ,
          .acc_scale = LSM6DS3_ACC_SCALE_2G,
          .gyr_sample_rate = LSM6DS3_GYR_SAMPLE_RATE_416HZ,
          .gyr_scale = LSM6DS3_GYR_SCALE_245DPS,
    },
};

 #ifdef __cplusplus
 }
 #endif

 #endif /* LSM6DS3_PARAMS_H */
 /** @} */
