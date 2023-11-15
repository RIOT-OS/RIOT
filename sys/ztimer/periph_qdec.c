/*
 * Copyright (C) 2023 Prime Controls
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer_periph_timer
 * @{
 *
 * @file
 * @brief       ztimer periph/timer backend implementation
 *
 * @}
 */

#include "assert.h"
#include "irq.h"
#include "ztimer/periph_qdec.h"

static bool _qdec_cb(void *arg)
{
    ztimer_handler(arg);
    return false;
}

static void _set(ztimer_clock_t *clock, uint32_t val)
{
    ztimer_periph_qdec_t *ztimer_periph = (ztimer_periph_qdec_t *)clock;

    const unsigned state = irq_disable(); //TODO: needed?
    qdec_alarm_set(ztimer_periph->dev, 0, val, _qdec_cb, clock);
    irq_restore(state);
}

static uint32_t _now(ztimer_clock_t *clock)
{
    ztimer_periph_qdec_t *ztimer_periph = (ztimer_periph_qdec_t *)clock;

    return qdec_read(ztimer_periph->dev);
}

static void _cancel(ztimer_clock_t *clock)
{
    ztimer_periph_qdec_t *ztimer_periph = (ztimer_periph_qdec_t *)clock;

    qdec_alarm_clear(ztimer_periph->dev, 0);
}

#if MODULE_ZTIMER_ONDEMAND_TIMER
static void _start(ztimer_clock_t *clock)
{
    ztimer_periph_qdec_t *ztimer_periph = (ztimer_periph_qdec_t *)clock;

    qdec_start(ztimer_periph->dev);
}

static void _stop(ztimer_clock_t *clock)
{
    ztimer_periph_qdec_t *ztimer_periph = (ztimer_periph_qdec_t *)clock;

    qdec_stop(ztimer_periph->dev);
}
#endif /* MODULE_ZTIMER_ONDEMAND_TIMER */

static const ztimer_ops_t _ops = {
    .set = _set,
    .now = _now,
    .cancel = _cancel,
#if MODULE_ZTIMER_ONDEMAND_TIMER //TODO: MODULE_ZTIMER_ONDEMAND_QDEC?
    .start = _start,
    .stop = _stop,
#endif
};

void ztimer_periph_qdec_init(ztimer_periph_qdec_t *clock, qdec_t dev,
                             uint32_t max_val)
{
    clock->dev = dev;
    clock->super.ops = &_ops;
    clock->super.max_value = max_val;
    int ret = qdec_init(dev, QDEC_X4, NULL, NULL);

    (void)ret;
    assert(ret == 0);

#if !MODULE_ZTIMER_ONDEMAND
    /* extend lower clock only if the ondemand driver isn't selected
     * otherwise, the clock extension will be called with the first
     * ztimer_acquire() call */
    ztimer_init_extend(&clock->super);
#endif

#if MODULE_ZTIMER_ONDEMAND_TIMER //TODO: MODULE_ZTIMER_ONDEMAND_QDEC?
    /* turn off the timer peripheral after init
     * the first ztimer_acquire() call starts the peripheral */
    timer_stop(dev);
#endif
}
