/*
 * Copyright (C) 2021   Franz Freitag, Justus Krebs, Nick Weiler
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup sys_ztimer_periph_systick
 * @{
 *
 * @file
 * @brief       ztimer periph/systick implementation
 *
 * @author      Franz Freitag <franz.freitag@st.ovgu.de>
 * @author      Justus Krebs <justus.krebs@st.ovgu.de>
 * @author      Nick Weiler <nick.weiler@st.ovgu.de>
 *
 * @}
 */
#include "assert.h"
#include "irq.h"
#include "ztimer/periph_systick.h"
#include "debug.h"

#define ENABLE_DEBUG 0

#define ENABLE_MASK     (SysTick_CTRL_CLKSOURCE_Msk | \
                         SysTick_CTRL_ENABLE_Msk    | \
                         SysTick_CTRL_TICKINT_Msk)

/* full periods before alarm */
static uint16_t alarm_ext;

/* remaining time to alarm (last period length) */
static uint32_t alarm_val;

/* current time if the timer ran at freq Hz */
static uint32_t now;

static ztimer_clock_t *clock_timer;

void isr_systick(void)
{
    ztimer_handler(clock_timer);
}

static void _ztimer_periph_systick_set(ztimer_clock_t *clock, uint32_t val)
{
    (void)clock;
    unsigned state = irq_disable();

    /*Stop the systick to prevent race condition*/
    SysTick->CTRL = 0;

    /*val divided by 2^24 to guarantee a suitable size*/
    alarm_ext = val >> 24;
    alarm_val = val & SysTick_LOAD_RELOAD_Msk;

    /*Systick is counting backwards*/
    now += SysTick->LOAD - SysTick->VAL;

    /*If the value is greater than the maximum value the value is set to the maximum*/
    if (alarm_ext) {
        SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
    }
    else {
        SysTick->LOAD = alarm_val;
    }

    /* force reload */
    SysTick->VAL = 0;

    /* start the timer again */
    SysTick->CTRL = ENABLE_MASK;

    irq_restore(state);
}

static uint32_t _ztimer_periph_systick_now(ztimer_clock_t *clock)
{
    (void)clock;
    return (now + SysTick->LOAD - SysTick->VAL) & 0xFFFFFFU;
}

static void _ztimer_periph_systick_cancel(ztimer_clock_t *clock)
{
    (void)clock;
    SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
}

static const ztimer_ops_t _ztimer_periph_systick_ops = {
    .set = _ztimer_periph_systick_set,
    .now = _ztimer_periph_systick_now,
    .cancel = _ztimer_periph_systick_cancel,
};

void ztimer_periph_systick_init(ztimer_clock_t *clock)
{
    clock->ops = &_ztimer_periph_systick_ops;
    clock->max_value = 0xFFFFFFU;
    clock_timer = clock;

    SysTick->VAL = 0;
    SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
    SysTick->CTRL = ENABLE_MASK;

    NVIC_SetPriority(SysTick_IRQn, CPU_DEFAULT_IRQ_PRIO);
    NVIC_EnableIRQ(SysTick_IRQn);
}
