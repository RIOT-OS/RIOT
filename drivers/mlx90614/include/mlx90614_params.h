/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mlx90614
 *
 * @{
 * @file
 * @brief       Default configuration parameters for the MLX90614 sensor.
 *
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#ifndef MLX90614_PARAMS_H
#define MLX90614_PARAMS_H

#include "mlx90614.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MLX90614_PARAM_I2C_BUS
#define MLX90614_PARAM_I2C_BUS  I2C_DEV(0)  /**< Default I2C device to use */
#endif

#ifndef MLX90614_PARAM_I2C_ADDR
#define MLX90614_PARAM_I2C_ADDR (0x5A)      /**< Default I2C address */
#endif

/**
 * @brief   Default MLX90614 configuration array
 */
#ifndef MLX90614_PARAMS
#define MLX90614_PARAMS {                                   \
                        .bus = MLX90614_PARAM_I2C_BUS,      \
                        .addr = MLX90614_PARAM_I2C_ADDR,    \
                        }
#endif

/**
 * @brief MLX90614 power-up configuration
 */
static const mlx90614_params_t mlx90614_params[] =
{
    MLX90614_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* MLX90614_PARAMS_H */
/** @} */
