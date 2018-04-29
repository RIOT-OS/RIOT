/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sens0
 * @brief       Sens0 I2C API
 *
 * Sens0 sensor specifications can be used with multiple interface types, this
 * API is for using sensors connected via I2C.
 *
 * @{
 *
 * @file
 * @brief       Sens0 I2C API declarations
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef SENS0_I2C_H
#define SENS0_I2C_H

#include <stdint.h>
#include "periph/i2c.h"
#include "sens0.h"
#include "phydat.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   I2C connection parameters
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus the device is connected to */
    uint8_t addr;               /**< I2C bus address of the device */
} sens0_i2c_params_t;

/**
 * @brief   Device descriptor for Sens0 sensors on I2C
 */
typedef struct {
    const sens0_specs_t *specs;         /**< device hardware specification */
    const sens0_i2c_params_t *params;   /**< device configuration parameters */
} sens0_i2c_t;

/**
 * @brief   Initialize an I2C connected Sens0 device
 *
 * @param[in]  dev      device descriptor of the device to initialize
 *
 * @return              0 on success
 * @return              <0 on failure
 * @return              -EINVAL if initialization of I2C bus failed
 * @return              -ENODEV if the hardware identification failed
 * @return              -EIO if there was a communication problem
 */
int sens0_i2c_init(const sens0_i2c_t *dev);

/**
 * @brief   Read sensor data
 *
 * @param[in]  dev          device descriptor
 * @param[out] data         output data
 * @param[in]  iout         output index, must be < `dev->specs->num_outputs`
 *
 * @return              0 on success
 * @return              <0 on failure
 * @return              -EIO if there was a communication problem
 */
int sens0_i2c_read(const sens0_i2c_t *dev, phydat_t *data, uint8_t iout);

/**
 * @brief   Bring the device out of standby
 *
 * This will configure the sensor for active measurements
 *
 * @param[out] dev          device descriptor
 */
void sens0_i2c_set_active(const sens0_i2c_t *dev);

/**
 * @brief   Put the device in standby
 *
 * Depending on the sensor, this may reduce power consumption.
 *
 * @param[in]  dev          device descriptor
 */
void sens0_i2c_set_standby(const sens0_i2c_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* SENS0_I2C_H */
/** @} */
