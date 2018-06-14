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

#include "saul_reg.h"

#if MODULE_SAUL_GPIO || DOXYGEN
#include "periph/gpio.h"
#endif /* MODULE_SAUL_GPIO */

#if MODULE_SAUL_ADC || DOXYGEN
#include "periph/adc.h"
#endif /* MODULE_SAUL_ADC */

#ifdef __cplusplus
extern "C" {
#endif

#if MODULE_SAUL_GPIO || DOXYGEN
/**
 * @brief SAUL GPIO configuration flags
 */
typedef enum {
    SAUL_GPIO_INVERTED   = (1 << 0),  /**< pin is used as inverted */
    SAUL_GPIO_INIT_CLEAR = (1 << 1),  /**< set pin inactive after init */
    SAUL_GPIO_INIT_SET   = (1 << 2),  /**< set pin active after init */
} saul_gpio_flags_t;

/**
 * @brief   Direct mapped GPIO configuration values
 */
typedef struct {
    const char *name;            /**< name of the device(s) on this pin */
    gpio_t pin;                  /**< GPIO pin offset for ctxtlist */
    saul_ctxt_list_t pinlist;    /**< bit list of pins to map */
    gpio_mode_t mode;            /**< pin mode to use */
    saul_gpio_flags_t flags;     /**< Configuration flags */
} saul_gpio_params_t;
#endif /* MODULE_SAUL_GPIO */

#if MODULE_SAUL_ADC || DOXYGEN
/**
 * @brief   Direct mapped ADC configuration values
 */
typedef struct {
    const char *name;             /**< name of the device(s) on this pin */
    adc_t line;                   /**< ADC line offset for ctxtlist */
    saul_ctxt_list_t linelist;    /**< bit list of lines to expose */
    adc_res_t res;                /**< ADC resolution */
} saul_adc_params_t;
#endif /* MODULE_SAUL_ADC */

#ifdef __cplusplus
}
#endif

#endif /* SAUL_PERIPH_H */
/** @} */
