/*
 * SPDX-FileCopyrightText: 2018 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
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

static void _ztimer_periph_ptp_set(ztimer_clock_t *clock, uint32_t val)
{
    (void)clock;
    ptp_timer_set_u64(val);
}

static uint32_t _ztimer_periph_ptp_now(ztimer_clock_t *clock)
{
    (void)clock;
    return (uint32_t)ptp_clock_read_u64();
}

static void _ztimer_periph_ptp_cancel(ztimer_clock_t *clock)
{
    (void)clock;
    ptp_timer_clear();
}

static const ztimer_ops_t _ztimer_periph_ptp_ops = {
    .set = _ztimer_periph_ptp_set,
    .now = _ztimer_periph_ptp_now,
    .cancel = _ztimer_periph_ptp_cancel,
};

void ztimer_periph_ptp_init(ztimer_periph_ptp_t *clock)
{
    clock->ops = &_ztimer_periph_ptp_ops;
    clock->max_value = UINT32_MAX;
    clock_timer = clock;

    ztimer_init_extend(clock);
}
