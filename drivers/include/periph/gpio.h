/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the LGPLv2 License.
 * See the file LICENSE in the top level directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @brief       Low-level GPIO periphial driver
 * @{
 *
 * @file        gpio.h
 * @brief       Low-level GPIO periphial driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __GPIO_H
#define __GPIO_H

#include "periph_conf.h"


/**
 * @brief Definition of available GPIO devices. Each device is managin exactly one pin.
 */
#ifdef GPIO_NUMOF
typedef enum {
#ifdef GPIO_0_EN
    GPIO_0 = 0,             ///< GPIO device 0
#endif
#ifdef GPIO_1_EN
    GPIO_1,                 ///< GPIO device 1
#endif
#ifdef GPIO_2_EN
    GPIO_2,                 ///< GPIO device 2
#endif
#ifdef GPIO_3_EN
    GPIO_3,                 ///< GPIO device 3
#endif
#ifdef GPIO_4_EN
    GPIO_4,                 ///< GPIO device 4
#endif
#ifdef GPIO_5_EN
    GPIO_5,                 ///< GPIO device 5
#endif
#ifdef GPIO_6_EN
    GPIO_6,                 ///< GPIO device 6
#endif
#ifdef GPIO_7_EN
    GPIO_7,                 ///< GPIO device 7
#endif
#ifdef GPIO_8_EN
    GPIO_8,                 ///< GPIO device 8
#endif
#ifdef GPIO_9_EN
    GPIO_9,                 ///< GPIO device 9
#endif
#ifdef GPIO_10_EN
    GPIO_10,                ///< GPIO device 10
#endif
#ifdef GPIO_11_EN
    GPIO_11,                ///< GPIO device 11
#endif
#ifdef GPIO_12_EN
    GPIO_12,                ///< GPIO device 12
#endif
#ifdef GPIO_13_EN
    GPIO_13,                ///< GPIO device 13
#endif
#ifdef GPIO_14_EN
    GPIO_14,                ///< GPIO device 14
#endif
#ifdef GPIO_15_EN
    GPIO_15                 ///< GPIO device 15
#endif
} gpio_t;
#endif

/**
 * @brief Definition of pull-up/pull-down modes
 */
typedef enum {
    GPIO_PULLDOWN,          ///< enable internal pull-down resistor
    GPIO_PULLUP,            ///< enable internal pull-up resistor
    GPIO_NOPULL             ///< do not use internal pull resistors
} gpio_pp_t;

/**
 * @brief Definition of possible active flanks for external interrupt mode
 */
typedef enum {
    GPIO_FALLING,           ///< emit interrupt on falling flank
    GPIO_RISING,            ///< emit interrupt on rising flank
    GPIO_BOTH               ///< emit interrupt on both flanks
} gpio_flank_t;

/**
 * @brief Initialize GPIO pin as output pin
 * 
 * @param dev       the GPIO pin to set as output
 * @param pullup    define if pull resistors should be used
 */
void gpio_init_out(gpio_t dev, gpio_pp_t pullup);

/**
 * @brief Initialize a GPIO pin in input mode
 * 
 * @param dev       the GPIO pin to use as input
 * @param pullup    define if pull resistors should be used
 */
void gpio_init_in(gpio_t dev, gpio_pp_t pullup);

/**
 * @brief Initialize a GPIO pin for external interrupt usage
 * 
 * The registerd callback function will be called in interrupt context everytime the defined flank(s)
 * are detected.
 * 
 * The interrupt is activated automatically after the initialization.
 * 
 * @param dev       the GPIO pin to initialize
 * @param pullup    define if pull resistors should be enabled
 * @param flank     define the active flank(s)
 * @param cb        register the callback that is called in interrupt context
 */
void gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, void (*cb)(void));

/**
 * @brief Get the current value of the given GPIO pin
 * 
 * @param dev       the GPIO pin to read
 * @return          the current value, 0 for LOW, != 0 for HIGH
 */
int gpio_read(gpio_t dev);

/**
 * @brief Set the given GPIO pin to HIGH
 * 
 * @param dev       the GPIO pin to set
 */
void gpio_set(gpio_t dev);

/**
 * @brief Set the given GPIO pin to LOW
 * 
 * @param dev       the GPIO pin to clear
 */
void gpio_clear(gpio_t dev);

/**
 * @brief Toggle the value of the given GPIO pin
 * 
 * @param dev       the GPIO pin to toggle
 */
void gpio_toggle(gpio_t dev);

/**
 * @brief Set the given GPIO pin to the given value
 * 
 * @param dev       the GPIO pin to set
 * @param value     value to set the pin to, 0 for LOW, HIGH otherwise
 */
void gpio_write(gpio_t dev, int value);

#endif /* __GPIO_H */
/** @} */
