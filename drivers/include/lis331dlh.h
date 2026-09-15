/*
 * SPDX-FileCopyrightText: 2026 Arseny Sukhman
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_lis331dlh LIS331DLH accelerometer
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the ST LIS331DLH 3-axis accelerometer
 *
 * This driver provides basic polling support for LIS331DLH accelerometers over
 * I2C. It verifies the WHO_AM_I register during initialization, configures the
 * device for normal mode with all three axes enabled, and reads acceleration
 * values as milli-g.
 *
 * Supported features are the 2 g, 4 g, and 8 g full-scale ranges, the 50 Hz,
 * 100 Hz, 400 Hz, and 1000 Hz output data rates, and @ref drivers_saul
 * integration. Interrupts, click/wake-up detection, and power-management modes
 * other than the basic normal mode are not implemented.
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
 * @name    LIS331DLH registers and bit fields
 * @{
 */
#define LIS331DLH_REG_WHO_AM_I      (0x0f)  /**< WHO_AM_I register */
#define LIS331DLH_REG_CTRL1         (0x20)  /**< control register 1 */
#define LIS331DLH_REG_CTRL2         (0x21)  /**< control register 2 */
#define LIS331DLH_REG_CTRL3         (0x22)  /**< control register 3 */
#define LIS331DLH_REG_CTRL4         (0x23)  /**< control register 4 */
#define LIS331DLH_REG_CTRL5         (0x24)  /**< control register 5 */
#define LIS331DLH_REG_OUT_X_L       (0x28)  /**< X axis low byte register */
#define LIS331DLH_I2C_AUTO_INC      (0x80)  /**< auto-increment I2C address */
#define LIS331DLH_CTRL1_NORMAL_MODE (0x20)  /**< normal mode */
#define LIS331DLH_CTRL1_XYZ_ENABLE  (0x07)  /**< enable X, Y, and Z axes */
#define LIS331DLH_CTRL1_ODR_SHIFT   (3)     /**< output data rate bit shift */
#define LIS331DLH_CTRL4_BDU         (0x80)  /**< block data update */
#define LIS331DLH_CTRL4_FS_2G       (0x00)  /**< 2 g full-scale range */
#define LIS331DLH_CTRL4_FS_4G       (0x10)  /**< 4 g full-scale range */
#define LIS331DLH_CTRL4_FS_8G       (0x30)  /**< 8 g full-scale range */
/** @} */

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
 * @retval 0 on success
 * @retval -ENODEV if the device ID does not match
 * @retval -EIO on I2C error
 * @retval -EINVAL on invalid parameters
 */
int lis331dlh_init(lis331dlh_t *dev, const lis331dlh_params_t *params);

/**
 * @brief   Read acceleration data
 *
 * @param[in]  dev      device descriptor
 * @param[out] data     acceleration vector in milli-g
 *
 * @retval 0 on success
 * @retval -EIO on I2C error
 */
int lis331dlh_read(const lis331dlh_t *dev, lis331dlh_data_t *data);

/**
 * @brief   Set full-scale range
 *
 * @param[in,out] dev   device descriptor
 * @param[in]     scale full-scale range
 *
 * @retval 0 on success
 * @retval -EIO on I2C error
 * @retval -EINVAL on invalid scale
 */
int lis331dlh_set_scale(lis331dlh_t *dev, lis331dlh_scale_t scale);

/**
 * @brief   Set output data rate
 *
 * @param[in,out] dev   device descriptor
 * @param[in]     odr   output data rate
 *
 * @retval 0 on success
 * @retval -EIO on I2C error
 * @retval -EINVAL on invalid data rate
 */
int lis331dlh_set_odr(lis331dlh_t *dev, lis331dlh_odr_t odr);

#ifdef __cplusplus
}
#endif

/** @} */
