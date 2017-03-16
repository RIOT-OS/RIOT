/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lis3dh
 *
 * @{
 * @file
 * @brief       Default configuration for LIS3DH devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LIS3DH_PARAMS_H
#define LIS3DH_PARAMS_H

#include "board.h"
#include "lis3dh.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters
 * @{
 */
#ifndef LIS3DH_PARAM_SPI
#define LIS3DH_PARAM_SPI            (SPI_DEV(0))
#endif
#ifndef LIS3DH_PARAM_CS
#define LIS3DH_PARAM_CS             (GPIO_PIN(0, 0))
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

#define LIS3DH_PARAMS_DEFAULT       { .spi   = LIS3DH_PARAM_SPI, \
                                      .cs    = LIS3DH_PARAM_CS, \
                                      .int1  = LIS3DH_PARAM_INT1, \
                                      .int2  = LIS3DH_PARAM_INT2, \
                                      .scale = LIS3DH_PARAM_SCALE, \
                                      .odr   = LIS3DH_PARAM_ODR }
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const lis3dh_params_t lis3dh_params[] =
{
#ifdef LIS3DH_PARAMS_CUSTOM
    LIS3DH_PARAMS_CUSTOM,
#else
    LIS3DH_PARAMS_DEFAULT,
#endif
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lis3dh_saul_info[] =
{
    { .name = "lis3dh" }
};

#ifdef __cplusplus
}
#endif

#endif /* LIS3DH_PARAMS_H */
/** @} */
