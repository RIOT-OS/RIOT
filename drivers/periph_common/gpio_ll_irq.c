/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "periph/gpio_ll_irq.h"
#include <stdint.h>

__attribute__((weak))
void gpio_ll_irq_off(gpio_port_t port, uint8_t pin)
{
    gpio_ll_irq_mask(port, pin);
}
