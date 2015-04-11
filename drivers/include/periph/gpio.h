/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_periph_gpio GPIO
 * @ingroup     driver_periph
 * @brief       Low-level GPIO peripheral driver
 *
 * @{
 * @file
 * @brief       Low-level GPIO peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef GPIO_H
#define GPIO_H

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* guard file in case no GPIO devices are defined */
#if GPIO_NUMOF

/**
 * @brief Definition of available GPIO devices. Each device is managing exactly one pin.
 */
typedef enum {
#if GPIO_0_EN
    GPIO_0 = 0,             /**< GPIO device 0 */
#endif
#if GPIO_1_EN
    GPIO_1,                 /**< GPIO device 1 */
#endif
#if GPIO_2_EN
    GPIO_2,                 /**< GPIO device 2 */
#endif
#if GPIO_3_EN
    GPIO_3,                 /**< GPIO device 3 */
#endif
#if GPIO_4_EN
    GPIO_4,                 /**< GPIO device 4 */
#endif
#if GPIO_5_EN
    GPIO_5,                 /**< GPIO device 5 */
#endif
#if GPIO_6_EN
    GPIO_6,                 /**< GPIO device 6 */
#endif
#if GPIO_7_EN
    GPIO_7,                 /**< GPIO device 7 */
#endif
#if GPIO_8_EN
    GPIO_8,                 /**< GPIO device 8 */
#endif
#if GPIO_9_EN
    GPIO_9,                 /**< GPIO device 9 */
#endif
#if GPIO_10_EN
    GPIO_10,                /**< GPIO device 10 */
#endif
#if GPIO_11_EN
    GPIO_11,                /**< GPIO device 11 */
#endif
#if GPIO_12_EN
    GPIO_12,                /**< GPIO device 12 */
#endif
#if GPIO_13_EN
    GPIO_13,                /**< GPIO device 13 */
#endif
#if GPIO_14_EN
    GPIO_14,                /**< GPIO device 14 */
#endif
#if GPIO_15_EN
    GPIO_15,                /**< GPIO device 15 */
#endif
#if GPIO_16_EN
    GPIO_16,                /**< GPIO device 16 */
#endif
#if GPIO_17_EN
    GPIO_17,                /**< GPIO device 17 */
#endif
#if GPIO_18_EN
    GPIO_18,                /**< GPIO device 18 */
#endif
#if GPIO_19_EN
    GPIO_19,                /**< GPIO device 19 */
#endif
#if GPIO_20_EN
    GPIO_20,                /**< GPIO device 20 */
#endif
#if GPIO_21_EN
    GPIO_21,                /**< GPIO device 21 */
#endif
#if GPIO_22_EN
    GPIO_22,                /**< GPIO device 22 */
#endif
#if GPIO_23_EN
    GPIO_23,                /**< GPIO device 23 */
#endif
#if GPIO_24_EN
    GPIO_24,                /**< GPIO device 24 */
#endif
#if GPIO_25_EN
    GPIO_25,                /**< GPIO device 25 */
#endif
#if GPIO_26_EN
    GPIO_26,                /**< GPIO device 26 */
#endif
#if GPIO_27_EN
    GPIO_27,                /**< GPIO device 27 */
#endif
#if GPIO_28_EN
    GPIO_28,                /**< GPIO device 28 */
#endif
#if GPIO_29_EN
    GPIO_29,                /**< GPIO device 29 */
#endif
#if GPIO_30_EN
    GPIO_30,                /**< GPIO device 30 */
#endif
#if GPIO_31_EN
    GPIO_31,                /**< GPIO device 31 */
#endif
} gpio_t;

/**
 * @brief Definition of pull-up/pull-down modes
 */
typedef enum {
    GPIO_NOPULL = 0,        /**< do not use internal pull resistors */
    GPIO_PULLUP = 1,        /**< enable internal pull-up resistor */
    GPIO_PULLDOWN = 2       /**< enable internal pull-down resistor */
} gpio_pp_t;

/**
 * @brief Definition of possible active flanks for external interrupt mode
 */
typedef enum {
    GPIO_FALLING = 0,       /**< emit interrupt on falling flank */
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 2           /**< emit interrupt on both flanks */
} gpio_flank_t;

/**
 * @brief Signature for function called from interrupt context after an external event
 *
 * @param[in] arg       optional context for the callback
 */
typedef void (*gpio_cb_t)(void *arg);

/**
 * @brief Initialize GPIO pin as output pin
 *
 * @param[in] dev       the GPIO pin to set as output
 * @param[in] pullup    define if pull resistors should be used
 *
 * @return              0 on success
 * @return              -1 on pull-up mode unavailable
 */
int gpio_init_out(gpio_t dev, gpio_pp_t pullup);

/**
 * @brief Initialize a GPIO pin in input mode
 *
 * @param[in] dev       the GPIO pin to use as input
 * @param[in] pullup    define if pull resistors should be used
 *
 * @return              0 on success
 * @return              -1 on pull-up mode unavailable
 */
int gpio_init_in(gpio_t dev, gpio_pp_t pullup);

/**
 * @brief Initialize a GPIO pin for external interrupt usage
 *
 * The registered callback function will be called in interrupt context every time the defined flank(s)
 * are detected.
 *
 * The interrupt is activated automatically after the initialization.
 *
 * @param[in] dev       the GPIO pin to initialize
 * @param[in] pullup    define if pull resistors should be enabled
 * @param[in] flank     define the active flank(s)
 * @param[in] cb        register the callback that is called in interrupt context
 * @param[in] arg       optional argument passed to the callback
 *
 * @return              0 on success
 * @return              -1 on pull-up mode unavailable
 * @return              -2 on active flank unavailable
 */
int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg);

/**
 * @brief Enable the GPIO pin's interrupt if configured as external interrupt source.
 *
 * @param[in] dev       the GPIO pin to enable the interrupt for
 */
void gpio_irq_enable(gpio_t dev);

/**
 * @brief Disable the GPIO pin's interrupt if configured as external interrupt source.
 *
 * @param[in] dev       the GPIO pin to disable the interrupt for
 */
void gpio_irq_disable(gpio_t dev);

/**
 * @brief Get the current value of the given GPIO pin
 *
 * @param[in] dev       the GPIO pin to read
 * @return              the current value, 0 for LOW, != 0 for HIGH
 *
 * @return              0 when pin is LOW
 * @return              greater 0 for HIGH
 */
int gpio_read(gpio_t dev);

/**
 * @brief Set the given GPIO pin to HIGH
 *
 * @param[in] dev       the GPIO pin to set
 */
void gpio_set(gpio_t dev);

/**
 * @brief Set the given GPIO pin to LOW
 *
 * @param[in] dev       the GPIO pin to clear
 */
void gpio_clear(gpio_t dev);

/**
 * @brief Toggle the value of the given GPIO pin
 *
 * @param[in] dev       the GPIO pin to toggle
 */
void gpio_toggle(gpio_t dev);

/**
 * @brief Set the given GPIO pin to the given value
 *
 * @param[in] dev       the GPIO pin to set
 * @param[in] value     value to set the pin to, 0 for LOW, HIGH otherwise
 */
void gpio_write(gpio_t dev, int value);

#endif /* GPIO_NUMOF */

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H */
/** @} */
