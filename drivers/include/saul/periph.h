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
 * @brief   Direct mapped GPIO configuration values
 */
typedef struct {
    const char *name;       /**< name of the device connected to this pin */
    gpio_t pin;             /**< GPIO pin to initialize and expose */
    gpio_mode_t mode;       /**< pin mode to use */
} saul_gpio_params_t;
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
