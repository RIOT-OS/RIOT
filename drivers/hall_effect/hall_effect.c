/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_hall_effect
 * @{
 *
 * @file
 * @brief       Device driver implementation for a generic hall effect sensor
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 *
 * @}
 */

#include "hall_effect.h"
#include "hall_effect_params.h"

#include <errno.h>
#include "log.h"
#include "ztimer.h"
#include "time_units.h"

/* Prototypes */

static void _pulse_callback(void *arg);
static void _read_reset_pulse_counter(hall_effect_t *dev, int32_t *pulse_counter);
static bool _read_delta_t_direction(hall_effect_t *dev, uint32_t *delta_t, bool *ccw);

/* Public API */

int hall_effect_init(hall_effect_t *dev, const hall_effect_params_t *params)
{
    dev->params = *params;

    if (gpio_init(dev->params.direction, GPIO_IN)) {
        LOG_ERROR("[hall_effect] Failed configuring the direction pin as an input!\n");
        return -EIO;
    }

    dev->delta_t = 0;
    dev->ccw = false;
    dev->stale = false;
    dev->pulse_counter = 0;
    dev->last_read_time = ztimer_now(ZTIMER_USEC);

    if (gpio_init_int(dev->params.interrupt, GPIO_IN, GPIO_RISING, _pulse_callback, (void *) dev)) {
        LOG_ERROR("[hall_effect] Failed configuring the interrupt pin!\n");
        return -EIO;
    }

    return 0;
}

int hall_effect_read_rpm(hall_effect_t *dev, int32_t *rpm)
{
    uint32_t delta_t;
    bool ccw;
    if (!_read_delta_t_direction(dev, &delta_t, &ccw)) {
        *rpm = 0;
        return 0;
    }

    /* delta_t represents the number of micro seconds since the last pulse.
     * Invert and divide by the number of micro seconds per minute
     * to obtain the rpm. Apply scaling factors like gear reduction
     * or pulses per revolution.
     */
    *rpm = SEC_PER_MIN * US_PER_SEC * 10
           / (delta_t * CONFIG_HALL_EFFECT_PPR * CONFIG_HALL_EFFECT_GEAR_RED_RATIO);
    if (ccw) {
        *rpm *= -1;
    }
    return 0;
}

int hall_effect_read_reset_ceti_revs(hall_effect_t *dev, int32_t *pulse_counter)
{
    _read_reset_pulse_counter(dev, pulse_counter);
    *pulse_counter *= 1000;
    *pulse_counter /= CONFIG_HALL_EFFECT_PPR;
    *pulse_counter /= CONFIG_HALL_EFFECT_GEAR_RED_RATIO;
    return 0;
}

/* Private API */

/* Triggered on the high flank of a pulse */
static void _pulse_callback(void *arg)
{
    hall_effect_t *dev = (hall_effect_t *) arg;

    uint32_t now = ztimer_now(ZTIMER_USEC);

    /* Reading the shifted phase: low -> cw, high -> ccw */
    dev->ccw = gpio_read(dev->params.direction);
    if (now < dev->last_read_time) {
        /* timer had an overflow */
        dev->delta_t = UINT32_MAX - dev->last_read_time + now + 1;
    } else {
        dev->delta_t = now - dev->last_read_time;
    }
    dev->last_read_time = now;
    dev->pulse_counter += dev->ccw ? -1 : 1;
    /* data is no longer stale */
    dev->stale= false;
}

static void _read_reset_pulse_counter(hall_effect_t *dev, int32_t *pulse_counter)
{
    int irq_state = irq_disable();
    *pulse_counter = dev->pulse_counter;
    dev->pulse_counter = 0;
    irq_restore(irq_state);
}

static bool _read_delta_t_direction(hall_effect_t *dev, uint32_t *delta_t, bool *ccw)
{
    int irq_state = irq_disable();
    /* There have been no pulses for a while -> rotation probably stopped. */
    if (dev->stale) {
        irq_restore(irq_state);
        return false;
    }
    uint32_t now = ztimer_now(ZTIMER_USEC);
    uint32_t pulse_age;

    if (now < dev->last_read_time) {
        /* the timer had an overflow */
        pulse_age = UINT32_MAX - dev->last_read_time + now + 1;
    } else {
        pulse_age = now - dev->last_read_time;
    }
    if (pulse_age >= dev->delta_t) {
        /* Data is stale if the time elapsed since the last pulse
         * is longer than delta_t */
        *delta_t = pulse_age;
        dev->stale= true;
    } else {
        *delta_t = dev->delta_t;
    }
    *ccw = dev->ccw;

    irq_restore(irq_state);
    return true;
}
