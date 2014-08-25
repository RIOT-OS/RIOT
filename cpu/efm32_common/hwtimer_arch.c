/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_efm32
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's hwtimer interface
 *
 * The hardware timer implementation uses the Cortex build-in system timer as back-end.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Ryan Kurte <ryankurte@gmail.com>
 *
 * @}
 */

#include "arch/hwtimer_arch.h"
#include "periph/timer.h"
#include "board.h"
#include "thread.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

void irq_handler(int channel);
void (*timeout_handler)(int);


void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    timeout_handler = handler;
    DEBUG("hwtimer_arch_init(): initialising\n");
    timer_init(HW_TIMER, 3, &irq_handler);
}

void hwtimer_arch_enable_interrupt(void)
{
    DEBUG("hwtimer_arch_enable_interrupt()\n");
    timer_irq_enable(HW_TIMER);
}

void hwtimer_arch_disable_interrupt(void)
{
    DEBUG("hwtimer_arch_disable_interrupt()\n");
    timer_irq_disable(HW_TIMER);
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    DEBUG("hwtimer_arch_set() set timer %d to %lu\n", timer, offset);
    timer_set(HW_TIMER, timer, offset);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    DEBUG("hwtimer_arch_set_absolute(): set timer %d to %lu\n", timer, value);
    timer_set_absolute(HW_TIMER, timer, value);
}

void hwtimer_arch_unset(short timer)
{
    DEBUG("hwtimer_arch_unset(): timer %d\n", timer);
    timer_clear(HW_TIMER, timer);
}

unsigned long hwtimer_arch_now(void)
{
    unsigned long time_now = timer_read(HW_TIMER);
    DEBUG("hwtimer_arch_now(): time is %lu\n", time_now);
    return time_now;
}

void irq_handler(int channel)
{
    DEBUG("hwtimer channel %d irq\n", channel);
    timeout_handler((short)(channel));
    thread_yield();
}
