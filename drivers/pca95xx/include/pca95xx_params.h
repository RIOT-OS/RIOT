/*
 * Copyright (C) 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pca95xx
 * @{
 *
 * @file
 * @brief       Default configuration for PCA95xx devices
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef PCA95XX_PARAMS_H
#define PCA95XX_PARAMS_H

#include "board.h"
#include "pca95xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the ADCXX1C driver
 * @{
 */
#ifndef PCA95XX_PARAM_I2C
#define PCA95XX_PARAM_I2C        (I2C_DEV(0))
#endif
#ifndef PCA95XX_PARAM_ADDR
#define PCA95XX_PARAM_ADDR       (PCA95XX_I2C_ADDRESS)
#endif
#ifndef PCA95XX_PARAM_INT_PIN
#define PCA95XX_PARAM_INT_PIN    (GPIO_UNDEF)
#endif
#ifndef PCA95XX_PARAM_DFLAGS
#define PCA95XX_PARAM_DFLAGS     (PCA95XX_16PIN_DEV)
#endif

#ifndef PCA95XX_PARAMS
#define PCA95XX_PARAMS        { .i2c          = PCA95XX_PARAM_I2C,     \
                                .addr         = PCA95XX_PARAM_ADDR,    \
                                .int_pin      = PCA95XX_PARAM_INT_PIN, \
                                .dflags       = PCA95XX_PARAM_DFLAGS }
#endif
/** @} */

/**
 * @brief   PCA95xx configuration
 */
static const pca95xx_params_t pca95xx_params[] =
{
    PCA95XX_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* PCA95XX_PARAMS_H */
/** @} */
