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
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "foo_gpio_exp.h"

const gpio_driver_t foo_gpio_exp_driver = {
    .init = foo_gpio_exp_init,
#ifdef MODULE_PERIPH_GPIO_IRQ
    .init_int = foo_gpio_exp_init_int,
    .irq_enable = foo_gpio_exp_irq_enable,
    .irq_disable = foo_gpio_exp_irq_disable,
#endif
    .read = foo_gpio_exp_read,
    .set = foo_gpio_exp_set,
    .clear = foo_gpio_exp_clear,
    .toggle = foo_gpio_exp_toggle,
    .write = foo_gpio_exp_write,
};

int foo_gpio_exp_init(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode)
{
    (void)mode;
    assert(port.dev);
    assert(port.dev->dev);
    foo_gpio_exp_t *dev = (foo_gpio_exp_t *)port.dev->dev;

    printf("init dev %p (%s) pin %u\n", dev, dev->name, pin);
    return 0;
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int  foo_gpio_exp_init_int(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode,
                           gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    (void)mode;
    (void)flank;
    (void)cb;
    (void)arg;
    assert(port.dev);
    assert(port.dev->dev);
    foo_gpio_exp_t *dev = (foo_gpio_exp_t *)port.dev->dev;

    printf("init_int dev %p (%s) pin %u\n", dev, dev->name, pin);
    return 0;
}

void foo_gpio_exp_irq_enable(gpio_port_t port, gpio_pin_t pin)
{
    assert(port.dev);
    assert(port.dev->dev);
    foo_gpio_exp_t *dev = (foo_gpio_exp_t *)port.dev->dev;

    printf("irq_enable dev %p (%s) pin %u\n", dev, dev->name, pin);
}

void foo_gpio_exp_irq_disable(gpio_port_t port, gpio_pin_t pin)
{
    assert(port.dev);
    assert(port.dev->dev);
    foo_gpio_exp_t *dev = (foo_gpio_exp_t *)port.dev->dev;

    printf("irq_disable dev %p (%s) pin %u\n", dev, dev->name, pin);
}
#endif

gpio_mask_t foo_gpio_exp_read(gpio_port_t port)
{
    assert(port.dev);
    assert(port.dev->dev);
    foo_gpio_exp_t *dev = (foo_gpio_exp_t *)port.dev->dev;

    printf("read dev %p (%s) state 0x%02x\n", dev, dev->name, dev->state);
    return dev->state;
}

void foo_gpio_exp_set(gpio_port_t port, gpio_mask_t pins)
{
    assert(port.dev);
    assert(port.dev->dev);
    foo_gpio_exp_t *dev = (foo_gpio_exp_t *)port.dev->dev;

    dev->state |= pins;
    printf("set dev %p (%s) pins 0x%02x, state 0x%02x\n",
           dev, dev->name, pins, dev->state);
}

void foo_gpio_exp_clear(gpio_port_t port, gpio_mask_t pins)
{
    assert(port.dev);
    assert(port.dev->dev);
    foo_gpio_exp_t *dev = (foo_gpio_exp_t *)port.dev->dev;

    dev->state &= ~pins;
    printf("clear dev %p (%s) pins 0x%02x, state 0x%02x\n",
           dev, dev->name, pins, dev->state);
}

void foo_gpio_exp_toggle(gpio_port_t port, gpio_mask_t pins)
{
    assert(port.dev);
    assert(port.dev->dev);
    foo_gpio_exp_t *dev = (foo_gpio_exp_t *)port.dev->dev;

    dev->state ^= pins;
    printf("toggle dev %p (%s) pins 0x%02x, state 0x%02x\n",
           dev, dev->name, pins, dev->state);}

void foo_gpio_exp_write(gpio_port_t port, gpio_mask_t values)
{
    assert(port.dev);
    assert(port.dev->dev);
    foo_gpio_exp_t *dev = (foo_gpio_exp_t *)port.dev->dev;

    dev->state = values;
    printf("write dev %p (%s) values 0x%02x, state 0x%02x\n",
           dev, dev->name, values, dev->state);
}
