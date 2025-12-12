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

#include "log.h"
#include "ztimer.h"
#include "time_units.h"

#define MRPM_PER_RPM 1000LL

/* Prototypes */
static bool _rpm_calc_timer_cb(void *arg);
static void _acc_overflow_cb(void *args);

/* Public API */
int inc_encoder_init(inc_encoder_t *dev, const inc_encoder_params_t *params)
{
    dev->params = *params;

    if (qdec_init(dev->params.qdec_dev, QDEC_X4, _acc_overflow_cb, (void *) dev)) {
        LOG_ERROR("[inc_encoder] Qdec mode not supported!\n");
        return -EINVAL;
    }

    dev->extended_count = 0;
    dev->prev_count     = 0;
    dev->leftover_count = 0;
    dev->last_mrpm      = 0;

    /* Task to periodically calculate RPM */
    ztimer_periodic_init(ZTIMER_MSEC, &dev->rpm_timer, _rpm_calc_timer_cb, (void *) dev,
                         CONFIG_INC_ENCODER_HARDWARE_PERIOD_MS);

    ztimer_periodic_start(&dev->rpm_timer);

    return 0;
}

int inc_encoder_read_mrpm(inc_encoder_t *dev, int32_t *mrpm)
{
    int irq_state = irq_disable();
    *mrpm = dev->last_mrpm;
    irq_restore(irq_state);
    return 0;
}

int inc_encoder_read_reset_milli_revs(inc_encoder_t *dev, int32_t *rev_counter)
{
    int32_t total_count;
    int32_t delta_count;

    int irq_state = irq_disable();
    total_count = qdec_read_and_reset(dev->params.qdec_dev);
    total_count += dev->extended_count;
    delta_count = total_count - dev->prev_count;

    /* We reset the counter but we need to keep the number
     * of pulses since last read for the RPM calculation */
    dev->leftover_count += delta_count;
    dev->extended_count = 0;
    dev->prev_count = 0;
    irq_restore(irq_state);

    /* The 4X mode counts all falling and rising edges */
    *rev_counter =  (int32_t) total_count / 4;

    *rev_counter *= UNIT_SCALE * GEAR_RED_RATIO_SCALE;
    *rev_counter /= CONFIG_INC_ENCODER_PPR;
    *rev_counter /= CONFIG_INC_ENCODER_GEAR_RED_RATIO;
    return 0;
}

/* Private API */
static bool _rpm_calc_timer_cb(void *arg)
{
    inc_encoder_t *dev = (inc_encoder_t *) arg;
    int32_t delta_count;
    int32_t mrpm;
    int32_t total_count;

    total_count = dev->extended_count + qdec_read(dev->params.qdec_dev);
    delta_count = total_count - dev->prev_count;
    if (dev->leftover_count != 0) {
        /* Add leftover count from last reset */
        delta_count += dev->leftover_count;
        dev->leftover_count = 0;
    }
    dev->prev_count = total_count;

    mrpm = (MRPM_PER_RPM * SEC_PER_MIN * MS_PER_SEC * GEAR_RED_RATIO_SCALE * delta_count) /
          (CONFIG_INC_ENCODER_PPR * CONFIG_INC_ENCODER_GEAR_RED_RATIO
                    * CONFIG_INC_ENCODER_HARDWARE_PERIOD_MS * 4); /* 4X mode counts all edges */

    dev->last_mrpm = mrpm;
    return true;
}

static void _acc_overflow_cb(void *args)
{
    inc_encoder_t *dev = (inc_encoder_t *) args;

    dev->extended_count += qdec_read_and_reset(dev->params.qdec_dev);
}
