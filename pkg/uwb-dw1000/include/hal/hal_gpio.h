/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_dw1000
 * @{
 *
 * @file
 * @brief       GPIO abstraction layer RIOT adaption
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef HAL_HAL_GPIO_H
#define HAL_HAL_GPIO_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Map hal_gpio_pull_t enum types to gpio_mode_t enum types
 */
enum {
    /** Pull-up/down not enabled */
    HAL_GPIO_PULL_NONE = GPIO_IN,
    /** Pull-up enabled */
    HAL_GPIO_PULL_UP = GPIO_IN_PU,
    /** Pull-down enabled */
    HAL_GPIO_PULL_DOWN = GPIO_IN_PD
};
/**
 * @brief hal_gpio_pull type
 */
typedef gpio_mode_t hal_gpio_pull_t;

/**
 * @brief Map hal_gpio_irq_trig_t enum types to gpio_flank_t enum types
 */
enum {
#ifdef GPIO_NONE
    HAL_GPIO_TRIG_NONE = GPIO_NONE,
#endif
    /** IRQ occurs on rising edge */
    HAL_GPIO_TRIG_RISING = GPIO_RISING,
    /** IRQ occurs on falling edge */
    HAL_GPIO_TRIG_FALLING = GPIO_FALLING,
    /** IRQ occurs on either edge */
    HAL_GPIO_TRIG_BOTH = GPIO_BOTH,
    /** IRQ occurs when line is low */
#ifdef GPIO_LOW
    HAL_GPIO_TRIG_LOW = GPIO_LOW,
#endif
    /** IRQ occurs when line is high */
#ifdef GPIO_HIGH
    HAL_GPIO_TRIG_HIGH = GPIO_HIGH
#endif
};
/**
 * @brief hal_gpio_irq_trig type
 */
typedef gpio_flank_t hal_gpio_irq_trig_t;

/**
 * @brief Function proto for GPIO irq handler functions
 */
typedef gpio_cb_t hal_gpio_irq_handler_t;

/**
 * Initializes the specified pin as an input
 *
 * @param pin   Pin number to set as input
 * @param pull  pull type
 *
 * @return int  0: no error; -1 otherwise.
 */
static inline int hal_gpio_init_in(gpio_t pin, hal_gpio_pull_t pull)
{
    return gpio_init(pin, pull);
}

/**
 * Initialize the specified pin as an output, setting the pin to the specified
 * value.
 *
 * @param pin Pin number to set as output
 * @param val Value to set pin
 *
 * @return int  0: no error; -1 otherwise.
 */
static inline int hal_gpio_init_out(gpio_t pin, int val)
{
    int res = gpio_init(pin, GPIO_OUT);
    gpio_write(pin, val);
    return res;
}

/**
 * Write a value (either high or low) to the specified pin.
 *
 * @param pin Pin to set
 * @param val Value to set pin (0:low 1:high)
 */
static inline void hal_gpio_write(gpio_t pin, int val)
{
    gpio_write(pin, val);
}

/**
 * Reads the specified pin.
 *
 * @param pin Pin number to read
 *
 * @return int 0: low, 1: high
 */
static inline int hal_gpio_read(gpio_t pin)
{
    return gpio_read(pin);
}

/**
 * Toggles the specified pin
 *
 * @param pin Pin number to toggle
 *
 * @return current gpio state int 0: low, 1: high
 */
static inline int hal_gpio_toggle(gpio_t pin)
{
    gpio_toggle(pin);
    return gpio_read(pin);
}

/**
 * Initialize a given pin to trigger a GPIO IRQ callback.
 *
 * @param pin     The pin to trigger GPIO interrupt on
 * @param handler The handler function to call
 * @param arg     The argument to provide to the IRQ handler
 * @param trig    The trigger mode (e.g. rising, falling)
 * @param pull    The mode of the pin (e.g. pullup, pulldown)
 *
 * @return 0 on success, non-zero error code on failure.
 */
static inline int hal_gpio_irq_init(gpio_t pin,
                                    hal_gpio_irq_handler_t handler,
                                    void *arg,
                                    hal_gpio_irq_trig_t trig,
                                    hal_gpio_pull_t pull)
{
    return gpio_init_int(pin, pull, trig, handler, arg);
}

/**
 * Release a pin from being configured to trigger IRQ on state change.
 *
 * @param pin The pin to release
 */
static inline void hal_gpio_irq_release(gpio_t pin)
{
    /* can't release the interrupt so ar least disable it */
    gpio_irq_disable(pin);
}

/**
 * Enable IRQs on the passed pin
 *
 * @param pin The pin to enable IRQs on
 */
static inline void hal_gpio_irq_enable(gpio_t pin)
{
    gpio_irq_enable(pin);
}

/**
 * Disable IRQs on the passed pin
 *
 * @param pin The pin to disable IRQs on
 */
static inline void hal_gpio_irq_disable(gpio_t pin)
{
    gpio_irq_disable(pin);
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_HAL_GPIO_H */
