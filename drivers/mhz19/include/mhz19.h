/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mhz19 MH-Z19 CO2 sensor driver
 * @ingroup     drivers_sensors
 * @{
 *
 * @file
 * @brief       Device driver implementation for the Zhengzhou Winsen Electronics Technology MH-Z19 CO2 sensor
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 */

#ifndef MHZ19_H
#define MHZ19_H

#include <stdint.h>
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name mhz19 status return codes
 * @{
 */
#define MHZ19_OK      (0)
#define MHZ19_ERROR  (-1)
/** @} */

/**
 * @brief  Device descriptor for a mhz19 driver
 */
typedef struct {
    gpio_t pin;   /**< Pin the sensor is connected to */
} mhz19_t;

/**
 * @brief  Device initialization parameters
 */
typedef struct {
    gpio_t pin;   /**< Pin the sensor is connected to */
} mhz19_params_t;

/**
 * @brief  Initialize a mhz19 device
 *
 * @param[out] dev        device descriptor
 * @param[in]  params     mhz19 initialization struct
 *
 * @return                0 on success
 * @return               -1 on error
 */
int mhz19_init(mhz19_t *dev, mhz19_params_t *params);


/**
 * @brief  Get a measurement
 *
 * @param[in] dev         device descriptor
 *
 * @return                >=0 ppm of CO2 in the air (max 2000)
 * @return                <0 on error
 */
int16_t mhz19_get_ppm(mhz19_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* MHZ19_H */
/** @} */
