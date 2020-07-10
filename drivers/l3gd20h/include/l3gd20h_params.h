/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_l3gd20h
 * @brief       Default configuration for ST L3GD20H devices
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef L3GD20H_PARAMS_H
#define L3GD20H_PARAMS_H

#include "board.h"
#include "l3gd20h.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#if MODULE_L3GD20H_I2C
#ifndef L3GD20H_PARAM_DEV
#define L3GD20H_PARAM_DEV          I2C_DEV(0)
#endif
#ifndef L3GD20H_PARAM_ADDR
#define L3GD20H_PARAM_ADDR         (L3GD20H_I2C_ADDRESS_2)
#endif
#endif

#if MODULE_L3GD20H_SPI
#ifndef L3GD20H_PARAM_DEV
#define L3GD20H_PARAM_DEV          SPI_DEV(0)
#endif
#ifndef L3GD20H_PARAM_CLK
#define L3GD20H_PARAM_CLK          (SPI_CLK_1MHZ)
#endif
#ifndef L3GD20H_PARAM_CS
#define L3GD20H_PARAM_CS           (GPIO_PIN(0, 1))
#endif
#endif

#ifndef L3GD20H_PARAM_INT1
#define L3GD20H_PARAM_INT1         (GPIO_PIN(0, 0))
#endif
#ifndef L3GD20H_PARAM_INT2
#define L3GD20H_PARAM_INT2         (GPIO_PIN(0, 1))
#endif
#ifndef L3GD20H_PARAM_MODE
#define L3GD20H_PARAM_MODE         (L3GD20H_MODE_50_16)
#endif
#ifndef L3GD20H_PARAM_SCALE
#define L3GD20H_PARAM_SCALE        (L3GD20H_SCALE_245_DPS)
#endif


#if MODULE_L3GD20H_I2C
#ifndef L3GD20H_PARAMS
#define L3GD20H_PARAMS             { \
                                     .dev   = L3GD20H_PARAM_DEV,  \
                                     .addr  = L3GD20H_PARAM_ADDR, \
                                     .int1  = L3GD20H_PARAM_INT1, \
                                     .int2  = L3GD20H_PARAM_INT2, \
                                     .mode  = L3GD20H_PARAM_MODE, \
                                     .scale = L3GD20H_PARAM_SCALE \
                                   }
#endif
#endif

#if MODULE_L3GD20H_SPI
#ifndef L3GD20H_PARAMS
#define L3GD20H_PARAMS             { \
                                     .dev   = L3GD20H_PARAM_DEV,  \
                                     .clk   = L3GD20H_PARAM_CLK,  \
                                     .cs    = L3GD20H_PARAM_CS,   \
                                     .int1  = L3GD20H_PARAM_INT1, \
                                     .int2  = L3GD20H_PARAM_INT2, \
                                     .mode  = L3GD20H_PARAM_MODE, \
                                     .scale = L3GD20H_PARAM_SCALE \
                                   }
#endif
#endif


#ifndef L3GD20H_SAUL_INFO
#define L3GD20H_SAUL_INFO          { .name = "l3gd20h" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const l3gd20h_params_t l3gd20h_params[] =
{
    L3GD20H_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t l3gd20h_saul_info[] =
{
    L3GD20H_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* L3GD20H_PARAMS_H */
/** @} */
