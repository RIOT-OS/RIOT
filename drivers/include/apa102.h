/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_apa102 APA102 RGB LED
 * @ingroup     drivers_actuators
 * @brief       Driver for chained APA102 RGB LEDs
 * @{
 *
 * @file
 * @brief       Interface for controlling APA102 LEDs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "color.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Configuration parameters for (chained) APA102 LEDs
 */
typedef struct {
    int led_numof;          /**< number of chained LEDs */
    gpio_t data_pin;        /**< data pin */
    gpio_t clk_pin;         /**< clock pin */
} apa102_params_t;

/**
 * @brief   Device descriptor definition for APA102 LEDs
 */
typedef apa102_params_t apa102_t;

/**
 * @brief   Initialize (chained) APA102 LEDs
 *
 * @param[out] dev      device descriptor
 * @param[in]  params   device configuration
 *
 * @pre     @p dev != NULL
 * @pre     @p params != NULL
 */
void apa102_init(apa102_t *dev, const apa102_params_t *params);

/**
 * @brief   Apply the given color values to the connected LED(s)
 *
 * @param[in] dev       device descriptor
 * @param[in] vals      color values, MUST be of size `dev->led_numof`
 *
 * @pre     @p dev != NULL
 * @pre     @p vals != NULL
 */
void apa102_load_rgba(const apa102_t *dev, const color_rgba_t vals[]);

#ifdef __cplusplus
}
#endif

/** @} */
