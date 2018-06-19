/*
 * Copyright (C) 2017 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_pulse_counter
 * @{
 *
 * @file
 * @brief       Driver for the PULSE COUNTER.
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include <string.h>

#include "pulse_counter_params.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* Accumulate pulse count */
static void pulse_counter_trigger(void *arg)
{
    pulse_counter_t *dev = (pulse_counter_t *)arg;

    /* Use atomic operations to avoid messing with IRQ flags */
    __atomic_fetch_add(&(dev->pulse_count), 1, __ATOMIC_SEQ_CST);
}

/* Initialize pulse counter */
int pulse_counter_init(pulse_counter_t *dev, const pulse_counter_params_t *params)
{
    gpio_mode_t gpio_mode;
    if (params->gpio_flank == GPIO_FALLING) {
        gpio_mode = GPIO_IN_PU;
    }
    else {
        gpio_mode = GPIO_IN_PD;
    }

    if (gpio_init_int(params->gpio, gpio_mode, params->gpio_flank, pulse_counter_trigger, dev)) {
        return -1;
    }

    dev->pulse_count = 0;
    return 0;
}

/* Return the accumulated pulse counts and reset the count to zero */
int16_t pulse_counter_read_with_reset(const void *dev)
{
    int16_t pulse_count_output = 0;
    int16_t reset_value = 0;

    /* Use atomic operations to avoid messing with IRQ flags */
    __atomic_exchange(&(((pulse_counter_t *)dev)->pulse_count), &reset_value, &pulse_count_output, __ATOMIC_SEQ_CST);
    return pulse_count_output;
}

/* Return the accumulated pulse counts */
int16_t pulse_counter_read_without_reset(const void *dev)
{
    return ((pulse_counter_t *)dev)->pulse_count;
}

/* Reset the pulse count value to zero */
void pulse_counter_reset(const void *dev)
{
    ((pulse_counter_t *)dev)->pulse_count = 0;
}
