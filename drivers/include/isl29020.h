/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_isl29020 ISL29020 light sensor
 * @ingroup     drivers
 * @brief       Device driver for the ISL29020 light sensor
 * @{
 *
 * @file
 * @brief       Device driver interface for the ISL29020 light sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __ISL29020_H
#define __ISL29020_H

#include <stdint.h>
#include "periph/i2c.h"

 /**
  * @brief The sensors default I2C address
  */
#define ISL29020_DEFAULT_ADDRESS        0x44

/**
 * @brief Device descriptor for ISL29020 sensors
 */
typedef struct {
    i2c_t i2c;                  /**< I2C device the sensor is connected to */
    uint8_t address;            /**< I2C bus address of the sensor */
    float lux_fac;              /**< factor to calculate actual LUX value */
} isl29020_t;

/**
 * @brief Possible modes for the ISR29020 sensor
 */
typedef enum {
    ISL29020_MODE_AMBIENT = 0,  /**< set sensor to detect ambient light */
    ISL29020_MODE_IR      = 1   /**< set sensor to detect infrared light */
} isl29020_mode_t;

/**
 * @brief Possible range values for the ISR29020 sensor
 */
typedef enum {
    ISL29020_RANGE_1K = 0,      /**< set range to 0-1000 LUX */
    ISL29020_RANGE_4K = 1,      /**< set range to 0-4000 LUX */
    ISL29020_RANGE_16K = 2,     /**< set range to 0-16000 LUX */
    ISL29020_RANGE_64K = 3      /**< set range to 0-64000 LUX */
} isl29020_range_t;

/**
 * @brief Initialize a new ISL29020 device
 *
 * @param[in] dev       device descriptor of an ISL29020 device
 * @param[in] i2c       I2C device the sensor is connected to
 * @param[in] address   I2C address of the sensor
 * @param[in] range     measurement range
 * @param[in] mode      configure if sensor reacts to ambient or infrared light
 *
 * @return              0 on success
 * @return              -1 on error
 */
int isl29020_init(isl29020_t *dev, i2c_t i2c, uint8_t address,
                  isl29020_range_t range, isl29020_mode_t mode);

/**
 * @brief Read a lighting value from the sensor, the result is given in LUX
 *
 * @param[in] dev       device descriptor of an ISL29020 device
 *
 * @return              the measured brightness in LUX
 * @return              -1 on error
 */
int isl29020_read(isl29020_t *dev);

/**
 * @brief Enable the given sensor
 *
 * @param[in] dev       device descriptor of an ISL29020 device
 *
 * @return              0 on success
 * @return              -1 on error
 */
int isl29020_enable(isl29020_t *dev);

/**
 * @brief Disable the given sensor
 *
 * @param[in] dev       device descriptor of an ISL29020 device
 *
 * @return              0 on success
 * @return              -1 on error
 */
int isl29020_disable(isl29020_t *dev);

#endif /* __ISL29020_H */
/** @} */
