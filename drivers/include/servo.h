/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_servo Servo Motor Driver
 * @ingroup     drivers_actuators
 * @brief       High-level driver for servo motors
 * @{
 *
 * @file
 * @brief       High-level driver for easy handling of servo motors
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef SERVO_H
#define SERVO_H

#include "periph/pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Descriptor struct for a servo
 */
typedef struct {
    pwm_t device;           /**< the PWM device driving the servo */
    int channel;            /**< the channel the servo is connected to */
    unsigned int min;       /**< minimum pulse width, in us */
    unsigned int max;       /**< maximum pulse width, in us */
    unsigned int scale_nom; /**< timing scale factor, to adjust for an inexact PWM frequency, nominator */
    unsigned int scale_den; /**< timing scale factor, to adjust for an inexact PWM frequency, denominator */
} servo_t;

/**
 * @brief   Initialize a servo motor by assigning it a PWM device and channel
 *
 * Digital servos are controlled by regular pulses sent to them. The width
 * of a pulse determines the position of the servo. A pulse width of 1.5ms
 * puts the servo in the center position, a pulse width of about 1.0ms and
 * about 2.0ms put the servo to the maximum angles. These values can however
 * differ slightly from servo to servo, so the min and max values are
 * parameterized in the init function.
 *
 * The servo is initialized with default PWM values:
 * - frequency: 100Hz (10ms interval)
 * - resolution: 10000 (1000 steps per ms)
 *
 * These default values can be changed by setting SERVO_RESOLUTION and
 * SERVO_FREQUENCY macros.
 * Caution: When initializing a servo, the PWM device will be reconfigured to
 * new frequency/resolution values. It is however fine to use multiple servos
 * with the same PWM device, just on different channels.
 *
 * @param[out] dev          struct describing the servo
 * @param[in] pwm           the PWM device the servo is connected to
 * @param[in] pwm_channel   the PWM channel the servo is connected to
 * @param[in] min           minimum pulse width (in the resolution range)
 * @param[in] max           maximum pulse width (in the resolution range)
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int servo_init(servo_t *dev, pwm_t pwm, int pwm_channel, unsigned int min, unsigned int max);

/**
 * @brief   Set the servo motor to a specified position
 *
 * The position of the servo is specified in the pulse width that
 * controls the servo. With default configurations, a value of 1500
 * means a pulse width of 1.5 ms, which is the center position on
 * most servos.
 *
 * In case pos is larger/smaller then the max/min values, pos will be set to
 * these values.
 *
 * @param[in] dev           the servo to set
 * @param[in] pos           the position to set the servo (in the resolution range)
 */
void servo_set(const servo_t *dev, unsigned int pos);

#ifdef __cplusplus
}
#endif

#endif /* SERVO_H */
/** @} */
