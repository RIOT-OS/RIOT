/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_servo
 * @{
 *
 * @file
 * @brief       Servo motor driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "servo.h"
#include "periph/pwm.h"
#include "timex.h" /* for US_PER_SEC */

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifndef SERVO_FREQUENCY
#define SERVO_FREQUENCY       (100U)
#endif

#ifndef SERVO_RESOLUTION
#define SERVO_RESOLUTION      (US_PER_SEC / SERVO_FREQUENCY)
#endif

int servo_init(servo_t *dev, pwm_t pwm, int pwm_channel, unsigned int min, unsigned int max)
{
    int actual_frequency;

    actual_frequency = pwm_init(pwm, PWM_LEFT, SERVO_FREQUENCY, SERVO_RESOLUTION);

    DEBUG("servo: requested %d hz, got %d hz\n", SERVO_FREQUENCY, actual_frequency);

    if (actual_frequency < 0) {
        /* PWM error */
        return -1;
    }
    dev->device = pwm;
    dev->channel = pwm_channel;
    dev->min = min;
    dev->max = max;

    /* Compute scaling fractional */
    /*
     * The PWM pulse width can be written as:
     *
     * t = k / (f * r)
     *
     * where t is the pulse high time, k is the value set in the PWM peripheral,
     * f is the frequency, and r is the resolution of the PWM module.
     *
     * define t0 as the desired pulse width:
     *
     * t0 = k0 / (f0 * r)
     *
     * where f0 is the requested frequency, k0 is the requested number of ticks.
     * Introducing f1 as the closest achievable frequency and k1 as the set tick
     * value yields:
     *
     * t1 = k1 / (f1 * r)
     *
     * setting t1 = t0 and substituting k1 = k0 * s yields:
     *
     * k0 / (f0 * r) = k0 * s / (f1 * r)
     *
     * solve for s:
     *
     * s = f1 / f0
     *
     * where s is the optimal scale factor to translate from requested position
     * to actual hardware ticks.
     */
    dev->scale_nom = actual_frequency;
    dev->scale_den = SERVO_FREQUENCY;

    return 0;
}

void servo_set(servo_t *dev, unsigned int pos)
{
    unsigned int raw_value;
    if (pos > dev->max) {
        pos = dev->max;
    }
    else if (pos < dev->min) {
        pos = dev->min;
    }

    /* rescale value to match PWM peripheral configuration */
    raw_value = (pos * dev->scale_nom) / dev->scale_den;

    DEBUG("servo_set: pos %d -> raw %d\n", pos, raw_value);

    pwm_set(dev->device, dev->channel, raw_value);
}
