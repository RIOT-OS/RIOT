/*
 * SPDX-FileCopyrightText: 2015 HAW Hamburg
 * SPDX-FileCopyrightText: 2016 INRIA
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
        p->ddr &= ~(1U << pin);
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

int gpio_ll_init(gpio_port_t port, uint8_t pin, gpio_conf_t conf)
{
    if ((conf.pull > GPIO_PULL_UP)
        || (conf.state > GPIO_OUTPUT_PUSH_PULL)) {
        return -ENOTSUP;
    }

    unsigned state = irq_disable();
    if (conf.initial_value) {
        gpio_ll_set(port, 1UL << pin);
    }
    else {
        gpio_ll_clear(port, 1UL << pin);
    }
    _set_dir(port, pin, conf.state == GPIO_OUTPUT_PUSH_PULL);
    if (conf.state == GPIO_INPUT) {
        _set_pull_config(port, pin, conf.pull);
    }
    irq_restore(state);

    return 0;
}

gpio_conf_t gpio_ll_query_conf(gpio_port_t port, uint8_t pin)
{
    gpio_conf_t result = { 0 };
    if (_is_output(port, pin)) {
        result.state = GPIO_OUTPUT_PUSH_PULL;
        result.initial_value = (gpio_ll_read_output(port) >> pin) & 1U;
    }
    else {
        result.state = GPIO_INPUT;
        result.pull = (gpio_ll_read_output(port) >> pin) & 1U;
        result.initial_value = (gpio_ll_read(port) >> pin) & 1U;
    }

    return result;
}
