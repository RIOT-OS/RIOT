/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 *
 * @}
 */

#include "servo.h"
#include "periph/pwm.h"

#define FREQUENCY       (100U)
#define RESOLUTION      (10000U)

int servo_init(servo_t *dev, pwm_t pwm, int pwm_channel, unsigned int min, unsigned int max)
{
    dev->device = pwm;
    dev->channel = pwm_channel;
    dev->min = min;
    dev->max = max;

    return pwm_init(dev->device, PWM_LEFT, FREQUENCY, RESOLUTION);
}

int servo_set(servo_t *dev, unsigned int pos)
{
    if (pos > dev->max) {
        pos = dev->max;
    }
    else if (pos < dev->min) {
        pos = dev->min;
    }
    return pwm_set(dev->device, dev->channel, pos);
}
