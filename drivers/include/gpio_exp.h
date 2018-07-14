/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_gpio_exp GPIO expansion driver
 * @ingroup     drivers
 * @brief       Global GPIO driver for pin-mapped GPIO expanders
 *
 * The GPIO expansion interface makes handling of GPIO expanders invisible to
 * code using the GPIO pin API (periph/gpio.h). This is accomplished by
 * reserving part of the range of values of gpio_t. When a call to the GPIO API
 * uses a pin that falls within this range, it is parsed into a device ID that
 * is looked up in the list of GPIO expanders and the call is redirected to
 * the corresponding device.
 *
 * @{
 *
 * @file
 * @brief       GPIO expansion interface definition
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef GPIO_EXP_H
#define GPIO_EXP_H

#include <stdint.h>

#include "periph_cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GPIO_EXP_DEV_LOC
/**
 * @brief   Set device ID location in gpio_t
 */
#define GPIO_EXP_DEV_LOC    (8*sizeof(gpio_t) - 8)
#endif

#ifndef GPIO_EXP_MAXDEVMASK
/**
 * @brief   Maximum device number mask for gpio_exp
 */
#define GPIO_EXP_MAXDEVMASK    (0x7f)
#endif

#ifndef GPIO_EXP_MAXPINMASK
/**
 * @brief   Maximum pin number mask for gpio_exp
 */
#define GPIO_EXP_MAXPINMASK    (0xff)
#endif

#ifndef GPIO_EXP_THRESH
/**
 * @brief   Pin numbers greater than GPIO_EXP_THRESH are on GPIO expanders
 */
#define GPIO_EXP_THRESH    ((gpio_t)(-1) >> 1)
#endif

#ifndef GPIO_EXP_PIN
/**
 * @brief   Convert (device, pin) tuple to @c gpio_t value
 *
 * @note    Device number is determined by its position in gpio_exp_list,
 *          defined in gpio_exp_params.h
 */
#define GPIO_EXP_PIN(x, y) \
    (gpio_t)(~GPIO_EXP_THRESH | (x << GPIO_EXP_DEV_LOC) | GPIO_PIN(0,y))
#endif

/**
 * @brief   Callback typedef for gpio_init
 *
 * @see @ref drivers_periph_gpio
 */
typedef int (*gpio_exp_init_t)(void *dev, gpio_t pin, gpio_mode_t mode);

/**
 * @brief   Callback typedef for gpio_init_int
 *
 * @see @ref drivers_periph_gpio
 */
typedef int (*gpio_exp_init_int_t)(void *dev, gpio_t pin, gpio_mode_t mode,
                                   gpio_flank_t flank, gpio_cb_t cb, void *arg);

/**
 * @brief   Callback typedef for gpio_irq (merged enable and disable)
 *
 * @see @ref drivers_periph_gpio
 */
typedef void (*gpio_exp_irq_t)(void *dev, gpio_t pin, int state);

/**
 * @brief   Callback typedef for gpio_read
 *
 * @see @ref drivers_periph_gpio
 */
typedef int (*gpio_exp_read_t)(void *dev, gpio_t pin);

/**
 * @brief   Callback typedef for gpio_write
 *
 * @see @ref drivers_periph_gpio
 */
typedef void (*gpio_exp_write_t)(void *dev, gpio_t pin, int value);

/**
 * @brief   Default not supported functions
 * @{
 */
int gpio_exp_init_notsup(void *dev, gpio_t pin, gpio_mode_t mode);
int gpio_exp_init_int_notsup(void *dev, gpio_t pin, gpio_mode_t mode,
                             gpio_flank_t flank, gpio_cb_t cb, void *arg);
void gpio_exp_irq_notsup(void *dev, gpio_t pin, int state);
int gpio_exp_read_notsup(void *dev, gpio_t pin);
void gpio_exp_write_notsup(void *dev, gpio_t pin, int value);
/** @} */

/**
 * @brief   GPIO expansion driver entry
 */
typedef struct gpio_exp_driver {
    gpio_exp_init_t const init;            /**< callback for gpio_init */
    gpio_exp_init_int_t const init_int;    /**< callback for gpio_init_int */
    gpio_exp_irq_t const irq;              /**< callback for gpio_irq */
    gpio_exp_read_t const read;            /**< callback for read */
    gpio_exp_write_t const write;          /**< callback for write */
} gpio_exp_driver_t;

/**
 * @brief   GPIO expansion list entry
 */
typedef struct gpio_exp {
    gpio_exp_driver_t const *driver;    /**< pointer to driver */
    void *dev;                          /**< pointer to device descriptor */
} gpio_exp_t;

/**
 * @brief   Find an entry in the list of expanders by GPIO pin number
 *
 * @param[in] gpio    GPIO to look up
 *
 * @return      pointer to the list entry
 * @return      NULL if no device is listed
 */
gpio_exp_t *gpio_exp_entry(gpio_t gpio);

/**
 * @brief   Strip encoding from a GPIO and return pin number
 *
 * @param[in] gpio    GPIO to strip
 *
 * @return      pin number
 */
gpio_t gpio_exp_pin(gpio_t gpio);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_EXP_H */
/** @} */
