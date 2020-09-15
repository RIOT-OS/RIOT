/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests_periph_gpio_exp
 * @{
 *
 * @file
 * @brief       Example driver for a single port GPIO expander
 *
 * This example shows how to implement a driver for a GPIO expander with a
 * single port. The low-level GPIO API can be implemented and used directly
 * in this case.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef FOO_GPIO_EXP_H
#define FOO_GPIO_EXP_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device-specific descriptor as used by the driver.
 */
typedef struct {
    const char *name;   /**< name of the GPIO expander */
    gpio_mask_t state;  /**< the port of the GPIO expander */
} foo_gpio_exp_t;

/**
 * @name    Device-specific implementation of the low-Level GPIO API.
 * @{
 */
int foo_gpio_exp_init(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode);
#ifdef MODULE_PERIPH_GPIO_IRQ
int foo_gpio_exp_init_int(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode,
                     gpio_flank_t flank, gpio_cb_t cb, void *arg);
void foo_gpio_exp_irq_enable(gpio_port_t port, gpio_pin_t pin);
void foo_gpio_exp_irq_disable(gpio_port_t port, gpio_pin_t pin);
#endif
gpio_mask_t foo_gpio_exp_read(gpio_port_t port);
void foo_gpio_exp_set(gpio_port_t port, gpio_mask_t pins);
void foo_gpio_exp_clear(gpio_port_t port, gpio_mask_t pins);
void foo_gpio_exp_toggle(gpio_port_t port, gpio_mask_t pins);
void foo_gpio_exp_write(gpio_port_t port, gpio_mask_t values);
/** @} */

/**
 * @brief   GPIO expander driver structure as required by the GPIO API
 */
extern const gpio_driver_t foo_gpio_exp_driver;

#ifdef __cplusplus
}
#endif

#endif /* FOO_GPIO_EXP_H */
/** @} */
