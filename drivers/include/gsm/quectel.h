/*
 * Copyright (C) 2018 Max van Kessel
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_gsm_quectel Quectel
 * @ingroup     drivers_gsm
 * @brief       A generic implementation for the Quectel gsm modules
 *
 * @{
 *
 * @file
 * @brief   Quectel GSM driver
 *
 * @author  Max van Kessel
 */

#ifndef GSM_QUECTEL_H
#define GSM_QUECTEL_H

#include <stdint.h>
#include <stdbool.h>

#include "periph/gpio.h"
#include "gsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Specifies the default on time the power key needs for action
 */
#ifndef QUECTEL_POWER_KEY_ON_TIME_US
#define QUECTEL_POWER_KEY_ON_TIME_US            (1000 * US_PER_MS)
#endif

/**
 * @brief Specifies the default off time the power key needs for action
 */
#ifndef QUECTEL_POWER_KEY_OFF_TIME_US
#define QUECTEL_POWER_KEY_OFF_TIME_US           (1000 * US_PER_MS)
#endif

/**
 * @brief Specifies the default time of action on the status line
 */
#ifndef QUECTEL_STATUS_DETECT_TIMEOUT_US
#define QUECTEL_STATUS_DETECT_TIMEOUT_US        (5000 * US_PER_MS)
#endif

/**
 * @brief Specifies the default time on the reset pin
 */
#ifndef QUECTEL_RESET_TIME_US
#define QUECTEL_RESET_TIME_US                   (500 * US_PER_MS)
#endif

/**
 * @brief Quectel gsm device parameters
 */
typedef struct gsm_quectel_params {
    gsm_params_t base;              /**< gsm base parameters */
    gpio_t      power_pin;          /**< quectel power pin*/
    bool        invert_power_pin;   /**< select inversion of power pin */
    gpio_t      status_pin;         /**< quectel status pin (modem output) */
    bool        invert_status_pin;  /**< select inversion of status pin */
    gpio_t      reset_pin;          /**< quectel reset pin*/
    bool        invert_reset_pin;   /**< select inversion of reset pin*/
    gpio_t      dtr_pin;            /**< quectel dtr pin (modem intput) */
    gpio_t      dcd_pin;            /**< quectel dcd pin (modem output) */
} quectel_params_t;

/**
 * @brief Quectel gsm device context
 */
typedef struct quectel {
    gsm_t base;                     /**< gsm base driver */
} quectel_t;

/**
 * @brief exported gsm api for the Quectel device
 */
extern const gsm_driver_t quectel_driver;

#ifdef __cplusplus
}
#endif

#endif /* GSM_QUECTEL_H */
/** @} */
