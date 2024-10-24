/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lis3mdl
 *
 * @{
 * @file
 * @brief       Default configuration for LIS3MDL devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LIS3MDL_PARAMS_H
#define LIS3MDL_PARAMS_H

#include "board.h"
#include "lis3mdl.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef LIS3MDL_PARAM_I2C
#define LIS3MDL_PARAM_I2C           (I2C_DEV(0))
#endif
#ifndef LIS3MDL_PARAM_ADDR
#define LIS3MDL_PARAM_ADDR          (0x1E)
#endif
#ifndef LIS3MDL_PARAM_XYMODE
#define LIS3MDL_PARAM_XYMODE        (LIS3MDL_XY_MODE_HIGH)
#endif
#ifndef LIS3MDL_PARAM_ZMODE
#define LIS3MDL_PARAM_ZMODE         (LIS3MDL_Z_MODE_HIGH)
#endif
#ifndef LIS3MDL_PARAM_ODR
#define LIS3MDL_PARAM_ODR           (LIS3MDL_ODR_10Hz)
#endif
#ifndef LIS3MDL_PARAM_SCALE
#define LIS3MDL_PARAM_SCALE         (4)
#endif
#ifndef LIS3MDL_PARAM_OPMODE
#define LIS3MDL_PARAM_OPMODE        (LIS3MDL_OP_CONT_CONV)
#endif

#ifndef LIS3MDL_PARAMS
#define LIS3MDL_PARAMS               { .i2c     = LIS3MDL_PARAM_I2C,    \
                                       .addr    = LIS3MDL_PARAM_ADDR,   \
                                       .xy_mode = LIS3MDL_PARAM_XYMODE, \
                                       .z_mode  = LIS3MDL_PARAM_ZMODE,  \
                                       .odr     = LIS3MDL_PARAM_ODR,  \
                                       .scale   = LIS3MDL_PARAM_SCALE,    \
                                       .op_mode = LIS3MDL_PARAM_OPMODE }
#endif
#ifndef LIS3MDL_SAUL_INFO
#define LIS3MDL_SAUL_INFO            { .name = "lis3mdl" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const lis3mdl_params_t lis3mdl_params[] =
{
    LIS3MDL_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lis3mdl_saul_info[] =
{
    LIS3MDL_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LIS3MDL_PARAMS_H */
