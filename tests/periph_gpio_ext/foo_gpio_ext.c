/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests_periph_gpio_ext
 * @{
 *
 * @file
 * @brief       Example GPIO extender driver
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifdef MODULE_EXTEND_GPIO

#include "foo_gpio_ext.h"

const gpio_driver_t foo_gpio_ext_driver = {
    .init = (gpio_dev_init_t)foo_gpio_ext_init,
#ifdef MODULE_PERIPH_GPIO_IRQ
    .init_int = (gpio_dev_init_int_t)foo_gpio_ext_init_int,
    .irq_enable = (gpio_dev_irq_enable_t)foo_gpio_ext_irq_enable,
    .irq_disable = (gpio_dev_irq_disable_t)foo_gpio_ext_irq_disable,
#endif
    .read = (gpio_dev_read_t)foo_gpio_ext_read,
    .set = (gpio_dev_set_t)foo_gpio_ext_set,
    .clear = (gpio_dev_clear_t)foo_gpio_ext_clear,
    .toggle = (gpio_dev_toggle_t)foo_gpio_ext_toggle,
    .write = (gpio_dev_write_t)foo_gpio_ext_write,
};

int foo_gpio_ext_init(foo_gpio_ext_t *dev, uint8_t pin, gpio_mode_t mode)
{
    (void)mode;
    printf("init dev %p (%s) pin %u\n", dev, dev->name, pin);
    return 0;
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int  foo_gpio_ext_init_int(foo_gpio_ext_t *dev, uint8_t pin, gpio_mode_t mode,
                  gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    (void)mode;
    (void)flank;
    (void)cb;
    (void)arg;
    printf("init_int dev %p (%s) pin %u\n", dev, dev->name, pin);
    return 0;
}

void foo_gpio_ext_irq_enable(foo_gpio_ext_t *dev, uint8_t pin)
{
    printf("irq_enable dev %p (%s) pin %u\n", dev, dev->name, pin);
}

void foo_gpio_ext_irq_disable(foo_gpio_ext_t *dev, uint8_t pin)
{
    printf("irq_disable dev %p (%s) pin %u\n", dev, dev->name, pin);
}
#endif

gpio_mask_t foo_gpio_ext_read(foo_gpio_ext_t *dev)
{
    printf("read dev %p (%s) state 0x%02x\n", dev, dev->name, dev->state);
    return dev->state;
}

void foo_gpio_ext_set(foo_gpio_ext_t *dev, gpio_mask_t pins)
{
    dev->state |= pins;
    printf("set dev %p (%s) pins 0x%02x, state 0x%02x\n",
           dev, dev->name, pins, dev->state);
}

void foo_gpio_ext_clear(foo_gpio_ext_t *dev, gpio_mask_t pins)
{
    dev->state &= ~pins;
    printf("clear dev %p (%s) pins 0x%02x, state 0x%02x\n",
           dev, dev->name, pins, dev->state);
}

void foo_gpio_ext_toggle(foo_gpio_ext_t *dev, gpio_mask_t pins)
{
    dev->state ^= pins;
    printf("toggle dev %p (%s) pins 0x%02x, state 0x%02x\n",
           dev, dev->name, pins, dev->state);}

void foo_gpio_ext_write(foo_gpio_ext_t *dev, gpio_mask_t values)
{
    dev->state = values;
    printf("write dev %p (%s) values 0x%02x, state 0x%02x\n",
           dev, dev->name, values, dev->state);
}

#endif /* MODULE_EXTEND_GPIO */
