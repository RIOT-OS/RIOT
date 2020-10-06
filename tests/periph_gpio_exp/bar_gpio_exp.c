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
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "bar_gpio_exp.h"

const gpio_driver_t bar_gpio_exp_driver = {
    .init = bar_gpio_exp_init,
#ifdef MODULE_PERIPH_GPIO_IRQ
    .init_int = bar_gpio_exp_init_int,
    .irq_enable = bar_gpio_exp_irq_enable,
    .irq_disable = bar_gpio_exp_irq_disable,
#endif
    .read = bar_gpio_exp_read,
    .set = bar_gpio_exp_set,
    .clear = bar_gpio_exp_clear,
    .toggle = bar_gpio_exp_toggle,
    .write = bar_gpio_exp_write,
};

int bar_exp_init(bar_exp_t *dev, uint8_t port, uint8_t pin, gpio_mode_t mode)
{
    (void)mode;
    assert(dev);

    printf("init dev %p (%s) port %u, pin %u\n", dev, dev->name, port, pin);
    return 0;
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int  bar_exp_init_int(bar_exp_t *dev, uint8_t port, uint8_t pin,
                      gpio_mode_t mode, gpio_flank_t flank,
                      gpio_cb_t cb, void *arg)
{
    (void)mode;
    (void)flank;
    (void)cb;
    (void)arg;
    assert(dev);

    printf("init_int dev %p (%s) port %u, pin %u\n",
           dev, dev->name, port, pin);
    return 0;
}

void bar_exp_irq_enable(bar_exp_t *dev, uint8_t port, uint8_t pin)
{
    assert(dev);
    printf("irq_enable dev %p (%s) port %u, pin %u\n",
           dev, dev->name, port, pin);
}

void bar_exp_irq_disable(bar_exp_t *dev, uint8_t port, uint8_t pin)
{
    assert(dev);
    printf("irq_disable dev %p (%s) port %u, pin %u\n",
           dev, dev->name, port, pin);
}
#endif

gpio_mask_t bar_exp_read(bar_exp_t *dev, uint8_t port)
{
    assert(dev);
    printf("read dev %p (%s) port %u, state 0x%02x\n",
           dev, dev->name, port, dev->state[port]);
    return dev->state[port];
}

void bar_exp_set(bar_exp_t *dev, uint8_t port, gpio_mask_t pins)
{
    assert(dev);

    dev->state[port] |= pins;
    printf("set dev %p (%s) port %u, pins 0x%02x, state 0x%02x\n",
           dev, dev->name, port, pins, dev->state[port]);
}

void bar_exp_clear(bar_exp_t *dev, uint8_t port, gpio_mask_t pins)
{
    assert(dev);

    dev->state[port] &= ~pins;
    printf("clear dev %p (%s) port %u, pins 0x%02x, state 0x%02x\n",
           dev, dev->name, port, pins, dev->state[port]);
}

void bar_exp_toggle(bar_exp_t *dev, uint8_t port, gpio_mask_t pins)
{
    assert(dev);

    dev->state[port] ^= pins;
    printf("toggle dev %p (%s) port %u, pins 0x%02x, state 0x%02x\n",
           dev, dev->name, port, pins, dev->state[port]);}

void bar_exp_write(bar_exp_t *dev, uint8_t port, gpio_mask_t values)
{
    assert(dev);

    dev->state[port] = values;
    printf("write dev %p (%s) port %u, values 0x%02x, state 0x%02x\n",
           dev, dev->name, port, values, dev->state[port]);
}
