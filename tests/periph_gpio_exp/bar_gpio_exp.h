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
 * @brief       Example driver for a GPIO expander with multiple ports.
 *
 * This example of a GPIO expander driver demonstrates how to implement a
 * driver for a GPIO expander with multiple ports.
 *
 * Since each GPIO expander port is considered as a separate device by the
 * GPIO API, the low-level API cannot be implemented directly. Instead,
 * a device-specific driver interface must be implemented, which uses a
 * device-specific device descriptor and handles the different ports.
 * The low-level GPIO API is then realized by wrapper functions that
 * map the low-level GPIO API to the device-specific driver interface
 * using a separate device descriptor for each port.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BAR_GPIO_EXP_H
#define BAR_GPIO_EXP_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device-specific descriptor used by the driver for a GPIO expander
 *          with 4 ports and 8 pins each.
 */
typedef struct {
    const char *name;   /**< name of the GPIO expander */
    uint8_t state[4];   /**< ports of the GPIO expander with 8 pins */
} bar_exp_t;

/**
 * @brief   Device descriptor for a single port as required by the GPIO API.
 *
 * This descriptor defines exactly one port of the GPIO expander. References
 * to such descriptors are used in the GPIO expander port definition of type
 * #gpio_dev_t by the GPIO API.
 */
typedef struct {
    bar_exp_t *dev;     /**< reference to the device-specific descriptor */
    uint8_t port;       /**< index of the port */
} bar_gpio_exp_t;

/**
 * @name    Device-specific driver interface for the example GPIO expander
 *
 * The functions of the device-specific driver interface could also be used
 * directly by the application without the GPIO API.
 * @{
 */
int bar_exp_init(bar_exp_t *dev, uint8_t port, uint8_t pin, gpio_mode_t mode);
int bar_exp_init_int(bar_exp_t *dev, uint8_t port, uint8_t pin,
                     gpio_mode_t mode, gpio_flank_t flank,
                     gpio_cb_t cb, void *arg);
void bar_exp_irq_enable(bar_exp_t *dev, uint8_t port, uint8_t pin);
void bar_exp_irq_disable(bar_exp_t *dev, uint8_t port, uint8_t pin);
gpio_mask_t bar_exp_read(bar_exp_t *dev, uint8_t port);
void bar_exp_set(bar_exp_t *dev, uint8_t port, gpio_mask_t pins);
void bar_exp_clear(bar_exp_t *dev, uint8_t port, gpio_mask_t pins);
void bar_exp_toggle(bar_exp_t *dev, uint8_t port, gpio_mask_t pins);
void bar_exp_write(bar_exp_t *dev, uint8_t port, gpio_mask_t values);
/** @} */

/**
 * @name    Device-specific implementation of the low-Level GPIO API.
 *
 * The following functions map the low-level GPIO API to the driver interface.
 * This allows to use the GPIO expander via the GPIO API.
 * @{
 */
static inline int bar_gpio_exp_init(gpio_port_t port, gpio_pin_t pin,
                                    gpio_mode_t mode)
{
    assert(port.dev);
    bar_gpio_exp_t *bar = (bar_gpio_exp_t *)port.dev->dev;
    return bar_exp_init(bar->dev, bar->port, pin, mode);
}

#ifdef MODULE_PERIPH_GPIO_IRQ
static inline int bar_gpio_exp_init_int(gpio_port_t port, gpio_pin_t pin,
                                        gpio_mode_t mode, gpio_flank_t flank,
                                        gpio_cb_t cb, void *arg)
{
    assert(port.dev);
    bar_gpio_exp_t *bar = (bar_gpio_exp_t *)port.dev->dev;
    return bar_exp_init_int(bar->dev, bar->port, pin, mode, flank, cb, arg);
}

static inline void bar_gpio_exp_irq_enable(gpio_port_t port, gpio_pin_t pin)
{
    assert(port.dev);
    bar_gpio_exp_t *bar = (bar_gpio_exp_t *)port.dev->dev;
    bar_exp_irq_enable(bar->dev, bar->port, pin);
}

static inline void bar_gpio_exp_irq_disable(gpio_port_t port, gpio_pin_t pin)
{
    assert(port.dev);
    bar_gpio_exp_t *bar = (bar_gpio_exp_t *)port.dev->dev;
    bar_exp_irq_disable(bar->dev, bar->port, pin);
}
#endif

static inline gpio_mask_t bar_gpio_exp_read(gpio_port_t port)
{
    assert(port.dev);
    bar_gpio_exp_t *bar = (bar_gpio_exp_t *)port.dev->dev;
    return bar_exp_read(bar->dev, bar->port);
}

static inline void bar_gpio_exp_set(gpio_port_t port, gpio_mask_t pins)
{
    assert(port.dev);
    bar_gpio_exp_t *bar = (bar_gpio_exp_t *)port.dev->dev;
    bar_exp_set(bar->dev, bar->port, pins);
}

static inline void bar_gpio_exp_clear(gpio_port_t port, gpio_mask_t pins)
{
    assert(port.dev);
    bar_gpio_exp_t *bar = (bar_gpio_exp_t *)port.dev->dev;
    bar_exp_clear(bar->dev, bar->port, pins);
}

static inline void bar_gpio_exp_toggle(gpio_port_t port, gpio_mask_t pins)
{
    assert(port.dev);
    bar_gpio_exp_t *bar = (bar_gpio_exp_t *)port.dev->dev;
    bar_exp_toggle(bar->dev, bar->port, pins);
}

static inline void bar_gpio_exp_write(gpio_port_t port, gpio_mask_t values)
{
    assert(port.dev);
    bar_gpio_exp_t *bar = (bar_gpio_exp_t *)port.dev->dev;
    bar_exp_write(bar->dev, bar->port, values);
}

/** @} */

/**
 * @brief   GPIO expander driver structure as required by the GPIO API
 */
extern const gpio_driver_t bar_gpio_exp_driver;

#ifdef __cplusplus
}
#endif

#endif /* BAR_GPIO_EXP_H */
/** @} */
