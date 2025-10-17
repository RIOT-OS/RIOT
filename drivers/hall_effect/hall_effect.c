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

static void _pulse_callback(void *arg);
static void read_reset_pulse_counter(hall_effect_t *dev, int32_t *pulse_counter);
static bool _read_delta_t_direction(hall_effect_t *dev, uint32_t *delta_t, bool *clock_wise);

/* Public API */

int hall_effect_init(hall_effect_t *dev, const hall_effect_params_t *params)
{
    dev->params = *params;

    if (gpio_init(dev->params.direction, GPIO_IN)) {
        LOG_ERROR("[hall_effect] Failed configuring the direction pin as an input!\n");
        return -EIO;
    }

    dev->delta_t = 0;
    dev->clock_wise = false;
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
    bool clock_wise;
    if(!_read_delta_t_direction(dev, &delta_t, &clock_wise)) {
        *rpm = 0;
        return 0;
    }

    /* delta_t represents the number of micro seconds since last readout.
     * Invert and devide by the number of micro seconds per minute
     * to obtain the rpm. Apply scaling factors like gear reduction
     * or pulses per rotation.
     */
    *rpm = SEC_PER_MIN * US_PER_SEC * 10
           / (delta_t * CONFIG_HALL_EFFECT_PPR * CONFIG_HALL_EFFECT_GEAR_RED_RATIO);
    if (!clock_wise) {
        *rpm *= -1;
    }
    return 0;
}

int hall_effect_read_reset_revolutions_hundreths(hall_effect_t *dev, int32_t *pulse_counter)
{
    read_reset_pulse_counter(dev, pulse_counter);
    *pulse_counter *= 1000;
    *pulse_counter /= CONFIG_HALL_EFFECT_PPR;
    *pulse_counter /= CONFIG_HALL_EFFECT_GEAR_RED_RATIO;
    return 0;
}

/* Private API */

static void _pulse_callback(void *arg)
{
    hall_effect_t *dev = (hall_effect_t *) arg;

    uint32_t now = ztimer_now(ZTIMER_USEC);

    dev->clock_wise = gpio_read(dev->params.direction);
    dev->delta_t = now - dev->last_read_time;
    dev->last_read_time = now;
    dev->pulse_counter += dev->clock_wise ? 1 : -1;
    dev->stale= false;
}

static void read_reset_pulse_counter(hall_effect_t *dev, int32_t *pulse_counter)
{
    int irq_state = irq_disable();
    *pulse_counter = dev->pulse_counter;
    dev->pulse_counter = 0;
    irq_restore(irq_state);
}

static bool _read_delta_t_direction(hall_effect_t *dev, uint32_t *delta_t, bool *clock_wise)
{
    int irq_state = irq_disable();
    /* If the last data we read is stale we can just return false*/
    if (dev->stale) {
        irq_restore(irq_state);
        return false;
    }
    uint32_t now = ztimer_now(ZTIMER_USEC);
    if (now - dev->last_read_time >= dev->delta_t) {
        /* we have been waiting for an update longer than the last delta t
         * the rotation stopped we canmark the data as stale for optimizations */
        *delta_t = now - dev->last_read_time;
        dev->stale= true;
    } else {
        *delta_t = dev->delta_t;
    }
    *clock_wise = dev->clock_wise;
    irq_restore(irq_state);
    return true;
}