/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         tests
 * @{
 *
 * @file
 * @brief           Test application for periph_gpio that does not rely on working stdio
 *
 * @author          Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdlib.h>

#include "periph/gpio.h"
#include "test_utils/expect.h"

static gpio_t pin_in = PIN_IN;
static gpio_t pin_in_pu = PIN_IN_PU;
static gpio_t pin_in_pd = PIN_IN_PD;
static gpio_t pin_irq_rising = PIN_IRQ_RISING;
static gpio_t pin_irq_falling = PIN_IRQ_FALLING;
static gpio_t pin_irq_both = PIN_IRQ_BOTH;
static gpio_t pin_out_1 = PIN_OUT_1;
static gpio_t pin_out_2 = PIN_OUT_2;
static gpio_t pin_out_3 = PIN_OUT_3;
static gpio_t pin_out_4 = PIN_OUT_4;
static gpio_t pin_out_5 = PIN_OUT_5;
static gpio_t pin_out_6 = PIN_OUT_6;

static void _cb(void *arg)
{
    gpio_t *pin = arg;
    gpio_toggle(*pin);
}

int main(void)
{
    expect(0 == gpio_init(pin_in, GPIO_IN));
    expect(0 == gpio_init(pin_out_1, GPIO_OUT));

    if (!IS_ACTIVE(NO_PULL_UP)) {
        expect(0 == gpio_init(pin_in_pu, GPIO_IN_PU));
        expect(0 == gpio_init(pin_out_2, GPIO_OUT));
    }

    if (!IS_ACTIVE(NO_PULL_DOWN)) {
        expect(0 == gpio_init(pin_in_pd, GPIO_IN_PD));
        expect(0 == gpio_init(pin_out_3, GPIO_OUT));
    }

    if (IS_USED(MODULE_PERIPH_GPIO_IRQ)) {
        expect(0 == gpio_init(pin_out_4, GPIO_OUT));
        expect(0 == gpio_init(pin_out_5, GPIO_OUT));
        expect(0 == gpio_init(pin_out_6, GPIO_OUT));
        expect(0 == gpio_init_int(pin_irq_rising, PIN_IRQ_MODE, GPIO_RISING, _cb, &pin_out_4));
        expect(0 == gpio_init_int(pin_irq_falling, PIN_IRQ_MODE, GPIO_FALLING, _cb, &pin_out_5));
        expect(0 == gpio_init_int(pin_irq_both, PIN_IRQ_MODE, GPIO_BOTH, _cb, &pin_out_6));
    }

    while (1) {
        gpio_write(pin_out_1, gpio_read(pin_in));
        if (!IS_ACTIVE(NO_PULL_UP)) {
            gpio_write(pin_out_2, gpio_read(pin_in_pu));
        }
        if (!IS_ACTIVE(NO_PULL_DOWN)) {
            gpio_write(pin_out_3, gpio_read(pin_in_pd));
        }
    }

    return 0;
}
