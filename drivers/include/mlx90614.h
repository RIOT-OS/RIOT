/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mlx90614 MLX90614 temperature sensor
 * @ingroup     drivers_sensors
 *
 * @brief       Device Driver for MLX90614 Melexis Temperature Sensor
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @file
 * @{
 */

#ifndef MLX90614_H
#define MLX90614_H

#include <stdint.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief MLX90614 configuration parameters
 */
typedef struct {
    i2c_t bus;                      /**< I2C Bus the sensor is connected to */
    uint8_t addr;                   /**< I2C Address of the sensor */
} mlx90614_params_t;

/**
 * @brief MLX90614 device descriptor
 */
typedef struct {
    mlx90614_params_t params;       /**< Configuration parameters */
} mlx90614_t;

/**
 * @brief MLX90614 temperature channels
 */
typedef enum {
    MLX90614_TEMP_AMBIENT = 0x6,    /**< Ambient Temperature  */
    MLX90614_TEMP_OBJ1    = 0x7,    /**< Object temperature 1 */
    MLX90614_TEMP_OBJ2    = 0x8,    /**< Object temperature 2 */
} mlx90614_chan_t;

/**
 * @brief Initialize MLX90614 sensor
 *
 * @param[out] dev      sensor device
 * @param[in] params    configuration parameter
 */
void mlx90614_init(mlx90614_t *dev, const mlx90614_params_t *params);

/**
 * @brief Read a Temperature channel
 *
 * @param[in] dev   sensor device
 * @param[in] chan  temperature channel
 * @param[out] temp temperature in cK
 *
 * @return 0 on success, I2C error otherwise
 */
int mlx90614_read_temperature(const mlx90614_t *dev, mlx90614_chan_t chan, uint32_t *temp);

#ifdef __cplusplus
}
#endif

#endif /* MLX90614_H */
/** @} */
