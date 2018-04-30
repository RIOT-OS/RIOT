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
#include "saul_reg.h"
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
#ifndef PCA95XX_PARAM_PIN
#define PCA95XX_PARAM_PIN        (0U)
#endif
#ifndef PCA95XX_PARAM_FLAGS
#define PCA95XX_PARAM_FLAGS      (PCA95XX_16PIN_DEV)
#endif
#ifndef PCA95XX_PARAM_INT_PIN
#define PCA95XX_PARAM_INT_PIN    (GPIO_UNDEF)
#endif

#ifndef PCA95XX_PARAMS
#define PCA95XX_PARAMS        { .i2c          = PCA95XX_PARAM_I2C,  \
                                .addr         = PCA95XX_PARAM_ADDR, \
                                .pin          = PCA95XX_PARAM_PIN,
                                .flags        = PCA95XX_PARAM_FLAGS }
#endif
#ifndef PCA95XX_INT_PARAMS
#define PCA95XX_INT_PARAMS    { .i2c          = PCA95XX_PARAM_I2C,  \
                                .addr         = PCA95XX_PARAM_ADDR, \
                                .int_pin      = PCA95XX_PARAM_ALERT_PIN }
#endif
#ifndef PCA95XX_SAUL_INFO
#define PCA95XX_SAUL_INFO     { .name = "pca95xx" }
#endif
/** @} */

/**
 * @brief   PCA95xx configuration
 */
static const pca95xx_params_t pca95xx_params[] =
{
    PCA95XX_PARAMS
};
static const pca95xx_int_params_t pca95xx_int_params[] =
{
    PCA95XX_INT_PARAMS
};


/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t pca95xx_saul_info[] =
{
    PCA95XX_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* PCA95XX_PARAMS_H */
/** @} */
