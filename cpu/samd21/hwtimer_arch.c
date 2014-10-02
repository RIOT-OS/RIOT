/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 *
 * @file        hwtimer_arch.c
 * @brief       Implementation of the kernels hwtimer interface
 *
 * The hardware timer implementation uses the Cortex build-in system timer as back-end.
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "arch/hwtimer_arch.h"
#include "board.h"
#include "periph/timer.h"
#include "thread.h"

tim_t hw_timers[2];

void irq_handler0(int channel);
void irq_handler1(int channel);
void (*timeout_handler)(int);


void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    hw_timers[0] = HW_TIMER_0;
    hw_timers[1] = HW_TIMER_1;
    timeout_handler = handler;
    timer_init(hw_timers[0], 1, &irq_handler0);
    timer_init(hw_timers[1], 1, &irq_handler1);
}

void hwtimer_arch_enable_interrupt(void)
{
    timer_irq_enable(hw_timers[0]);
    timer_irq_enable(hw_timers[1]);
}

void hwtimer_arch_disable_interrupt(void)
{
    timer_irq_disable(hw_timers[0]);
    timer_irq_disable(hw_timers[1]);
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    timer_set(hw_timers[timer/2], (timer%2), offset);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    timer_set_absolute(hw_timers[timer/2], (timer%2), value);
}

void hwtimer_arch_unset(short timer)
{
    timer_clear(hw_timers[timer/2], (timer%2));
}

unsigned long hwtimer_arch_now(void)
{
    return timer_read(hw_timers[1]);
}

void irq_handler0(int channel)
{
    timeout_handler((short)(channel));
}

void irq_handler1(int channel)
{
    timeout_handler((short)(channel + 2));
}
