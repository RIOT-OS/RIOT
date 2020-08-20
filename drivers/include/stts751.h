/*
 * Copyright (C) 2020 Université Grenoble Alpes
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_stts751 STTS751 I2C Temperature Sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver for the STTS751 Temperature Sensor
 *
 * @{
 *
 * @author      Pierre Millot
 * @file        stts751.h
 */

#ifndef STTS751_H
#define STTS751_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdbool.h"
#include "periph/i2c.h"

#include "stts751_regs.h"

/**
 * @brief   STTS751 device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C device */
    uint16_t i2c_addr;          /**< I2C address of device */
} stts751_params_t;

/**
 * @brief   STTS751 PWM device data structure type
 */
typedef struct {
    stts751_params_t params;     /**< Device initialization parameters */
} stts751_t;

/**
 * @brief Initialization.
 *
 * @param[in] dev       Device descriptor of the STTS751
 * @param[in] params    Parameters for device initialization
 *
 * @return  STTS751_OK on success
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
int stts751_init(stts751_t *dev, const stts751_params_t *params);


/**
 * @brief Get the temperature
 *
 * @param[in] dev       Device descriptor of the STTS751
 *
 * @return the temperature in Celcius
 */
double stts751_get_temperature(const stts751_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* STTS751_H */
/** @} */
