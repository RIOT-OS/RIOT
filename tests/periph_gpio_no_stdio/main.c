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

#ifndef PIN_IN
#define PIN_IN          GPIO_PIN(0, 0)
#endif

#ifndef PIN_IN_PU
#define PIN_IN_PU       GPIO_PIN(0, 1)
#endif

#ifndef PIN_IN_PD
#define PIN_IN_PD       GPIO_PIN(0, 2)
#endif

#ifndef PIN_IRQ_MODE
#define PIN_IRQ_MODE    GPIO_IN_PU
#endif

#ifndef PIN_IRQ_RISING
#define PIN_IRQ_RISING  GPIO_PIN(0, 3)
#endif

#ifndef PIN_IRQ_FALLING
#define PIN_IRQ_FALLING GPIO_PIN(0, 4)
#endif

#ifndef PIN_IRQ_BOTH
#define PIN_IRQ_BOTH    GPIO_PIN(0, 5)
#endif

#ifndef PIN_OUT_1
#define PIN_OUT_1       GPIO_PIN(0, 6)
#endif

#ifndef PIN_OUT_2
#define PIN_OUT_2       GPIO_PIN(0, 7)
#endif

#ifndef PIN_OUT_3
#define PIN_OUT_3       GPIO_PIN(0, 8)
#endif

#ifndef PIN_OUT_4
#define PIN_OUT_4       GPIO_PIN(0, 9)
#endif

#ifndef PIN_OUT_5
#define PIN_OUT_5       GPIO_PIN(0, 10)
#endif

#ifndef PIN_OUT_6
#define PIN_OUT_6       GPIO_PIN(0, 11)
#endif

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
    expect(0 == gpio_init(pin_out_1, GPIO_OUT));
    expect(0 == gpio_init(pin_out_2, GPIO_OUT));
    expect(0 == gpio_init(pin_out_3, GPIO_OUT));
    expect(0 == gpio_init(pin_in, GPIO_IN));
    expect(0 == gpio_init(pin_in_pu, GPIO_IN_PU));
    expect(0 == gpio_init(pin_in_pd, GPIO_IN_PD));

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
        gpio_write(pin_out_2, gpio_read(pin_in_pu));
        gpio_write(pin_out_3, gpio_read(pin_in_pd));
    }

    return 0;
}
