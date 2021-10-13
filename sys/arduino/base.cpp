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
#include "assert.h"
#include "ztimer.h"
#include "periph/gpio.h"
#include "periph/adc.h"
#include "periph/pwm.h"
}

#include "arduino.hpp"

#define ANALOG_PIN_NUMOF     (ARRAY_SIZE(arduino_analog_map))

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
    ztimer_sleep(ZTIMER_MSEC, msec);
}

void delayMicroseconds(unsigned long usec)
{
    ztimer_sleep(ZTIMER_USEC, usec);
}

unsigned long micros()
{
    return ztimer_now(ZTIMER_USEC);
}

unsigned long millis()
{
    return ztimer_now(ZTIMER_MSEC);
}

#if MODULE_PERIPH_ADC
int analogRead(int arduino_pin)
{
    /*
    * Bitfield for the state of the ADC-channels.
    * 0: Not initialized
    * 1: Successfully initialized
    */
    static uint16_t adc_line_state;
    int adc_value;

    /* Check if the ADC line is valid */
    assert((arduino_pin >= 0) && (arduino_pin < (int)ANALOG_PIN_NUMOF));

    /* Initialization of given ADC channel */
    if (!(adc_line_state & (1 << arduino_pin))) {
        if (adc_init(arduino_analog_map[arduino_pin]) != 0) {
            return -1;
        }
        /* The ADC channel is initialized */
        adc_line_state |= (1 << arduino_pin);
    }

    /* Read the ADC channel */
    adc_value = adc_sample(arduino_analog_map[arduino_pin], ADC_RES_10BIT);

    return adc_value;
}
#endif

#if MODULE_PERIPH_PWM
static int _get_pwm_pin_idx(int pin)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(arduino_pwm_list); ++i) {
        if (arduino_pwm_list[i].pin == pin) {
            return i;
        }
    }

    return -1;
}

void analogWrite(int pin, int value)
{
    /*
    * Bitfield for the state of the PWM devices.
    * 0: Not initialized
    * 1: Successfully initialized
    */
    static uint8_t pwm_dev_state;

    /* Clamp given value within bounds */
    if (value < 0) {
        value = 0;
    }
    if ((unsigned)value >= ARDUINO_PWM_STEPS) {
        value = ARDUINO_PWM_STEPS - 1;
    }

    /* Check if the PWM pin is valid */
    int pin_idx = _get_pwm_pin_idx(pin);
    if (pin_idx == -1) {
        /* Set to digital write if not a PWM pin */
        pinMode(pin, OUTPUT);
        return;
    }

    /* Initialization of given PWM pin */
    if (!(pwm_dev_state & (1 << arduino_pwm_list[pin_idx].dev))) {
        if (pwm_init(arduino_pwm_list[pin_idx].dev,
                     ARDUINO_PWM_MODE, ARDUINO_PWM_FREQU, ARDUINO_PWM_STEPS) == 0) {
            return;
        }
        /* The PWM channel is initialized */
        pwm_dev_state |= (1 << arduino_pwm_list[pin_idx].dev);
    }

    /* Write analog value */
    pwm_set(arduino_pwm_list[pin_idx].dev, arduino_pwm_list[pin_idx].chan, value);
}
#endif /* MODULE_PERIPH_PWM */
