/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 INRIA
 *               2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief       Peripheral GPIO Low-Level API implementation for the ATmega GPIO peripheral
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 * @author      Torben Petersen <petersen@ibr.cs.tu-bs.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "cpu.h"
#include "irq.h"
#include "periph/gpio_ll.h"

static void _set_dir(gpio_port_t port, uint8_t pin, bool output)
{
    atmega_gpio_port_t *p = (void *)port;
    if (output) {
        p->ddr |= 1U << pin;
    }
    else {
        p-> ddr &= ~(1U << pin);
    }
}

static bool _is_output(gpio_port_t port, uint8_t pin)
{
    atmega_gpio_port_t *p = (void *)port;
    return (p->ddr >> pin) & 1U;
}

static void _set_pull_config(gpio_port_t port, uint8_t pin, gpio_pull_t pull)
{
    atmega_gpio_port_t *p = (void *)port;
    p->port |= pull << pin;
}

int gpio_ll_init(gpio_port_t port, uint8_t pin, const gpio_conf_t *conf)
{
    if ((conf->pull > GPIO_PULL_UP)
        || (conf->state == GPIO_OUTPUT_OPEN_DRAIN)
        || (conf->state == GPIO_OUTPUT_OPEN_SOURCE)) {
        return -ENOTSUP;
    }

    unsigned state = irq_disable();
    if (conf->initial_value) {
        gpio_ll_set(port, 1UL << pin);
    }
    else {
        gpio_ll_clear(port, 1UL << pin);
    }
    _set_dir(port, pin, conf->state == GPIO_OUTPUT_PUSH_PULL);
    if (conf->state == GPIO_INPUT) {
        _set_pull_config(port, pin, conf->pull);
    }
    irq_restore(state);

    return 0;
}

void gpio_ll_query_conf(gpio_conf_t *dest, gpio_port_t port, uint8_t pin)
{
    assert(dest);
    memset(dest, 0, sizeof(*dest));
    /* E.g. the schematics in figure 14-5 in the ATmega328P datasheet shows that
     * a Schmitt Trigger is always connected before the digital input signal.
     * Let's assume this is also true for all other ATmegas */
    dest->schmitt_trigger = true;
    if (_is_output(port, pin)) {
        dest->state = GPIO_OUTPUT_PUSH_PULL;
        dest->initial_value = (gpio_ll_read_output(port) >> pin) & 1U;
    }
    else {
        dest->state = GPIO_INPUT;
        dest->pull = (gpio_ll_read_output(port) >> pin) & 1U;
        dest->initial_value = (gpio_ll_read(port) >> pin) & 1U;
    }
}
