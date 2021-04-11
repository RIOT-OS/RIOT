/*
 * Copyright (C)    2018 Kaspar Schleiser <kaspar@schleiser.de>
                    2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer_periph_ptp
 * @{
 *
 * @file
 * @brief       ztimer periph/ptp implementation
 *
 * @author      Jana Eisoldt <jana.eisoldt@ovgu.de>
 *
 * @}
 */
#include "assert.h"

#include "irq.h"
#include "periph/ptp.h"
#include "ztimer/periph_ptp.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static ztimer_clock_t *clock_timer;

void ptp_timer_cb(void)
{
    ztimer_handler(clock_timer);
}

static void _ztimer_periph_ptp_set64(ztimer_clock_t *clock, uint64_t val)
{
    (void)clock;
    ptp_timer_set_u64(val);
}

static uint64_t _ztimer_periph_ptp_now64(ztimer_clock_t *clock)
{
    (void)clock;
    return ptp_clock_read_u64();
}

static void _ztimer_periph_ptp_cancel(ztimer_clock_t *clock)
{
    (void)clock;
    ptp_timer_clear();
}

static const ztimer_ops_t _ztimer_periph_ptp_ops = {
    .set64 = _ztimer_periph_ptp_set64,
    .now64 = _ztimer_periph_ptp_now64,
    .cancel = _ztimer_periph_ptp_cancel,
};

void ztimer_periph_ptp_init(ztimer_periph_ptp_t *clock)
{
    clock->ops = &_ztimer_periph_ptp_ops;
    clock_timer = clock;
}
