/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_LSM6DS3 LSM6DS3 3D accelerometer/gyroscope
 * @ingroup     drivers
 * @brief       Device driver for the LSM6DS3 3D accelerometer/gyroscope
 * @{
 *
 * @file
 * @brief       Device driver interface for the LSM6DS3 3D accelerometer/gyroscope
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 */

#ifndef LSM6DS3_H
#define LSM6DS3_H

#include <stdint.h>
#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 3d data container
 */
typedef struct {
    int16_t x_axis;
    int16_t y_axis;
    int16_t z_axis;
} lsm6ds3_3d_data_t;

/**
 * @brief Device descriptor for LSM6DS3 sensors
 */
typedef struct {
    i2c_t i2c;              /** I2C device descriptor */
    uint8_t addr;           /** address of the device */
} lsm6ds3_t;

/**
 * @brief Possible accelerometer sample rates
 */
typedef enum {
    LSM6DS3_ACC_SAMPLE_RATE_13HZ   = 0x10,  /**   13Hz sample rate */
    LSM6DS3_ACC_SAMPLE_RATE_26HZ   = 0x20,  /**   26Hz sample rate */
    LSM6DS3_ACC_SAMPLE_RATE_52HZ   = 0x30,  /**   52Hz sample rate */
    LSM6DS3_ACC_SAMPLE_RATE_104HZ  = 0x40,  /**  104Hz sample rate */
    LSM6DS3_ACC_SAMPLE_RATE_208HZ  = 0x50,  /**  208Hz sample rate */
    LSM6DS3_ACC_SAMPLE_RATE_416HZ  = 0x60,  /**  416Hz sample rate */
    LSM6DS3_ACC_SAMPLE_RATE_833HZ  = 0x70,  /**  833Hz sample rate */
    LSM6DS3_ACC_SAMPLE_RATE_1660HZ = 0x80,  /** 1660Hz sample rate */
    LSM6DS3_ACC_SAMPLE_RATE_3330HZ = 0x90,  /** 3330Hz sample rate */
    LSM6DS3_ACC_SAMPLE_RATE_6660HZ = 0xA0,  /** 6660Hz sample rate */
} lsm6ds3_acc_sample_rate_t;

/**
 * @brief Possible accelerometer scales
 */
typedef enum {
    LSM6DS3_ACC_SCALE_2G  = 0x00, /** +-  2g range */
    LSM6DS3_ACC_SCALE_4G  = 0x02, /** +-  4g range */
    LSM6DS3_ACC_SCALE_8G  = 0x03, /** +-  8g range */
    LSM6DS3_ACC_SCALE_16G = 0x01, /** +- 16g range */
} lsm6ds3_acc_scale_t;

/**
 * @brief Possible accelerometer bandwith
 */
typedef enum {
    LSM6DS3_ACC_BANDWITH_50HZ  = 0x03, /** 50Hz  Anti-aliasing */
    LSM6DS3_ACC_BANDWITH_100HZ = 0x02, /** 100Hz Anti-aliasing */
    LSM6DS3_ACC_BANDWITH_200HZ = 0x01, /** 200Hz Anti-aliasing */
    LSM6DS3_ACC_BANDWITH_400HZ = 0x00, /** 400Hz Anti-aliasing */
} lsm6ds3_acc_bandwith_t;

/**
 * @brief Possible gyroscope sample rates
 */
typedef enum {
    LSM6DS3_GYR_SAMPLE_RATE_13HZ   = 0x10,  /**   13Hz sample rate */
    LSM6DS3_GYR_SAMPLE_RATE_26HZ   = 0x20,  /**   26Hz sample rate */
    LSM6DS3_GYR_SAMPLE_RATE_52HZ   = 0x30,  /**   52Hz sample rate */
    LSM6DS3_GYR_SAMPLE_RATE_104HZ  = 0x40,  /**  104Hz sample rate */
    LSM6DS3_GYR_SAMPLE_RATE_208HZ  = 0x50,  /**  208Hz sample rate */
    LSM6DS3_GYR_SAMPLE_RATE_416HZ  = 0x60,  /**  416Hz sample rate */
    LSM6DS3_GYR_SAMPLE_RATE_833HZ  = 0x70,  /**  833Hz sample rate */
    LSM6DS3_GYR_SAMPLE_RATE_1660HZ = 0x80,  /** 1660Hz sample rate */
} lsm6ds3_gyr_sample_rate_t;

/**
 * @brief Possible gyroscope scales
 */
typedef enum {
    LSM6DS3_GYR_SCALE_125DPS  = 0x02, /**  245 degrees per second */
    LSM6DS3_GYR_SCALE_245DPS  = 0x00, /**  245 degrees per second */
    LSM6DS3_GYR_SCALE_500DPS  = 0x04, /**  500 degrees per second */
    LSM6DS3_GYR_SCALE_1000DPS = 0x08, /** 1000 degrees per second */
    LSM6DS3_GYR_SCALE_2000DPS = 0x0C, /** 2000 degrees per second */
} lsm6ds3_gyr_scale_t;

/**
 * @brief Data structure holding the full set of configuration parameters
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus the device is connected to */
    uint8_t addr;               /**< address on that bus */
    lsm6ds3_acc_sample_rate_t acc_sample_rate;
    lsm6ds3_acc_scale_t acc_scale;
    lsm6ds3_gyr_sample_rate_t gyr_sample_rate;
    lsm6ds3_gyr_scale_t gyr_scale;
} lsm6ds3_params_t;

/**
 * @brief Initialize a new LSM6DS3 device.
 *
 * @param[in] dev               device descriptor of an LSM6DS3 device
 * @param[in] i2c               I2C device the sensor is connected to
 * @param[in] address           address of the device
 * @param[in] acc_sample_rate   accelerometer sample rate
 * @param[in] acc_scale         accelerometer scale
 * @param[in] gyr_sample_rate   gyroscope sample rate
 * @param[in] gyr_scale         gyroscope scale
 *
 * @return                       0 on success
 * @return                      -1 on error
 */
int lsm6ds3_init(lsm6ds3_t *dev,
                 i2c_t i2c,
                 uint8_t address,
                 lsm6ds3_acc_sample_rate_t acc_sample_rate,
                 lsm6ds3_acc_scale_t acc_scale,
                 lsm6ds3_gyr_sample_rate_t gyr_sample_rate,
                 lsm6ds3_gyr_scale_t gyr_scale);

/**
 * @brief Read a accelerator value from the sensor.
 *
 * @param[in] dev   device descriptor of an LSM6DS3 device
 * @param[in] data  the measured accelerometer data in milli degrees per second [mdps]
 */
void lsm6ds3_read_acc(lsm6ds3_t *dev, lsm6ds3_3d_data_t *data);

/**
 * @brief Read a gyroscope value from the sensor.
 *
 * @param[in] dev   device descriptor of an LSM6DS3 device
 * @param[in] data  the measured gyroscope data in milligauss [mG]
 */
void lsm6ds3_read_gyr(lsm6ds3_t *dev, lsm6ds3_3d_data_t *data);

/**
 * @brief Read a temperature value from the sensor.
 *
 * @param[in] dev   device descriptor of an LSM6DS3 device
 * @param[in] data  the measured temperature in degree celsius [°C]
 */
void lsm6ds3_read_temp(lsm6ds3_t *dev, int16_t *data);

/**
 * @brief Enables the sensor.
 *
 * @param[in] dev   device descriptor of an LSM6DS3 device
 */
void lsm6ds3_enable(lsm6ds3_t *dev);

/**
 * @brief Disables the sensor.
 *
 * @param[in] dev   device descriptor of an LSM6DS3 device
 */
void lsm6ds3_disable(lsm6ds3_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* LSM6DS3_H */
/** @} */
