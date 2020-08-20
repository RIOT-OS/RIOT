/*
 * Copyright (C) 2020 Université Grenoble Alpes
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_stts751
 * @brief       Default configuration for the STTS751 I2C Temperature Sensor
 *
 * @{
 *
 * @author      Pierre Millot
 * @file        stts751_params.h
 */
#ifndef STTS751_PARAMS_H
#define STTS751_PARAMS_H

#include <stdbool.h>

#include "periph/i2c.h"

#include "stts751_regs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef STTS751_PARAM_I2C_DEV
/** I2C device is I2C_DEV(0) */
#define STTS751_PARAM_I2C_DEV               I2C_DEV(0)
#endif

#ifndef STTS751_PARAM_I2C_ADDR
/** I2C address of device is 0x3B */
#define STTS751_PARAM_I2C_ADDR              0x3B
#endif

#ifndef STTS751_PARAMS
#define STTS751_PARAMS                                          \
    {                                                           \
        .i2c_dev = STTS751_PARAM_I2C_DEV,                       \
        .i2c_addr = STTS751_PARAM_I2C_ADDR,                     \
    }
#endif /* STTS751_PARAMS */
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const stts751_params_t stts751_params[] =
{
    STTS751_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* STTS751_PARAMS_H */
/** @} */
