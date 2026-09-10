/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_noelv
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver for GRLIB GRGPIO
 *
 * @author      Matvii Ivashchenko
 * @}
 */

#include "periph/gpio.h"
#include "periph_conf.h"
#include "irq.h"
#include "vendor/grgpio.h"

/**
 * @brief   Extract the pin number (bit position) from a gpio_t value.
 *
 * GPIO_PIN() packs the pin into the low GPIO_PORT_SHIFT bits (GPIO_PIN_MASK).
 */
static inline int _pin(gpio_t g)
{
    return (int)(g & GPIO_PIN_MASK);
}

/**
 * @brief   Extract the port index from a gpio_t value and return its base addr.
 *
 * Currently only port 0 (GRGPIO0 at GPIO0_BASE_ADDR) is supported.
 */
static inline grgpio_t *_dev(gpio_t g)
{
    (void)g; /* single controller — port field ignored */
    return GRGPIO_DEV(GPIO0_BASE_ADDR);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    grgpio_t *dev = _dev(pin);
    int p = _pin(pin);

    switch (mode) {
    case GPIO_OUT: {
        unsigned state = irq_disable();
        dev->dir |= (1u << p);
        irq_restore(state);
        break;
    }
    case GPIO_IN:
    case GPIO_IN_PD: /* GRGPIO has no pull resistors — treated as plain input */
    case GPIO_IN_PU: {
        unsigned state = irq_disable();
        dev->dir &= ~(1u << p);
        irq_restore(state);
        break;
    }
    default:
        return -1;
    }
    return 0;
}

bool gpio_read(gpio_t pin)
{
    return (_dev(pin)->data >> _pin(pin)) & 1u;
}

void gpio_set(gpio_t pin)
{
    unsigned state = irq_disable();
    _dev(pin)->output |= (1u << _pin(pin));
    irq_restore(state);
}

void gpio_clear(gpio_t pin)
{
    unsigned state = irq_disable();
    _dev(pin)->output &= ~(1u << _pin(pin));
    irq_restore(state);
}

void gpio_toggle(gpio_t pin)
{
    unsigned state = irq_disable();
    _dev(pin)->output ^= (1u << _pin(pin));
    irq_restore(state);
}

void gpio_write(gpio_t pin, bool value)
{
    unsigned state = irq_disable();
    if (value) {
        _dev(pin)->output |= (1u << _pin(pin));
    }
    else {
        _dev(pin)->output &= ~(1u << _pin(pin));
    }
    irq_restore(state);
}
