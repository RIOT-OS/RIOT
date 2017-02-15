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
#include "periph/adc.h"
}

#include "arduino.hpp"

#define ANALOG_PIN_NUMOF     (sizeof(arduino_analog_map) / sizeof(arduino_analog_map[0]))

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

/*
 * Bitfield for the state of the ADC-channels.
 * 0: Not initialized
 * 1: Successfully initialized
 */
static uint16_t adc_line_state = 0;

int analogRead(int arduino_pin)
{
    int adc_value;

    /* Check if the ADC line is valid */
    assert((arduino_pin >= 0) && (arduino_pin < (int)ANALOG_PIN_NUMOF));

    /* Initialization of given ADC channel */
    if (!(adc_line_state & (1 << arduino_pin))) {
        if (adc_init(arduino_analog_map[arduino_pin]) != 0) {
            return -1;
        }
        /* The ADC channel is initialized */
        adc_line_state |= 1 << arduino_pin;
    }

    /* Read the ADC channel */
    adc_value = adc_sample(arduino_analog_map[arduino_pin], ADC_RES_10BIT);

    return adc_value;
}
