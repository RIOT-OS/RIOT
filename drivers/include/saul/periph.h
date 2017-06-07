/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_saul
 * @{
 *
 * @file
 * @brief       Parameter definitions for mapping peripherals directly to SAUL
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SAUL_PERIPH_H
#define SAUL_PERIPH_H

#ifdef MODULE_SAUL_GPIO
#include "periph/gpio.h"
#endif /* MODULE_SAUL_GPIO */

#ifdef MODULE_SAUL_ADC
#include "periph/adc.h"
#endif /* MODULE_SAUL_ADC */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_SAUL_GPIO
/**
 * @brief   Available SAUL GPIO modes
 */
typedef enum {
    SAUL_GPIO_IN,           /**< pin is used as input */
    SAUL_GPIO_IN_INV,       /**< pin is used as inverted input */
    SAUL_GPIO_OUT,          /**< pin is used as output */
    SAUL_GPIO_OUT_INV,      /**< pin is used as inverted output */
} saul_gpio_type_t;

/**
 * @brief   Initial pin state after initialization
 */
typedef enum {
    SAUL_GPIO_CLEAR = 0,    /**< set pin inactive after initialization */
    SAUL_GPIO_SET   = 1,    /**< set pin active after initialization */
    SAUL_GPIO_SKIP  = 2     /**< do not touch the pin state */
} saul_gpio_state_t;

/**
 * @brief   Direct mapped GPIO configuration values
 */
typedef struct {
    const char *name;       /**< name of the device connected to this pin */
    gpio_t pin;             /**< GPIO pin to initialize and expose */
    gpio_mode_t mode;       /**< GPIO mode to use */
    saul_gpio_type_t type;  /**< usage type for the pin */
    saul_gpio_state_t state;/**< initial pin state */
} saul_gpio_params_t;

/**
 * @brief   SAUL GPIO input driver
 */
extern const saul_driver_t gpio_in_saul_driver;

/**
 * @brief   SAUL GPIO inverted input driver
 */
extern const saul_driver_t gpio_in_inv_saul_driver;

/**
 * @brief   SAUL GPIO output driver
 */
extern const saul_driver_t gpio_out_saul_driver;

/**
 * @brief   SAUL GPIO inverted output driver
 */
extern const saul_driver_t gpio_out_inv_saul_driver;
#endif /* MODULE_SAUL_GPIO */

#ifdef MODULE_SAUL_ADC
/**
 * @brief   Direct mapped ADC configuration values
 */
typedef struct {
    const char *name;       /**< name of the device connected to this pin */
    adc_t line;             /**< ADC line to initialize and expose */
    adc_res_t res;          /**< ADC resolution */
} saul_adc_params_t;
#endif /* MODULE_SAUL_ADC */

#ifdef __cplusplus
}
#endif

#endif /* SAUL_PERIPH_H */
/** @} */
