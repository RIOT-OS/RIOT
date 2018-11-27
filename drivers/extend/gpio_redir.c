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
 * @brief       GPIO extension redirection functions
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#if MODULE_EXTEND_GPIO

#include "periph/gpio.h"
#include "extend/gpio.h"
#include "gpio_ext_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

gpio_ext_t *gpio_ext_entry(gpio_t gpio)
{
    if (gpio == GPIO_UNDEF) {
        return NULL;
    }

    gpio_t devnum = gpio_ext_dev(gpio);

    DEBUG("[gpio_ext_entry] list entry is %u\n", devnum);

    /* device is greater than number of listed entries */
    if (devnum >= (sizeof(gpio_ext_list) / sizeof(gpio_ext_list[0]))) {
        return NULL;
    }

    /* Cast to discard const */
    return (gpio_ext_t *)&gpio_ext_list[devnum];
}

int gpio_init_redir(gpio_t pin, gpio_mode_t mode)
{
    gpio_ext_t *entry = gpio_ext_entry(pin);

    if (entry == NULL) {
        DEBUG("[gpio_init_redir] ext entry doesn't exist for %X\n", pin);
        return -1;
    }

    pin = gpio_ext_pin(pin);

    return entry->driver->init(entry->dev, pin, mode);
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int gpio_init_int_redir(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                        gpio_cb_t cb, void *arg)
{
    gpio_ext_t *entry = gpio_ext_entry(pin);

    if (entry == NULL) {
        DEBUG("[gpio_init_int_redir] ext entry doesn't exist for %X\n", pin);
        return -1;
    }

    pin = gpio_ext_pin(pin);

    return entry->driver->init_int(entry->dev, pin, mode, flank, cb, arg);
}
#endif /* MODULE_PERIPH_GPIO_IRQ */

void gpio_irq_enable_redir(gpio_t pin)
{
    gpio_ext_t *entry = gpio_ext_entry(pin);

    if (entry == NULL) {
        DEBUG("[gpio_irq_enable_redir] ext entry doesn't exist for %X\n", pin);
        return;
    }

    pin = gpio_ext_pin(pin);

    entry->driver->irq_enable(entry->dev, pin);
}

void gpio_irq_disable_redir(gpio_t pin)
{
    gpio_ext_t *entry = gpio_ext_entry(pin);

    if (entry == NULL) {
        DEBUG("[gpio_irq_disable_redir] ext entry doesn't exist for %X\n", pin);
        return;
    }

    pin = gpio_ext_pin(pin);

    entry->driver->irq_disable(entry->dev, pin);
}

int gpio_read_redir(gpio_t pin)
{
    gpio_ext_t *entry = gpio_ext_entry(pin);

    if (entry == NULL) {
        DEBUG("[gpio_read_redir] ext entry doesn't exist for %X\n", pin);
        return 0;
    }

    pin = gpio_ext_pin(pin);

    return entry->driver->read(entry->dev, pin);
}

void gpio_set_redir(gpio_t pin)
{
    gpio_ext_t *entry = gpio_ext_entry(pin);

    if (entry == NULL) {
        DEBUG("[gpio_set_redir] ext entry doesn't exist for %X\n", pin);
        return;
    }

    pin = gpio_ext_pin(pin);

    entry->driver->set(entry->dev, pin);
}

void gpio_clear_redir(gpio_t pin)
{
    gpio_ext_t *entry = gpio_ext_entry(pin);

    if (entry == NULL) {
        DEBUG("[gpio_clear_redir] ext entry doesn't exist for %X\n", pin);
        return;
    }

    pin = gpio_ext_pin(pin);

    entry->driver->clear(entry->dev, pin);
}

void gpio_toggle_redir(gpio_t pin)
{
    gpio_ext_t *entry = gpio_ext_entry(pin);

    if (entry == NULL) {
        DEBUG("[gpio_toggle_redir] ext entry doesn't exist for %X\n", pin);
        return;
    }

    pin = gpio_ext_pin(pin);

    entry->driver->toggle(entry->dev, pin);
}

void gpio_write_redir(gpio_t pin, int value)
{
    gpio_ext_t *entry = gpio_ext_entry(pin);

    if (entry == NULL) {
        DEBUG("[gpio_write_redir] ext entry doesn't exist for %X\n", pin);
        return;
    }

    pin = gpio_ext_pin(pin);

    entry->driver->write(entry->dev, pin, value);
}

#endif /* MODULE_EXTEND_GPIO */
