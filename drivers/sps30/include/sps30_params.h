/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sps30
 * @brief       Default configuration for Sensirion SPS30 sensors devices
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @file
 * @{
 */

#ifndef SPS30_PARAMS_H
#define SPS30_PARAMS_H

#include "board.h"
#include "sps30.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPS30 default configuration parameters
 * @{
 */
#ifndef SPS30_PARAM_I2C_DEV
#define SPS30_PARAM_I2C_DEV  (I2C_DEV(0))
#endif

#ifndef SPS30_PARAMS
#define SPS30_PARAMS         { .i2c_dev  = SPS30_PARAM_I2C_DEV }
#endif
#ifndef SPS30_SAUL_INFO
#define SPS30_SAUL_INFO      { .name = "sps30" }
#endif
/**@}*/

/**
 * @brief   SPS30 configuration
 */
static const sps30_params_t sps30_params[] =
{
    SPS30_PARAMS
};

/**
 * @brief   Define the number of configured sensors
 */
#define SPS30_NUM               ARRAY_SIZE(sps30_params)

#ifdef __cplusplus
}
#endif

#endif /* SPS30_PARAMS_H */
/** @} */
