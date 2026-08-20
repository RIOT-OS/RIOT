/*
 * SPDX-FileCopyrightText: 2026 Anastasiia
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
 * @brief       Interface definition for the LIS331DLH accelerometer
 */

#include <stdint.h>

#include "periph/i2c.h"
#include "saul.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default I2C address of the LIS331DLH sensor
 */
#define LIS331DLH_ADDR_DEFAULT   (0x18)

/**
 * @brief   LIS331DLH full-scale configuration
 */
typedef enum {
    LIS331DLH_SCALE_2G = 0x00,  /**< +-2 g full-scale range */
    LIS331DLH_SCALE_4G = 0x10,  /**< +-4 g full-scale range */
    LIS331DLH_SCALE_8G = 0x30,  /**< +-8 g full-scale range */
} lis331dlh_scale_t;

/**
 * @brief   LIS331DLH output data rate configuration
 */
typedef enum {
    LIS331DLH_RATE_50HZ   = 0x00,  /**< 50 Hz output data rate */
    LIS331DLH_RATE_100HZ  = 0x08,  /**< 100 Hz output data rate */
    LIS331DLH_RATE_400HZ  = 0x10,  /**< 400 Hz output data rate */
    LIS331DLH_RATE_1000HZ = 0x18,  /**< 1000 Hz output data rate */
} lis331dlh_rate_t;

/**
 * @brief   LIS331DLH initialization parameters
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus the sensor is connected to */
    uint8_t addr;               /**< I2C address of the sensor */
    lis331dlh_scale_t scale;    /**< Full-scale configuration */
    lis331dlh_rate_t rate;      /**< Output data rate configuration */
} lis331dlh_params_t;

/**
 * @brief   LIS331DLH device descriptor
 */
typedef struct {
    const lis331dlh_params_t *params;  /**< Device initialization parameters */
} lis331dlh_t;

/**
 * @brief   LIS331DLH acceleration data
 */
typedef struct {
    int16_t x;                  /**< Raw X-axis acceleration value */
    int16_t y;                  /**< Raw Y-axis acceleration value */
    int16_t z;                  /**< Raw Z-axis acceleration value */
} lis331dlh_data_t;

/**
 * @brief   Initialize a LIS331DLH sensor instance
 *
 * @param[out] dev      Device descriptor to initialize
 * @param[in]  params   Device initialization parameters
 *
 * @return  0 on success
 * @return  Negative error code on failure
 */
int lis331dlh_init(lis331dlh_t *dev, const lis331dlh_params_t *params);

/**
 * @brief   Read raw acceleration data from the LIS331DLH sensor
 *
 * @param[in]  dev      Device descriptor
 * @param[out] data     Acceleration data output buffer
 *
 * @return  0 on success
 * @return  Negative error code on failure
 */
int lis331dlh_read_xyz(const lis331dlh_t *dev, lis331dlh_data_t *data);

/**
 * @brief   SAUL driver for LIS331DLH
 */
extern const saul_driver_t lis331dlh_saul_driver;

#ifdef __cplusplus
}
#endif

/** @} */
