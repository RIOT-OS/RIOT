/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_lis2dh12
 *
 * @{
 * @file
 * @brief       Default configuration for LIS2DH12 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#include "board.h"
#include "lis2dh12.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for LIS2DH12 devices
 * @{
 */
#ifdef MODULE_LIS2DH12_SPI          /* default configuration for SPI mode */
#ifndef LIS2DH12_PARAM_SPI
#define LIS2DH12_PARAM_SPI          SPI_DEV(0)
#endif
#ifndef LIS2DH12_PARAM_CS
#define LIS2DH12_PARAM_CS           GPIO_PIN(0, 0)
#endif
#define LIS2DH12_PARAMS_BUSCFG      .spi = LIS2DH12_PARAM_SPI, \
                                    .cs  = LIS2DH12_PARAM_CS

#else                               /* default configuration for I2C mode */
#ifndef LIS2DH12_PARAM_I2C
#define LIS2DH12_PARAM_I2C          I2C_DEV(0)
#endif
#ifndef LIS2DH12_PARAM_ADDR
#define LIS2DH12_PARAM_ADDR         LIS2DH12_ADDR_DEFAULT
#endif
#define LIS2DH12_PARAMS_BUSCFG      .i2c  = LIS2DH12_PARAM_I2C, \
                                    .addr = LIS2DH12_PARAM_ADDR

#endif

#ifndef LIS2DH12_PARAM_INT_PIN1
#define LIS2DH12_PARAM_INT_PIN1     GPIO_UNDEF
#endif

#ifndef LIS2DH12_PARAM_INT_PIN2
#define LIS2DH12_PARAM_INT_PIN2     GPIO_UNDEF
#endif

#ifndef LIS2DH12_PARAM_SCALE
#define LIS2DH12_PARAM_SCALE        LIS2DH12_SCALE_2G
#endif
#ifndef LIS2DH12_PARAM_RATE
#define LIS2DH12_PARAM_RATE         LIS2DH12_RATE_100HZ
#endif
#ifndef LIS2DH12_PARAM_RESOLUTION
#define LIS2DH12_PARAM_RESOLUTION   LIS2DH12_POWER_NORMAL
#endif

#ifndef LIS2DH12_PARAMS
#ifdef MODULE_LIS2DH12_INT
#define LIS2DH12_PARAMS         {  \
                                    LIS2DH12_PARAMS_BUSCFG,  \
                                    .int1_pin = LIS2DH12_PARAM_INT_PIN1,  \
                                    .int2_pin = LIS2DH12_PARAM_INT_PIN2,  \
                                    .scale = LIS2DH12_PARAM_SCALE,  \
                                    .rate  = LIS2DH12_PARAM_RATE,  \
                                    .resolution = LIS2DH12_PARAM_RESOLUTION,  \
                                }
#else /* MODULE_LIS2DH12_INT */
#define LIS2DH12_PARAMS         {  \
                                    LIS2DH12_PARAMS_BUSCFG,  \
                                    .scale = LIS2DH12_PARAM_SCALE,  \
                                    .rate  = LIS2DH12_PARAM_RATE,  \
                                    .resolution = LIS2DH12_PARAM_RESOLUTION,  \
                                }
#endif /* MODULE_LIS2DH12_INT */
#endif /* LIS2DH12_PARAMS */

#ifndef LIS2DH12_SAULINFO
#define LIS2DH12_SAULINFO           { .name = "lis2dh12" }
#endif
/**@}*/

/**
 * @brief   LIS2DH12 configuration
 */
static const lis2dh12_params_t lis2dh12_params[] =
{
    LIS2DH12_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lis2dh12_saul_info[] =
{
    LIS2DH12_SAULINFO
};

#ifdef __cplusplus
}
#endif

/** @} */
