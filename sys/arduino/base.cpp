/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_arduino
 * @{
 *
 * @file
 * @brief       Implementation of Arduino core functionality
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

extern "C" {
#include "xtimer.h"
#include "periph/gpio.h"
#ifdef ADC_NUMOF
#include "periph/adc.h"
#endif
}

#include "arduino.hpp"

void pinMode(int pin, int mode)
{
    gpio_mode_t m = GPIO_OUT;

    if (mode == INPUT) {
        m = GPIO_IN;
    }
    else if (mode == INPUT_PULLUP) {
        m = GPIO_IN_PU;
    }

    gpio_init(arduino_pinmap[pin], m);
}

void digitalWrite(int pin, int state)
{
    gpio_write(arduino_pinmap[pin], state);
}

int digitalRead(int pin)
{
    if (gpio_read(arduino_pinmap[pin])) {
        return HIGH;
    }
    else {
        return LOW;
    }
}

void delay(unsigned long msec)
{
    xtimer_usleep(1000 * msec);
}

#ifdef ADC_NUMOF
int analogRead(int pin)
{
    adc_init((adc_t)pin);
    return adc_sample((adc_t)pin, (adc_res_t)0);
}
#endif
