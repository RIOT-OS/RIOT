/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_onboard_led
 * @ingroup     drivers
 * @brief       Abstraction for handling on-board LEDs
 * @{
 *
 * @file
 * @brief       On-board LED handling abstraction
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef ONBOARD_LED_H
#define ONBOARD_LED_H

#include "board.h"
#include "assert.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED_NUMOF       (sizeof(led_config) / sizeof(led_config[0]))

#ifndef ONBOARD_LED_ACTIVE_LOW
#define TURN_ON         gpio_set
#define TURN_OFF        gpio_clear
#else
#define TURN_ON         gpio_clear
#define TURN_OFF        gpio_set
#endif

/**
 * @brief   Initialize all available on-board LEDs
 */
void onboard_led_init(void);

/**
 * @brief   Turn on the given on-board LED
 *
 * @param[in] num       LED to turn on
 */
static inline void onboard_led_on(unsigned num)
{
    assert(num < LED_NUMOF);
    TURN_ON(led_config[num]);
}

/**
 * @brief   Turn off the given on-board LED
 *
 * @param[in] num       LED to turn off
 */
static inline void onboard_led_off(unsigned num)
{
    assert(num < LED_NUMOF);
    TURN_OFF(led_config[num]);
}

/**
 * @brief   Toggle the given on-board LED
 *
 * @param[in] num       LED to toggle
 */
static inline void onboard_led_toggle(unsigned num)
{
    assert(num < LED_NUMOF);
    gpio_toggle(led_config[num]);
}

#ifdef __cplusplus
}
#endif

#endif /* ONBOARD_LED_H */
/** @} */
