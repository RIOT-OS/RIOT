/*
 * SPDX-FileCopyrightText: 2019 Philipp-Alexander Blum <philipp-blum@jakiku.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "periph/gpio.h"
#include <stdint.h>

uint8_t gpio_util_shiftin(gpio_t data_pin, gpio_t clock_pin)
{
    uint8_t byte = 0x00;

    for (unsigned int i = 8; i > 0; i--) {
        gpio_set(clock_pin);
        if (gpio_read(data_pin) > 0) {
            byte |= (1 << (i - 1));
        }
        gpio_clear(clock_pin);
    }

    return byte;
}
