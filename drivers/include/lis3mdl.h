/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef LIS3MDL_H
#define LIS3MDL_H

/**
 * @defgroup    drivers_lis3mdl LIS3MDL 3-axis magnetometer
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the LIS3MDL 3-axis magnetometer
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Device driver interface for the LIS3MDL 3-axis magnetometer
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 */

#include <stdint.h>
#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   3d data container of the LIS3MDL sensor
 */
typedef struct {
    int16_t x_axis;                  /**< Magnometer data from x-axis */
    int16_t y_axis;                  /**< Magnometer data from y_axis */
    int16_t z_axis;                  /**< Magnometer data from z_axis */
} lis3mdl_3d_data_t;

/**
 * @brief   Operating mode of x- and y-axis for LIS3MDL
 */
typedef enum {
    LIS3MDL_XY_MODE_LOW    = 0x00,   /**< Low-power mode */
    LIS3MDL_XY_MODE_MEDIUM = 0x20,   /**< Medium-performance mode */
    LIS3MDL_XY_MODE_HIGH   = 0x40,   /**< High-performance mode */
    LIS3MDL_XY_MODE_ULTRA  = 0x60,   /**< Ultra-High-performance mode */
} lis3mdl_xy_mode_t;

/**
 * @brief   Operating mode of z-axis for LIS3MDL
 */
typedef enum {
    LIS3MDL_Z_MODE_LOW    = 0x00,    /**< Low-power mode */
    LIS3MDL_Z_MODE_MEDIUM = 0x04,    /**< Medium-performance mode */
    LIS3MDL_Z_MODE_HIGH   = 0x08,    /**< High-performance mode */
    LIS3MDL_Z_MODE_ULTRA  = 0x0C,    /**< Ultra-High-performance mode */
} lis3mdl_z_mode_t;

/**
 * @brief   Output data rate [Hz] for LIS3MDL
 */
typedef enum {
    LIS3MDL_ODR_0_625Hz = 0x00,      /**<  0.625Hz */
    LIS3MDL_ODR_1_25Hz  = 0x04,      /**<  1.250Hz */
    LIS3MDL_ODR_2_5Hz   = 0x08,      /**<  5.000Hz */
    LIS3MDL_ODR_10Hz    = 0x10,      /**< 10.000Hz */
    LIS3DML_ODR_20HZ    = 0x14,      /**< 20.000Hz */
    LIS3DML_ODR_40HZ    = 0x18,      /**< 40.000Hz */
    LIS3MDL_ODR_80HZ    = 0x1C,      /**< 80.000Hz */
} lis3mdl_odr_t;

/**
 * @brief   Scale [gauss] for LIS3MDL
 */
typedef enum {
    LIS3MDL_SCALE_4G  = 0x00,        /**< +-  4 gauss */
    LIS3MDL_SCALE_8G  = 0x20,        /**< +-  8 gauss */
    LIS3MDL_SCALE_12G = 0x40,        /**< +- 12 gauss */
    LIS3MDL_SCALE_16G = 0x60,        /**< +- 16 gauss */
} lis3mdl_scale_t;

/**
 * @brief   Operating modes
 */
typedef enum {
    LIS3MDL_OP_CONT_CONV = 0x00,     /**< Continous-conversion mode */
    LIS3MDL_OP_SNGL_CONV = 0x01,     /**< Single-conversion mode */
    LIS3MDL_OP_PDOWN     = 0x11,     /**< Power-down mode */
} lis3mdl_op_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c;                       /**< I2C device */
    uint8_t addr;                    /**< Magnometer I2C address */
    lis3mdl_xy_mode_t xy_mode;       /**< Power mode of x- and y-axis */
    lis3mdl_z_mode_t z_mode;         /**< Power mode of z-axis */
    lis3mdl_odr_t odr;               /**< Output data rate */
    lis3mdl_scale_t scale;           /**< Scale factor */
    lis3mdl_op_t op_mode;            /**< Operation mode */
} lis3mdl_params_t;

/**
 * @brief   Device descriptor for LIS3MDL sensor
 */
typedef struct {
    lis3mdl_params_t params;         /**< Initialization parameters */
} lis3mdl_t;

/**
 * @brief   Initialize a new LIS3DML device.
 *
 * @param[in] dev          device descriptor of LIS3MDL
 * @param[in] params       initialization parameters
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int lis3mdl_init(lis3mdl_t *dev, const lis3mdl_params_t *params);

/**
 * @brief   Reads the magnometer value of LIS3MDL.
 *
 * @param[in] dev          device descriptor of LIS3MDL
 * @param[in] data         measured magnetometer data
 */
void lis3mdl_read_mag(const lis3mdl_t *dev, lis3mdl_3d_data_t *data);

/**
 * @brief   Reads the temperature value of LIS3MDL.
 *
 * @param[in] dev          device descriptor of LIS3MDL
 * @param[in] value        measured temperature in degree celsius
 */
void lis3mdl_read_temp(const lis3mdl_t *dev, int16_t *value);

/**
 * @brief   Enable the LIS3MDL device.
 *
 * @param[in] dev          device descriptor of LIS3MDL
 */
void lis3mdl_enable(const lis3mdl_t *dev);

/**
 * @brief   Disable the LIS3MDL device.
 *
 * @param[in] dev          device descriptor of LIS3MDL
 */
void lis3mdl_disable(const lis3mdl_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LIS3MDL_H */
