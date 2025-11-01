/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_lis3dh
 *
 * @{
 * @file
 * @brief       Default configuration for LIS3DH devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "board.h"
#include "lis3dh.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef LIS3DH_PARAM_SPI
#define LIS3DH_PARAM_SPI            (SPI_DEV(0))
#endif
#ifndef LIS3DH_PARAM_CS
#define LIS3DH_PARAM_CS             (GPIO_PIN(0, 0))
#endif
#ifndef LIS3DH_PARAM_CLK
#define LIS3DH_PARAM_CLK            (SPI_CLK_5MHZ)
#endif
#ifndef LIS3DH_PARAM_INT1
#define LIS3DH_PARAM_INT1           (GPIO_PIN(0, 1))
#endif
#ifndef LIS3DH_PARAM_INT2
#define LIS3DH_PARAM_INT2           (GPIO_PIN(0, 2))
#endif
#ifndef LIS3DH_PARAM_SCALE
#define LIS3DH_PARAM_SCALE          (4)
#endif
#ifndef LIS3DH_PARAM_ODR
#define LIS3DH_PARAM_ODR            (LIS3DH_ODR_100Hz)
#endif

#ifndef LIS3DH_PARAMS
#define LIS3DH_PARAMS               { .spi   = LIS3DH_PARAM_SPI,   \
                                      .cs    = LIS3DH_PARAM_CS,    \
                                      .clk   = LIS3DH_PARAM_CLK,   \
                                      .int1  = LIS3DH_PARAM_INT1,  \
                                      .int2  = LIS3DH_PARAM_INT2,  \
                                      .scale = LIS3DH_PARAM_SCALE, \
                                      .odr   = LIS3DH_PARAM_ODR }
#endif
#ifndef LIS3DH_SAUL_INFO
#define LIS3DH_SAUL_INFO            { .name = "lis3dh" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const lis3dh_params_t lis3dh_params[] =
{
    LIS3DH_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lis3dh_saul_info[] =
{
    LIS3DH_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
