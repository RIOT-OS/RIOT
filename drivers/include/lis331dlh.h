/*
 * SPDX-FileCopyrightText: 2026 Arseny Sukhman
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once


/**
 * @defgroup    drivers_lis331dlh LIS331DLH accelerometer
 * @ingroup     drivers_sensors
 * @brief       Device driver for the ST LIS331DLH 3-axis accelerometer
 * @{
 *
 * @file
 * @brief       Device driver interface for the LIS331DLH accelerometer
 */


#include <stdint.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LIS331DLH I2C address when SDO/SA0 is connected to GND
 */
#define LIS331DLH_I2C_ADDR_0        (0x18)

/**
 * @brief   LIS331DLH I2C address when SDO/SA0 is connected to VDD
 */
#define LIS331DLH_I2C_ADDR_1        (0x19)

/**
 * @brief   Expected value of the WHO_AM_I register
 */
#define LIS331DLH_WHO_AM_I          (0x32)

/**
 * @brief   LIS331DLH full-scale ranges
 */
typedef enum {
    LIS331DLH_SCALE_2G = 0,     /**< +/- 2 g */
    LIS331DLH_SCALE_4G,         /**< +/- 4 g */
    LIS331DLH_SCALE_8G,         /**< +/- 8 g */
} lis331dlh_scale_t;

/**
 * @brief   LIS331DLH output data rates in normal mode
 */
typedef enum {
    LIS331DLH_ODR_50HZ = 0,     /**< 50 Hz */
    LIS331DLH_ODR_100HZ,        /**< 100 Hz */
    LIS331DLH_ODR_400HZ,        /**< 400 Hz */
    LIS331DLH_ODR_1000HZ,       /**< 1000 Hz */
} lis331dlh_odr_t;

/**
 * @brief   LIS331DLH initialization parameters
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus */
    uint8_t addr;               /**< I2C address */
    lis331dlh_scale_t scale;    /**< full-scale range */
    lis331dlh_odr_t odr;        /**< output data rate */
} lis331dlh_params_t;

/**
 * @brief   LIS331DLH device descriptor
 */
typedef struct {
    lis331dlh_params_t params;  /**< device configuration */
    int16_t sensitivity;        /**< sensitivity in mg/digit */
} lis331dlh_t;

/**
 * @brief   Acceleration vector in milli-g
 */
typedef struct {
    int16_t x;                  /**< X axis acceleration in mg */
    int16_t y;                  /**< Y axis acceleration in mg */
    int16_t z;                  /**< Z axis acceleration in mg */
} lis331dlh_data_t;

/**
 * @brief   Initialize a LIS331DLH device
 *
 * @param[out] dev      device descriptor
 * @param[in]  params   device parameters
 *
 * @return 0 on success
 * @return -ENODEV if the device ID does not match
 * @return -EIO on I2C error
 * @return -EINVAL on invalid parameters
 */
int lis331dlh_init(lis331dlh_t *dev, const lis331dlh_params_t *params);

/**
 * @brief   Read acceleration data
 *
 * @param[in]  dev      device descriptor
 * @param[out] data     acceleration vector in milli-g
 *
 * @return 0 on success
 * @return -EIO on I2C error
 */
int lis331dlh_read(const lis331dlh_t *dev, lis331dlh_data_t *data);

/**
 * @brief   Set full-scale range
 *
 * @param[in,out] dev   device descriptor
 * @param[in]     scale full-scale range
 *
 * @return 0 on success
 * @return -EIO on I2C error
 * @return -EINVAL on invalid scale
 */
int lis331dlh_set_scale(lis331dlh_t *dev, lis331dlh_scale_t scale);

/**
 * @brief   Set output data rate
 *
 * @param[in] dev       device descriptor
 * @param[in] odr       output data rate
 *
 * @return 0 on success
 * @return -EIO on I2C error
 * @return -EINVAL on invalid data rate
 */
int lis331dlh_set_odr(const lis331dlh_t *dev, lis331dlh_odr_t odr);

#ifdef __cplusplus
}
#endif

/** @} */
