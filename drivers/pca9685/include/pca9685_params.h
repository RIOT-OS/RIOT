/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_pca9685
 * @brief       Default configuration for the PCA9685 I2C PWM controller
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include "board.h"
#include "saul_reg.h"
#include "pca9685.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef PCA9685_PARAM_DEV
/** device is I2C_DEV(0) */
#define PCA9685_PARAM_DEV           I2C_DEV(0)
#endif

#ifndef PCA9685_PARAM_ADDR
/** device address is PCA9685_I2C_ADDR */
#define PCA9685_PARAM_ADDR          (PCA9685_I2C_BASE_ADDR + 0)
#endif

#ifndef PCA9685_PARAM_INV
/** Invert outputs: yes */
#define PCA9685_PARAM_INV           (false)
#endif

#ifndef PCA9685_PARAM_MODE
/** PWM mode for all channels: PWM_LEFT */
#define PCA9685_PARAM_MODE          (PWM_LEFT)
#endif

#ifndef PCA9685_PARAM_FREQ
/** PWM frequency in Hz: 100 */
#define PCA9685_PARAM_FREQ          (100)
#endif

#ifndef PCA9685_PARAM_RES
/** PWM resolution: 4096 */
#define PCA9685_PARAM_RES           (4096)
#endif

#ifndef PCA9685_PARAM_OE_PIN
/** Output enable pin: not used */
#define PCA9685_PARAM_OE_PIN        (GPIO_UNDEF)
#endif

#ifndef PCA9685_PARAM_EXT_FREQ
/** EXTCLK frequency and pin: not used */
#define PCA9685_PARAM_EXT_FREQ      (0)
#endif

#ifndef PCA9685_PARAM_OUT_DRV
/** Output driver mode: totem pole */
#define PCA9685_PARAM_OUT_DRV       (PCA9685_TOTEM_POLE)
#endif

#ifndef PCA9685_PARAM_OUT_NE
/** Output driver mode: totem pole */
#define PCA9685_PARAM_OUT_NE        (PCA9685_OFF)
#endif

#ifndef PCA9685_PARAMS
#define PCA9685_PARAMS { \
                            .i2c_dev = PCA9685_PARAM_DEV, \
                            .i2c_addr = PCA9685_PARAM_ADDR, \
                            .inv = PCA9685_PARAM_INV, \
                            .mode = PCA9685_PARAM_MODE, \
                            .freq = PCA9685_PARAM_FREQ, \
                            .res = PCA9685_PARAM_RES, \
                            .ext_freq = PCA9685_PARAM_EXT_FREQ, \
                            .oe_pin = PCA9685_PARAM_OE_PIN, \
                            .out_drv = PCA9685_PARAM_OUT_DRV, \
                            .out_ne = PCA9685_PARAM_OUT_NE, \
                        }
#endif /* PCA9685_PARAMS */

#ifndef PCA9685_SAUL_PWM_PARAMS
/** Example for mapping PWM channels to SAUL */
#define PCA9685_SAUL_PWM_PARAMS { \
                                    .name = "PCA9685-0:0", \
                                    .dev = 0, \
                                    .channel = 0, \
                                    .initial = (PCA9685_PARAM_RES >> 1), \
                                }, \
                                { \
                                    .name = "PCA9685-0:1", \
                                    .dev = 0, \
                                    .channel = 1, \
                                    .initial = (PCA9685_PARAM_RES >> 2), \
                                }, \
                                { \
                                    .name = "PCA9685-0:1", \
                                    .dev = 0, \
                                    .channel = 2, \
                                    .initial = (PCA9685_PARAM_RES >> 3), \
                                },
#endif /* PCA9685_PARAMS */
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const pca9685_params_t pca9685_params[] =
{
    PCA9685_PARAMS
};

#if MODULE_SAUL || DOXYGEN
/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const  pca9685_saul_pwm_params_t pca9685_saul_pwm_params[] =
{
    PCA9685_SAUL_PWM_PARAMS
};
#endif /* MODULE_SAUL || DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
