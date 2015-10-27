/*
 * Copyright (C) 2015 Freie Universität Berlin
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

#include "periph_cpu.h"
#include "periph_conf.h"
/* TODO: remove once all platforms are ported to this interface */
#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default GPIO macro maps port-pin tuples to the pin value
 */
#ifndef GPIO_PIN
#define GPIO_PIN(x,y)       ((x & 0) | y)
#endif

/**
 * @brief   Define global value for GPIO not defined
 */
#ifndef GPIO_UNDEF
#define GPIO_UNDEF      (-1)
#endif

/**
 * @brief   Define the default GPIO type identifier
 */
#ifndef HAVE_GPIO_T
typedef int gpio_t;
#endif

/**
 * @brief   Definition of available pin directions
 */
#ifndef HAVE_GPIO_DIR_T
typedef enum {
    GPIO_DIR_IN = 0,        /**< configure pin as input */
    GPIO_DIR_OUT = 1,       /**< configure pin as output */
} gpio_dir_t;
#endif

/**
 * @brief   Definition of pull-up/pull-down modes
 */
#ifndef HAVE_GPIO_PP_T
typedef enum {
    GPIO_NOPULL = 0,        /**< do not use internal pull resistors */
    GPIO_PULLUP = 1,        /**< enable internal pull-up resistor */
    GPIO_PULLDOWN = 2       /**< enable internal pull-down resistor */
} gpio_pp_t;
#endif

/**
 * @brief   Definition of possible active flanks for external interrupt mode
 */
#ifndef HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 0,       /**< emit interrupt on falling flank */
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 2           /**< emit interrupt on both flanks */
} gpio_flank_t;
#endif

/**
 * @brief   Signature of event callback functions triggered from interrupts
 *
 * @param[in] arg       optional context for the callback
 */
typedef void (*gpio_cb_t)(void *arg);

/**
 * @brief   Initialize the given pin as general purpose input or output
 *
 * @param[in] pin       pin to initialize
 * @param[in] dir       direction for the pin, input or output
 * @param[in] pullup    define what pull resistors should be used
 *
 * @return              0 on success
 * @return              -1 on error
 */
int gpio_init(gpio_t pin, gpio_dir_t dir, gpio_pp_t pullup);

/**
 * @brief   Initialize a GPIO pin for external interrupt usage
 *
 * The registered callback function will be called in interrupt context every
 * time the defined flank(s) are detected.
 *
 * The interrupt is activated automatically after the initialization.
 *
 * @param[in] pin       pin to initialize
 * @param[in] pullup    define what pull resistors should be enabled
 * @param[in] flank     define the active flank(s)
 * @param[in] cb        callback that is called from interrupt context
 * @param[in] arg       optional argument passed to the callback
 *
 * @return              0 on success
 * @return              -1 on error
 */
int gpio_init_int(gpio_t pin, gpio_pp_t pullup, gpio_flank_t flank,
                    gpio_cb_t cb, void *arg);

/**
 * @brief   Enable pin's interrupt if configured as interrupt source
 *
 * @param[in] pin       the pin to enable the interrupt for
 */
void gpio_irq_enable(gpio_t pin);

/**
 * @brief   Disable the pin's interrupt if configured as interrupt source
 *
 * @param[in] pin       the pin to disable the interrupt for
 */
void gpio_irq_disable(gpio_t pin);

/**
 * @brief   Get the current value of the given pin
 *
 * @param[in] pin       the pin to read
 * @return              the current value, 0 for LOW, != 0 for HIGH
 *
 * @return              0 when pin is LOW
 * @return              greater 0 for HIGH
 */
int gpio_read(gpio_t pin);

/**
 * @brief   Set the given pin to HIGH
 *
 * @param[in] pin       the pin to set
 */
void gpio_set(gpio_t pin);

/**
 * @brief   Set the given pin to LOW
 *
 * @param[in] pin       the pin to clear
 */
void gpio_clear(gpio_t pin);

/**
 * @brief   Toggle the value of the given pin
 *
 * @param[in] pin       the pin to toggle
 */
void gpio_toggle(gpio_t pin);

/**
 * @brief   Set the given pin to the given value
 *
 * @param[in] pin       the pin to set
 * @param[in] value     value to set the pin to, 0 for LOW, HIGH otherwise
 */
void gpio_write(gpio_t pin, int value);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H */
/** @} */
