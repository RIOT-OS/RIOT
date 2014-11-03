/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file        hwtimer_arch.c
 * @brief       Implementation of the kernels hwtimer interface
 *
 * The hardware timer implementation uses the CC2538 Sleep Timer as a back-end.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include "arch/hwtimer_arch.h"
#include "hwtimer.h"
#include "hwtimer_cpu.h"
#include "periph_conf.h"

#include "sched.h"
#include "thread.h"

#define sleep_timer_ready() ( SMWDTHROSC_STLOAD & 1 )

void (*timeout_handler)(int);

static inline void sleep_timer_load_compare_value(uint32_t value) {
    /* Wait until the compare registers are ready to be loaded: */
    while (!sleep_timer_ready());

    /* Load the three most-significant bytes: */
    SMWDTHROSC_ST3 = value >> 24;
    SMWDTHROSC_ST2 = value >> 16;
    SMWDTHROSC_ST1 = value >>  8;

    /* Load the least-significant byte, which also latches the other bytes: */
    SMWDTHROSC_ST0 = value;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    timeout_handler = handler;
    NVIC_SetPriority(SM_TIMER_ALT_IRQn, HWTIMER_IRQ_PRIO);
}

void hwtimer_arch_enable_interrupt(void)
{
    NVIC_EnableIRQ(SM_TIMER_ALT_IRQn);
}

void hwtimer_arch_disable_interrupt(void)
{
    NVIC_DisableIRQ(SM_TIMER_ALT_IRQn);
}

unsigned long hwtimer_arch_now(void)
{
    /* Read the least-significant byte, which also latches the other bytes: */
    unsigned long now = SMWDTHROSC_ST0;

    /* Read the remaining more-significant bytes: */
    now |= SMWDTHROSC_ST1 <<  8;
    now |= SMWDTHROSC_ST2 << 16;
    now |= SMWDTHROSC_ST3 << 24;

    return now;
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    unsigned long offset;

    offset = value - hwtimer_arch_now();

    if (offset < HWTIMER_SPIN_BARRIER) {
        hwtimer_spin(offset);
        timeout_handler(timer);
    }
    else {
        sleep_timer_load_compare_value(value);
        NVIC_EnableIRQ(SM_TIMER_ALT_IRQn);
    }
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    if (offset < HWTIMER_SPIN_BARRIER) {
        hwtimer_spin(offset);
        timeout_handler(timer);
    }
    else {
        sleep_timer_load_compare_value(hwtimer_arch_now() + offset);
        NVIC_EnableIRQ(SM_TIMER_ALT_IRQn);
    }
}

void hwtimer_arch_unset(short timer)
{
    NVIC_DisableIRQ(SM_TIMER_ALT_IRQn);
}

void isr_sleepmode(void) {
    timeout_handler(0);

    if (sched_context_switch_request) {
        thread_yield();
    }
}
