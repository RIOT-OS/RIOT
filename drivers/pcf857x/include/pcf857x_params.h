/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pcf857x
 * @brief       Default configuration for Texas Instruments PCF857X I2C I/O expanders
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef PCF857X_PARAMS_H
#define PCF857X_PARAMS_H

#include "board.h"
#include "pcf857x.h"
#include "saul_reg.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef PCF857X_PARAM_DEV
/** device is I2C_DEV(0) */
#define PCF857X_PARAM_DEV       I2C_DEV(0)
#endif

#ifndef PCF857X_PARAM_ADDR
/** I2C slave address offset is 0 */
#define PCF857X_PARAM_ADDR      (0)
#endif

#ifndef PCF857X_PARAM_EXP
/** PCF857X expander variant used depends on enabled pseudomodules*/
#if IS_USED(MODULE_PCF8575) || DOXYGEN
#define PCF857X_PARAM_EXP       (PCF857X_EXP_PCF8575)
#elif IS_USED(MODULE_PCF8574)
#define PCF857X_PARAM_EXP       (PCF857X_EXP_PCF8574)
#elif IS_USED(MODULE_PCF8574A)
#define PCF857X_PARAM_EXP       (PCF857X_EXP_PCF8574A)
#endif
#endif /* PCF857X_PARAM_EXP */

#ifndef PCF857X_PARAM_INT_PIN
/** MCU interrupt pin */
#define PCF857X_PARAM_INT_PIN   (GPIO_UNDEF)
#endif

#ifndef PCF857X_PARAMS
#if IS_USED(MODULE_PCF857X_IRQ) || DOXYGEN
/** Default configuration parameter set */
#define PCF857X_PARAMS { \
                            .dev = PCF857X_PARAM_DEV, \
                            .addr = PCF857X_PARAM_ADDR, \
                            .exp = PCF857X_PARAM_EXP, \
                            .int_pin = PCF857X_PARAM_INT_PIN, \
                        },
#else
#define PCF857X_PARAMS { \
                            .dev = PCF857X_PARAM_DEV, \
                            .addr = PCF857X_PARAM_ADDR, \
                            .exp = PCF857X_PARAM_EXP, \
                        },
#endif
#endif /* PCF857X_PARAMS */

#ifndef PCF857X_SAUL_GPIO_PARAMS
/** Example for mapping expander pins to SAUL */
#define PCF857X_SAUL_GPIO_PARAMS { \
                                        .dev = 0, \
                                        .gpio = { \
                                            .name = "P00 Output", \
                                            .pin = PCF857X_GPIO_PIN(0, 0), \
                                            .mode = GPIO_OUT, \
                                            .flags = SAUL_GPIO_INIT_CLEAR, \
                                        } \
                                 }, \
                                 { \
                                        .dev = 0, \
                                        .gpio = { \
                                            .name = "P01 Input", \
                                            .pin = PCF857X_GPIO_PIN(0, 1), \
                                            .mode = GPIO_IN, \
                                            .flags = 0, \
                                        } \
                                 },
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const pcf857x_params_t pcf857x_params[] =
{
    PCF857X_PARAMS
};

#if IS_USED(MODULE_SAUL_GPIO) || DOXYGEN
/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const  pcf857x_saul_gpio_params_t pcf857x_saul_gpio_params[] =
{
    PCF857X_SAUL_GPIO_PARAMS
};
#endif /* MODULE_SAUL_GPIO || DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PCF857X_PARAMS_H */
