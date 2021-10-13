/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       Provides an adaption of OpenWSN led handling
 *              to RIOTs handling of LEDs and/or GPIOs
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */
#ifndef OPENWSN_LEDS_H
#define OPENWSN_LEDS_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    OpenWSN leds pin configuration
 * @{
 */
typedef struct leds_config {
    gpio_t error;       /**< error led */
    gpio_t sync;        /**< synchronization state led */
    gpio_t radio;       /**< radio activity led */
    gpio_t debug;       /**< debug led */
    uint8_t led_on;     /**< GPIO set to turn led on */
} leds_config_t;
/** @} */

/**
 * @brief   Led on state values
 */
enum {
    GPIO_LED_LOW = 0,
    GPIO_LED_HIGH
};

/**
 * Sets the led pins for a specific board for OpenWSN
 *
 * @param[in] user_config A configuration of GPIO pins used for debugging.
 *
 * @note      Unused pins need to be defined as GPIO_UNDEF
 */
void ledpins_riot_init(const leds_config_t *user_config);

#ifdef __cplusplus
}
#endif

#endif /* OPENWSN_LEDS_H */
