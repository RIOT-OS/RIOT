/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f1
 * @{
 *
 * @file
 * @brief       Implementation of the kernels hwtimer interface
 *
 * The hardware timer implementation uses the Coretex build-in system timer as backend.
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "arch/hwtimer_arch.h"
#include "thread.h"
#include "board.h"
#include "periph/timer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


void irq_handler(int channel);
void (*timeout_handler)(int);


void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    timeout_handler = handler;
    timer_init(HW_TIMER, 1, &irq_handler);
}

void hwtimer_arch_enable_interrupt(void)
{
    timer_irq_enable(HW_TIMER);
}

void hwtimer_arch_disable_interrupt(void)
{
    timer_irq_disable(HW_TIMER);
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    timer_set(HW_TIMER, timer, offset);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    timer_set_absolute(HW_TIMER, timer, value);
}

void hwtimer_arch_unset(short timer)
{
    timer_clear(HW_TIMER, timer);
}

unsigned long hwtimer_arch_now(void)
{
    return timer_read(HW_TIMER);
}

void irq_handler(int channel)
{
    timeout_handler((short)channel);
}
