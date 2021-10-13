/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Freie Universität Berlin
 *               2020 Inria
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
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "assert.h"
#include "irq.h"
#include "ztimer/periph_timer.h"

static void _ztimer_periph_timer_set(ztimer_clock_t *clock, uint32_t val)
{
    ztimer_periph_timer_t *ztimer_periph = (ztimer_periph_timer_t *)clock;

    uint16_t min = ztimer_periph->min;

    if (val < min) {
        val = min;
    }

/* if this is undefined, timer_set() from drivers/periph_timer_common is used.
 * That already dieables irq's.
 * For the others, better ensure that happens.
 */
#ifdef PERIPH_TIMER_PROVIDES_SET
    unsigned state = irq_disable();
#endif
    timer_set(ztimer_periph->dev, 0, val);
#ifdef PERIPH_TIMER_PROVIDES_SET
    irq_restore(state);
#endif
}

static uint32_t _ztimer_periph_timer_now(ztimer_clock_t *clock)
{
    ztimer_periph_timer_t *ztimer_periph = (ztimer_periph_timer_t *)clock;

    return timer_read(ztimer_periph->dev);
}

static void _ztimer_periph_timer_cancel(ztimer_clock_t *clock)
{
    ztimer_periph_timer_t *ztimer_periph = (ztimer_periph_timer_t *)clock;

    timer_clear(ztimer_periph->dev, 0);
}

static void _ztimer_periph_timer_callback(void *arg, int channel)
{
    (void)channel;
    ztimer_handler((ztimer_clock_t *)arg);
}

static const ztimer_ops_t _ztimer_periph_timer_ops = {
    .set = _ztimer_periph_timer_set,
    .now = _ztimer_periph_timer_now,
    .cancel = _ztimer_periph_timer_cancel,
};

void ztimer_periph_timer_init(ztimer_periph_timer_t *clock, tim_t dev,
                              uint32_t freq, uint32_t max_val)
{
    clock->dev = dev;
    clock->super.ops = &_ztimer_periph_timer_ops;
    clock->super.max_value = max_val;
    int ret = timer_init(dev, freq, _ztimer_periph_timer_callback, clock);

    (void)ret;
    assert(ret == 0);
    ztimer_init_extend(&clock->super);
}
