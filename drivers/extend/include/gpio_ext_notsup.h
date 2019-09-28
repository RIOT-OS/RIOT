/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *               2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_extend_gpio
 * @brief       GPIO extension not-supported functions
 *
 * The functions defined here can be used as default functions for GPIO
 * extension devices that do not support all required functions of the GPIO
 * extension interface. In addition, these functions can be used for testing.
 * They just print a debug message, if `ENABLE_DEBUG` is set, and return with
 * an error if a return value is defined.
 *
 * @{
 *
 * @file
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef GPIO_EXT_NOTSUP_H
#define GPIO_EXT_NOTSUP_H

#include "extend/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Not-supported function for @ref gpio_init
 */
int gpio_ext_init_notsup(void *dev, gpio_t pin, gpio_mode_t mode);

#if MODULE_PERIPH_GPIO_IRQ || DOXYGEN

/**
 * @brief   Not-supported function for @ref gpio_init_int
 */
int gpio_ext_init_int_notsup(void *dev, gpio_t pin, gpio_mode_t mode,
                             gpio_flank_t flank, gpio_cb_t cb, void *arg);

/**
 * @brief   Not-supported function for @ref gpio_irq_enable
 */
void gpio_ext_irq_enable_notsup(void *dev, gpio_t pin);

/**
 * @brief   Not-supported function for @ref gpio_irq_disable
 */
void gpio_ext_irq_disable_notsup(void *dev, gpio_t pin);

#endif /* MODULE_PERIPH_GPIO_IRQ || DOXYGEN */

/**
 * @brief   Not-supported function for @ref gpio_read
 */
int  gpio_ext_read_notsup(void *dev, gpio_t pin);

/**
 * @brief   Not-supported function for @ref gpio_set
 */
void gpio_ext_set_notsup(void *dev, gpio_t pin);

/**
 * @brief   Not-supported function for @ref gpio_clear
 */
void gpio_ext_clear_notsup(void *dev, gpio_t pin);

/**
 * @brief   Not-supported function for @ref gpio_toggle
 */
void gpio_ext_toggle_notsup(void *dev, gpio_t pin);

/**
 * @brief   Not-supported function for @ref gpio_write
 */
void gpio_ext_write_notsup(void *dev, gpio_t pin, int value);

/**
 * @brief   Driver with not-supported functions
 *
 * Defines a driver with all not-supported GPIO extension functinos that
 * can be used for testing.
 */
extern const gpio_ext_driver_t gpio_ext_notsup_driver;

#ifdef __cplusplus
}
#endif

#endif /* GPIO_EXT_NOTSUP_H */
/** @} */
