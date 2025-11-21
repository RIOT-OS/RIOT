/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_inc_encoder
 * @{
 *
 * @file
 * @brief       Device driver implementation for a generic incremental rotary encoder
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 *
 * @}
 */

#include "inc_encoder.h"
#include "inc_encoder_params.h"
#include "inc_encoder_constants.h"

#include <errno.h>
#include "log.h"
#include "time_units.h"

/* If delta_t exceeds this threshold, the calculated RPM will be less than one
 * and will be truncated to zero.
 * When delta_t is larger than this threshold, we directly return zero
 * and prevent potential overflows in the RPM calculation.
 *
 * An overflow would occur when delta_t > INT32_MAX / (PPR * GEAR_RED_RATIO),
 * but our threshold is always lower than that because:
 *
 *     INT32_MAX > SEC_PER_MIN * US_PER_SEC * GEAR_RED_RATIO_SCALE
 */
#define DELTA_T_THRESHOLD ((SEC_PER_MIN * US_PER_SEC * GEAR_RED_RATIO_SCALE) \
                           / (CONFIG_INC_ENCODER_PPR * CONFIG_INC_ENCODER_GEAR_RED_RATIO))

/* Prototypes */
static void _pulse_cb(void *arg);
static bool _read_delta_t_direction(inc_encoder_t *dev, uint32_t *delta_t, bool *cw);

/* Public API */

int inc_encoder_init(inc_encoder_t *dev, const inc_encoder_params_t *params)
{
    dev->params = *params;
    if (gpio_init(dev->params.direction, GPIO_IN)) {
        LOG_ERROR("[inc_encoder] Failed configuring the direction pin as an input!\n");
        return -EIO;
    }

    dev->delta_t        = 0;
    dev->pulse_counter  = 0;
    dev->cw             = false;
    dev->stale          = true;
    dev->last_read_time = ztimer_now(ZTIMER_USEC);

    if (gpio_init_int(dev->params.interrupt, GPIO_IN, GPIO_RISING, _pulse_cb, (void *) dev)) {
        LOG_ERROR("[inc_encoder] Failed configuring the interrupt pin!\n");
        return -EIO;
    }

    return 0;
}

int inc_encoder_read_rpm(inc_encoder_t *dev, int32_t *rpm)
{
    uint32_t delta_t;
    bool cw;
    if (!_read_delta_t_direction(dev, &delta_t, &cw) || (delta_t >= DELTA_T_THRESHOLD)) {
        *rpm = 0;
        return 0;
    }

    /* delta_t represents the number of microseconds since the last pulse.
     * Invert and divide by the number of microseconds per minute
     * to obtain the RPM. Apply scaling factors like gear reduction
     * or pulses per revolution. */
    *rpm = SEC_PER_MIN * US_PER_SEC * GEAR_RED_RATIO_SCALE
           / (delta_t * CONFIG_INC_ENCODER_PPR * CONFIG_INC_ENCODER_GEAR_RED_RATIO);
    if (!cw) {
        *rpm *= -1;
    }

    return 0;
}

int inc_encoder_read_reset_milli_revs(inc_encoder_t *dev, int32_t *rev_counter)
{
    int irq_state = irq_disable();
    *rev_counter = dev->pulse_counter;
    dev->pulse_counter = 0;
    irq_restore(irq_state);

    *rev_counter *= UNIT_SCALE * GEAR_RED_RATIO_SCALE;
    *rev_counter /= CONFIG_INC_ENCODER_PPR;
    *rev_counter /= CONFIG_INC_ENCODER_GEAR_RED_RATIO;
    return 0;
}

/* Private API */

/* Triggered on the rising edge of a pulse */
static void _pulse_cb(void *arg)
{
    inc_encoder_t *dev = (inc_encoder_t *) arg;
    uint32_t now = ztimer_now(ZTIMER_USEC);

    /* Reading the shifted phase: high -> cw, low -> ccw */
    dev->cw = gpio_read(dev->params.direction);

    if (now < dev->last_read_time) {
        dev->delta_t = UINT32_MAX - dev->last_read_time + now + 1;
    }
    else {
        dev->delta_t = now - dev->last_read_time;
    }

    dev->last_read_time = now;
    dev->pulse_counter += dev->cw ? 1 : -1;
    /* data is no longer stale */
    dev->stale= false;
}

static bool _read_delta_t_direction(inc_encoder_t *dev, uint32_t *delta_t, bool *cw)
{
    uint32_t now;
    uint32_t pulse_age;
    int irq_state = irq_disable();

    if (dev->stale) {
        /* Rotation stopped */
        irq_restore(irq_state);
        return false;
    }
    now = ztimer_now(ZTIMER_USEC);

    /* Handle potential overflows */
    if (now < dev->last_read_time) {
        pulse_age = UINT32_MAX - dev->last_read_time + now + 1;
    }
    else {
        pulse_age = now - dev->last_read_time;
    }

    if (pulse_age >= dev->delta_t) {
        /* Data is stale if the time elapsed since the last pulse
         * is longer than delta_t */
        *delta_t = pulse_age;
        dev->stale= true;
    }
    else {
        *delta_t = dev->delta_t;
    }
    *cw = dev->cw;

    irq_restore(irq_state);
    return true;
}
