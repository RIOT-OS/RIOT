/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_countdown
 * @{
 *
 * @file        systick.c
 * @brief       Low-level countdown timer implementation based on SysTick
 *
 *              SysTick is a 24-bit Down-Counting Timer
 *              It runs at the same frequency as the CPU
 *              The count (VAL) register can only be set to 0 on write
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <assert.h>
#include "board.h"
#include "periph_conf.h"
#include "timex.h"
#include "periph/countdown.h"

/* SysTick CTRL configuration */
#define ENABLE_MASK     (SysTick_CTRL_CLKSOURCE_Msk |\
                         SysTick_CTRL_ENABLE_Msk    |\
                         SysTick_CTRL_TICKINT_Msk)

#define TICKS_PER_US    (CLOCK_CORECLOCK / US_PER_SEC)

static bool one_shot;
static countdown_cb_t systick_cb;
static void *systick_cb_arg;

static uint32_t _init(const void *ctx, uint32_t max_range, unsigned flags, uint32_t *mask)
{
    (void)ctx;
    (void)max_range;
    (void)flags;

    NVIC_SetPriority(SysTick_IRQn, CPU_DEFAULT_IRQ_PRIO);
    NVIC_EnableIRQ(SysTick_IRQn);

    *mask = SysTick_LOAD_RELOAD_Msk;

    return CLOCK_CORECLOCK;
}

static void _set_cb(const void *ctx, countdown_cb_t cb, void *arg)
{
    (void)ctx;

    systick_cb = cb;
    systick_cb_arg = arg;
}

static void _get_cb(const void *ctx, countdown_cb_t *cb, void **arg)
{
    (void)ctx;

    *cb = systick_cb;
    *arg = systick_cb_arg;
}

static void _set(const void *ctx, uint32_t alarm, bool once)
{
    (void)ctx;

    /* stop timer to prevent race condition */
    SysTick->CTRL = 0;

    /* set alarm value */
    SysTick->LOAD = alarm;

    one_shot = once;
}

static void _start(const void *ctx)
{
    (void)ctx;

    /* force reload */
    SysTick->VAL = 0;

    /* start the timer again */
    SysTick->CTRL = ENABLE_MASK;
}

static void _cancel(const void *ctx)
{
    (void)ctx;

    SysTick->CTRL = 0;
}

void isr_systick(void)
{
    /* no further alarms */
    if (one_shot) {
        SysTick->CTRL = 0;
    }

    systick_cb(systick_cb_arg);

    cortexm_isr_end();
}

static const countdown_desc_t _driver_systick = {
    .init   = _init,
    .set_cb = _set_cb,
    .get_cb = _get_cb,
    .set    = _set,
    .start  = _start,
    .cancel = _cancel,
};

XFA_CONST(countdown_timers_xfa, 0)
const countdown_dev_t countdown_systick = {
    .driver = &_driver_systick,
};

XFA(countdown_cfg_xfa, 0)
countdown_cfg_t countdown_cfg_systick;
