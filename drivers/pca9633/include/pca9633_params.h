/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_actuators
 * @brief       Default configuration for the PCA9633 I2C PWM controller
 *
 * @{
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 */

#include <stdbool.h>

#include "periph/i2c.h"

#include "pca9633_regs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @name    Set default configuration parameters
* @{
*/
#ifndef PCA9633_PARAM_I2C_DEV
/** I2C device is I2C_DEV(0) */
#define PCA9633_PARAM_I2C_DEV               I2C_DEV(0)
#endif

#ifndef PCA9633_PARAM_I2C_ADDR
/** I2C address of device is (0xc0 >> 1) */
#define PCA9633_PARAM_I2C_ADDR              (0xc0 >> 1)
#endif

#ifndef PCA9633_PARAM_REG_PWM_RED
/** Register for red color is PCA9633_REG_PWM2 */
#define PCA9633_PARAM_REG_PWM_RED           PCA9633_REG_PWM2
#endif

#ifndef PCA9633_PARAM_REG_PWM_GREEN
/** Register for green color is PCA9633_REG_PWM1 */
#define PCA9633_PARAM_REG_PWM_GREEN         PCA9633_REG_PWM1
#endif

#ifndef PCA9633_PARAM_REG_PWM_BLUE
/** Register for blue color is PCA9633_REG_PWM0 */
#define PCA9633_PARAM_REG_PWM_BLUE          PCA9633_REG_PWM0
#endif

#ifndef PCA9633_PARAM_REG_PWM_AMBER
/** Register for amber color is not given (0) */
#define PCA9633_PARAM_REG_PWM_AMBER         0
#endif

#ifndef PCA9633_PARAM_HAS_AMBER_CHANNEL
/** PCA9633 has no connected channel for amber (false) */
#define PCA9633_PARAM_HAS_AMBER_CHANNEL     false
#endif

#ifndef PCA9633_PARAMS
#define PCA9633_PARAMS                                          \
    {                                                           \
        .i2c_dev = PCA9633_PARAM_I2C_DEV,                       \
        .i2c_addr = PCA9633_PARAM_I2C_ADDR,                     \
        .reg_pwm_red = PCA9633_PARAM_REG_PWM_RED,               \
        .reg_pwm_green = PCA9633_PARAM_REG_PWM_GREEN,           \
        .reg_pwm_blue = PCA9633_PARAM_REG_PWM_BLUE,             \
        .reg_pwm_amber = PCA9633_PARAM_REG_PWM_AMBER,           \
        .has_amber_channel = PCA9633_PARAM_HAS_AMBER_CHANNEL    \
    }
#endif /* PCA9633_PARAMS */
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const pca9633_params_t pca9633_params[] =
{
        PCA9633_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
