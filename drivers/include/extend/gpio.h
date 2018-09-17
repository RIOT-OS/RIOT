/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_extend_gpio GPIO extension
 * @ingroup     drivers_extend
 * @brief       GPIO peripheral extension handling
 *
 * The GPIO extension interface makes handling of non-CPU GPIO devices invisible
 * to code using the GPIO periph API (periph/gpio.h). This is accomplished by
 * reserving part of the range of values of gpio_t. When a call to the GPIO API
 * uses a pin that falls within this range, it is parsed into a device ID that
 * is looked up in the GPIO extension list and the call is redirected to the
 * corresponding device.
 *
 * @{
 *
 * @file
 * @brief       GPIO extension interface definitions
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef EXTEND_GPIO_H
#define EXTEND_GPIO_H

#include <limits.h>
#include <stdint.h>

#include "periph_conf.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GPIO_EXT_DEV_MASK
/**
 * @brief   Set device mask in gpio_t
 */
#define GPIO_EXT_DEV_MASK    (gpio_t)((UINT_MAX << GPIO_EXT_DEV_LOC) \
                                      & GPIO_EXT_THRESH)
#endif

#ifndef GPIO_EXT_PIN_MASK
/**
 * @brief   Set pin mask in gpio_t
 */
#define GPIO_EXT_PIN_MASK    (gpio_t)(~(UINT_MAX << GPIO_EXT_DEV_LOC))
#endif

/**
 * @brief   Callback typedef for gpio_init
 *
 * @see @ref drivers_periph_gpio
 */
typedef int (*gpio_ext_init_t)(void *dev, gpio_t pin, gpio_mode_t mode);

/**
 * @brief   Callback typedef for gpio_init_int
 *
 * @see @ref drivers_periph_gpio
 */
typedef int (*gpio_ext_init_int_t)(void *dev, gpio_t pin, gpio_mode_t mode,
                                   gpio_flank_t flank, gpio_cb_t cb, void *arg);

/**
 * @brief   Callback typedef for gpio_irq_enable
 *
 * @see @ref drivers_periph_gpio
 */
typedef void (*gpio_ext_irq_enable_t)(void *dev, gpio_t pin);

/**
 * @brief   Callback typedef for gpio_irq_disable
 *
 * @see @ref drivers_periph_gpio
 */
typedef void (*gpio_ext_irq_disable_t)(void *dev, gpio_t pin);

/**
 * @brief   Callback typedef for gpio_read
 *
 * @see @ref drivers_periph_gpio
 */
typedef int (*gpio_ext_read_t)(void *dev, gpio_t pin);

/**
 * @brief   Callback typedef for gpio_set
 *
 * @see @ref drivers_periph_gpio
 */
typedef void (*gpio_ext_set_t)(void *dev, gpio_t pin);

/**
 * @brief   Callback typedef for gpio_clear
 *
 * @see @ref drivers_periph_gpio
 */
typedef void (*gpio_ext_clear_t)(void *dev, gpio_t pin);

/**
 * @brief   Callback typedef for gpio_toggle
 *
 * @see @ref drivers_periph_gpio
 */
typedef void (*gpio_ext_toggle_t)(void *dev, gpio_t pin);

/**
 * @brief   Callback typedef for gpio_write
 *
 * @see @ref drivers_periph_gpio
 */
typedef void (*gpio_ext_write_t)(void *dev, gpio_t pin, int value);

/**
 * @brief   Default not supported functions
 * @{
 */
int gpio_ext_init_notsup(void *dev, gpio_t pin, gpio_mode_t mode);
int gpio_ext_init_int_notsup(void *dev, gpio_t pin, gpio_mode_t mode,
                             gpio_flank_t flank, gpio_cb_t cb, void *arg);
void gpio_ext_irq_enable_notsup(void *dev, gpio_t pin);
void gpio_ext_irq_disable_notsup(void *dev, gpio_t pin);
int gpio_ext_read_notsup(void *dev, gpio_t pin);
void gpio_ext_set_notsup(void *dev, gpio_t pin);
void gpio_ext_clear_notsup(void *dev, gpio_t pin);
void gpio_ext_toggle_notsup(void *dev, gpio_t pin);
void gpio_ext_write_notsup(void *dev, gpio_t pin, int value);
/** @} */

/**
 * @brief   GPIO extension driver entry
 */
typedef struct gpio_ext_driver {
    gpio_ext_init_t const init;           /**< callback for gpio_init */
    gpio_ext_init_int_t const init_int;   /**< callback for gpio_init_int */
    gpio_ext_irq_enable_t const irq_enable;
                                          /**< callback for gpio_irq_enable */
    gpio_ext_irq_disable_t const irq_disable;
                                          /**< callback for gpio_irq_disable */
    gpio_ext_read_t const read;           /**< callback for read */
    gpio_ext_set_t const set;             /**< callback for set */
    gpio_ext_clear_t const clear;         /**< callback for clear */
    gpio_ext_toggle_t const toggle;       /**< callback for toggle */
    gpio_ext_write_t const write;         /**< callback for write */
} gpio_ext_driver_t;

/**
 * @brief   GPIO extension list entry
 */
typedef struct gpio_ext {
    gpio_ext_driver_t const *driver;    /**< pointer to driver */
    void *dev;                          /**< pointer to device descriptor */
} gpio_ext_t;

/**
 * @brief   Find an entry in the extension list by GPIO pin number
 *
 * @param[in] gpio    GPIO to look up
 *
 * @return      pointer to the list entry
 * @return      NULL if no device is listed
 */
gpio_ext_t *gpio_ext_entry(gpio_t gpio);

/**
 * @brief   Strip encoding from a GPIO and return device number
 *
 * @param[in] gpio    GPIO to strip
 *
 * @return      device number
 */
static inline gpio_t gpio_ext_dev(gpio_t gpio)
{
    return ((gpio & GPIO_EXT_DEV_MASK) >> GPIO_EXT_DEV_LOC);
}

/**
 * @brief   Strip encoding from a GPIO and return pin number
 *
 * @param[in] gpio    GPIO to strip
 *
 * @return      pin number
 */
static inline gpio_t gpio_ext_pin(gpio_t gpio)
{
    return (gpio & GPIO_EXT_PIN_MASK);
}

#ifdef __cplusplus
}
#endif

#endif /* EXTEND_GPIO_H */
/** @} */
