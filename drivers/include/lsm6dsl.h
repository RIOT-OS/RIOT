/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @defgroup    drivers_lsm6dsl LSM6DSL 3D accelerometer/gyroscope
 * @ingroup     drivers_sensors
 * @brief       Device driver for the LSM6DSL 3D accelerometer/gyroscope
 *
 * @{
 * @file
 * @brief       Device driver interface for the LSM6DSL 3D accelerometer/gyroscope.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef LSM6DSL_H
#define LSM6DSL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/i2c.h"

/** Data rate */
enum {
    LSM6DSL_DATA_RATE_POWER_DOWN = 0x0,
    LSM6DSL_DATA_RATE_1_6HZ      = 0xB,
    LSM6DSL_DATA_RATE_12_5HZ     = 0x1,
    LSM6DSL_DATA_RATE_26HZ       = 0x2,
    LSM6DSL_DATA_RATE_52HZ       = 0x3,
    LSM6DSL_DATA_RATE_104HZ      = 0x4,
    LSM6DSL_DATA_RATE_208HZ      = 0x5,
    LSM6DSL_DATA_RATE_416HZ      = 0x6,
    LSM6DSL_DATA_RATE_833HZ      = 0x7,
    LSM6DSL_DATA_RATE_1_66KHZ    = 0x8,
    LSM6DSL_DATA_RATE_3_33KHZ    = 0x9,
    LSM6DSL_DATA_RATE_6_66KHZ    = 0xa,
};

/** Decimation */
enum {
    LSM6DSL_DECIMATION_NOT_IN_FIFO = 0,
    LSM6DSL_DECIMATION_NO          = 1,
    LSM6DSL_DECIMATION_2           = 2,
    LSM6DSL_DECIMATION_3           = 3,
    LSM6DSL_DECIMATION_4           = 4,
    LSM6DSL_DECIMATION_8           = 5,
    LSM6DSL_DECIMATION_16          = 6,
    LSM6DSL_DECIMATION_32          = 7,
};

/** Accelerometer full scale */
enum {
    LSM6DSL_ACC_FS_2G  = 0,
    LSM6DSL_ACC_FS_4G  = 2,
    LSM6DSL_ACC_FS_8G  = 3,
    LSM6DSL_ACC_FS_16G = 1,
};

/** Gyroscope full scale */
enum {
    LSM6DSL_GYRO_FS_245DPS    = 0,
    LSM6DSL_GYRO_FS_500DPS    = 1,
    LSM6DSL_GYRO_FS_1000DPS   = 2,
    LSM6DSL_GYRO_FS_2000DPS   = 3,
};

/** LSM6DSL driver parameters */
typedef struct {
    i2c_t i2c;                  /**< i2c bus */
    uint8_t addr;               /**< i2c address */
    uint8_t acc_odr;            /**< accelerometer output data rate */
    uint8_t gyro_odr;           /**< gyroscope output data rate */
    uint8_t acc_fs;             /**< accelerometer full scale */
    uint8_t gyro_fs;            /**< gyroscope full scale */
    uint8_t acc_decimation;     /**< accelerometer decimation */
    uint8_t gyro_decimation;    /**< gyroscope decimation */
} lsm6dsl_params_t;

/** LSM6DSL device descriptor */
typedef struct {
    lsm6dsl_params_t params; /**< driver parameters */
} lsm6dsl_t;

/** 3D output data */
typedef struct {
    int16_t x;  /**< X axis */
    int16_t y;  /**< Y axis */
    int16_t z;  /**< Z axis */
} lsm6dsl_3d_data_t;

/**
 * @brief Initialize a LSM6DSL device
 *
 * @param[in] dev     device to initialize
 * @param[in] params  driver parameters
 *
 * @return 0 on success
 * @return < 0 on error
 */
int lsm6dsl_init(lsm6dsl_t *dev, const lsm6dsl_params_t *params);

/**
 * @brief Read accelerometer data
 *
 * @param[in] dev    device to read
 * @param[out] data  accelerometer values
 *
 * @return 0 on success
 * @return < 0 on error
 */
int lsm6dsl_read_acc(lsm6dsl_t *dev, lsm6dsl_3d_data_t *data);

/**
 * @brief Read gyroscope data
 *
 * @param[in] dev    device to read
 * @param[out] data  gyroscope values
 *
 * @return 0 on success
 * @return < 0 on error
 */
int lsm6dsl_read_gyro(lsm6dsl_t *dev, lsm6dsl_3d_data_t *data);

/**
 * @brief Read temperature data
 *
 * @param[in] dev    device to read
 * @param[out] data  temperature value
 *
 * @return 0 on success
 * @return < 0 on error
 */
int lsm6dsl_read_temp(lsm6dsl_t *dev, int16_t *data);

#ifdef __cplusplus
}
#endif

#endif /* LSM6DSL_H */
/** @} */
