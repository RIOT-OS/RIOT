/*
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "periph/gpio_ll_irq.h"
#include <stdint.h>

__attribute__((weak))
void gpio_ll_irq_off(gpio_port_t port, uint8_t pin)
{
    gpio_ll_irq_mask(port, pin);
}
