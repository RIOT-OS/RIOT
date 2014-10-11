/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 volatiles UG (haftungsbeschränkt)
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_tm4c123
 * @{
 *
 * @file
 * @brief       Implementation of the kernels hwtimer interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Benjamin Valentin <benjamin.valentin@volatiles.de>
 *
 * @}
 */

#include "arch/hwtimer_arch.h"
#include "hwtimer_cpu.h"
#include "board.h"
#include "periph/timer.h"
#include "thread.h"

static inline tim_t get_timer(short timer) {
    switch (timer) {
        case 0:
        return TIMER_0;
        case 1:
        return TIMER_1;
        case 2:
        return TIMER_2;
        case 3:
        return TIMER_3;
        case 4:
        return TIMER_4;
        case 5:
        return TIMER_5;
        default:
        return TIMER_UNDEFINED;
    }
}
/**
 * @brief Callback function that is given to the low-level timer
 *
 * @param[in] channel   the channel of the low-level timer that was triggered
 */
void irq_handler(int channel);

/**
 * @brief Hold a reference to the hwtimer callback
 */
void (*timeout_handler)(int);

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    unsigned int ticks_per_us = 1;
    timeout_handler = handler;

    for (int i = 0; i < HWTIMER_MAXTIMERS; ++i)
        timer_init(get_timer(i), ticks_per_us, &irq_handler);
}

void hwtimer_arch_enable_interrupt(void)
{
	// This function is never useds
}

void hwtimer_arch_disable_interrupt(void)
{
	// This function is never useds
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    timer_set(get_timer(timer), 0, offset);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    timer_set_absolute(get_timer(timer), 0, value);
}

void hwtimer_arch_unset(short timer)
{
    timer_clear(get_timer(timer), 0);
}

unsigned long hwtimer_arch_now(void)
{
    return timer_read(TIMER_0);
}

void irq_handler(int channel)
{
    timeout_handler((short)(channel));
}
