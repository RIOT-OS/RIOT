/**
 * Native dummy GPIO interface
 *
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup native_cpu
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#include <stdio.h>

#include "periph/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

typedef enum {
    _NATIVE_GPIO_CONF_NONE,
    _NATIVE_GPIO_CONF_IN,
    _NATIVE_GPIO_CONF_OUT,
    _NATIVE_GPIO_CONF_INT,
} _native_gpio_conf_t;

typedef struct {
    _native_gpio_conf_t conf;
    int pin_state;
    int int_enabled;
    gpio_pp_t pp;
    gpio_flank_t flank;
    gpio_cb_t cb;
    void *cb_arg;
} _native_gpio_state_t;

_native_gpio_state_t gpio_device_state[GPIO_NUMOF];

int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    gpio_device_state[dev].pp = pullup;
    gpio_device_state[dev].conf = _NATIVE_GPIO_CONF_OUT;
    return 0;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
{
    gpio_device_state[dev].pp = pullup;
    gpio_device_state[dev].conf = _NATIVE_GPIO_CONF_IN;
    return 0;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    gpio_device_state[dev].pp = pullup;
    gpio_device_state[dev].flank = flank;
    gpio_device_state[dev].cb = cb;
    gpio_device_state[dev].cb_arg = arg;
    gpio_device_state[dev].conf = _NATIVE_GPIO_CONF_INT;
    gpio_irq_enable(dev);
    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    gpio_device_state[dev].int_enabled = 1;
}

void gpio_irq_disable(gpio_t dev)
{
    gpio_device_state[dev].int_enabled = 0;
}

int gpio_read(gpio_t dev)
{
    return gpio_device_state[dev].pin_state;
}

void gpio_set(gpio_t dev)
{
    printf("Native GPIO device %i is now HIGH\n", dev);
    gpio_device_state[dev].pin_state = 1;
}

void gpio_clear(gpio_t dev)
{
    printf("Native GPIO device %i is now LOW\n", dev);
    gpio_device_state[dev].pin_state = 0;
}

void gpio_toggle(gpio_t dev)
{
    gpio_device_state[dev].pin_state = !gpio_device_state[dev].pin_state;
    printf("Native GPIO device %i is now %s\n", dev,
            (gpio_device_state[dev].pin_state == 0) ? "LOW" : "HIGH");
}

void gpio_write(gpio_t dev, int value)
{
    gpio_device_state[dev].pin_state = (value == 0) ? 0 : 1;
    printf("Native GPIO device %i is now %s\n", dev,
            (gpio_device_state[dev].pin_state == 0) ? "LOW" : "HIGH");
}
