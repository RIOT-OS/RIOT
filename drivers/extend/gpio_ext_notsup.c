/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_extend_gpio
 *
 * @{
 *
 * @file
 * @brief       GPIO extension not-supported functions
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#if MODULE_EXTEND_GPIO

#include "periph/gpio.h"
#include "extend/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int gpio_ext_init_notsup(void *dev, gpio_t pin, gpio_mode_t mode)
{
    (void)dev;
    (void)pin;
    (void)mode;

    DEBUG("[gpio_ext_init_notsup] call for dev %p\n", dev);

    return -1;
}

int gpio_ext_init_int_notsup(void *dev, gpio_t pin, gpio_mode_t mode,
                             gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    (void)dev;
    (void)pin;
    (void)mode;
    (void)flank;
    (void)cb;
    (void)arg;

    DEBUG("[gpio_ext_init_int_notsup] call for dev %p\n", dev);

    return -1;
}

void gpio_ext_irq_enable_notsup(void *dev, gpio_t pin)
{
    (void)dev;
    (void)pin;

    DEBUG("[gpio_ext_irq_enable_notsup] call for dev %p\n", dev);
}

void gpio_ext_irq_disable_notsup(void *dev, gpio_t pin)
{
    (void)dev;
    (void)pin;

    DEBUG("[gpio_ext_irq_disable_notsup] call for dev %p\n", dev);
}

int gpio_ext_read_notsup(void *dev, gpio_t pin)
{
    (void)dev;
    (void)pin;

    DEBUG("[gpio_ext_read_notsup] call for dev %p\n", dev);

    return 0;
}

void gpio_ext_set_notsup(void *dev, gpio_t pin)
{
    (void)dev;
    (void)pin;

    DEBUG("[gpio_ext_set_notsup] call for dev %p\n", dev);
}

void gpio_ext_clear_notsup(void *dev, gpio_t pin)
{
    (void)dev;
    (void)pin;

    DEBUG("[gpio_ext_clear_notsup] call for dev %p\n", dev);
}

void gpio_toggle_notsup(void *dev, gpio_t pin)
{
    (void)dev;
    (void)pin;

    DEBUG("[gpio_toggle_notsup] call for dev %p\n", dev);
}

void gpio_ext_write_notsup(void *dev, gpio_t pin, int value)
{
    (void)dev;
    (void)pin;
    (void)value;

    DEBUG("[gpio_ext_write_notsup] call for dev %p\n", dev);
}

/* not-supported driver */
const gpio_ext_driver_t gpio_ext_notsup_driver = {
    .init = gpio_ext_init_notsup,
#ifdef MODULE_PERIPH_GPIO_IRQ
    .init_int = gpio_ext_init_int_notsup,
    .irq_enable = gpio_ext_irq_enable_notsup,
    .irq_disable = gpio_ext_irq_disable_notsup,
#endif /* MODULE_PERIPH_GPIO_IRQ */
    .read = gpio_ext_read_notsup,
    .set = gpio_ext_set_notsup,
    .clear = gpio_ext_clear_notsup,
    .toggle = gpio_toggle_notsup,
    .write = gpio_ext_write_notsup,
};

#endif /* MODULE_EXTEND_GPIO */
