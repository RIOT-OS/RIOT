/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_gpio_exp
 *
 * @{
 *
 * @file
 * @brief       GPIO expansion functions
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <errno.h>

#include "gpio_exp.h"
#include "gpio_exp_conf.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int gpio_exp_init_notsup(void *dev, gpio_t pin, gpio_mode_t mode)
{
    (void)dev;
    (void)pin;
    (void)mode;

    DEBUG("[gpio_exp] notsup init called for dev %p, pin %u\n",
        dev, (unsigned int)pin);

    return -ENOTSUP;
}

int gpio_exp_init_int_notsup(void *dev, gpio_t pin,
                             gpio_mode_t mode, gpio_flank_t flank,
                             gpio_cb_t cb, void *arg)
{
    (void)dev;
    (void)pin;
    (void)mode;
    (void)flank;
    (void)cb;
    (void)arg;

    DEBUG("[gpio_exp] notsup init_int called for dev %p, pin %u\n",
        dev, (unsigned int)pin);

    return -ENOTSUP;
}

void gpio_exp_irq_notsup(void *dev, gpio_t pin, int state)
{
    (void)dev;
    (void)pin;
    (void)state;

    DEBUG("[gpio_exp] notsup irq called for dev %p, pin %u\n",
        dev, (unsigned int)pin);
}

int gpio_exp_read_notsup(void *dev, gpio_t pin)
{
    (void)dev;
    (void)pin;

    DEBUG("[gpio_exp] notsup read called for dev %p, pin %u\n",
        dev, (unsigned int)pin);

    return -ENOTSUP;
}

void gpio_exp_write_notsup(void *dev, gpio_t pin, int value)
{
    (void)dev;
    (void)pin;
    (void)value;

    DEBUG("[gpio_exp] notsup write called for dev %p, pin %u\n",
        dev, (unsigned int)pin);
}

gpio_exp_t *gpio_exp_entry(gpio_t gpio)
{
    uint8_t num;

    if (gpio == GPIO_UNDEF) {
        return NULL;
    }

    num = (uint8_t)((gpio >> GPIO_EXP_DEV_LOC) & GPIO_EXP_MAXDEVMASK);

    /* device is greater than number of listed entries */
    if (num > (sizeof(gpio_exp_list) / sizeof(gpio_exp_list[0]))) {
        return NULL;
    }

    DEBUG("[gpio_exp] returning list entry %u\n", num);

    /* Cast to discard const */
    return (gpio_exp_t *)&(gpio_exp_list[num]);
}

gpio_t gpio_exp_pin(gpio_t gpio)
{
    return (gpio & GPIO_EXP_MAXPINMASK);
}
