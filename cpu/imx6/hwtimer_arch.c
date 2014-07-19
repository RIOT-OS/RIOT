/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_imx6
 * @{
 *
 * @file        hwtimer_arch.c
 * @brief       Implementation of the kernels hwtimer interface
 *
 * The hardware timer implementation uses the Cortex build-in system timer as back-end.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Leon George <l.george@fu-berlin.de>
 *
 * @}
 */

#include "arch/hwtimer_arch.h"
#include "board.h"
#include "periph/timer.h"
#include "thread.h"
#include "periph_conf.h"


#if TIMER_0_EN == 1
void irq_handler0(int channel);
#endif
#if TIMER_1_EN == 1
void irq_handler1(int channel);
#endif
#if TIMER_2_EN == 1
void irq_handler2(int channel);
#endif
void (*timeout_handler)(int);


void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    timeout_handler = handler;
#if TIMER_0_EN == 1
    timer_init(TIMER_0, 1, &irq_handler0);
#endif
#if TIMER_1_EN == 1
    timer_init(TIMER_1, 1, &irq_handler1);
#endif
#if TIMER_2_EN == 1
    timer_init(TIMER_2, 1, &irq_handler2);
#endif
}

void hwtimer_arch_enable_interrupt(void)
{
#if TIMER_0_EN == 1
    timer_irq_enable(TIMER_0);
#endif
#if TIMER_1_EN == 1
    timer_irq_enable(TIMER_1);
#endif
#if TIMER_2_EN == 1
    timer_irq_enable(TIMER_2);
#endif
}

void hwtimer_arch_disable_interrupt(void)
{
#if TIMER_0_EN == 1
    timer_irq_disable(TIMER_0);
#endif
#if TIMER_1_EN == 1
    timer_irq_disable(TIMER_1);
#endif
#if TIMER_2_EN == 1
    timer_irq_disable(TIMER_2);
#endif
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
#if TIMER_0_EN == 1

    if (timer < TIMER_0_CHANNELS) {
        timer_set(TIMER_0, timer, offset);
    }

#if TIMER_1_EN == 1
    else if ((timer - TIMER_0_CHANNELS) < TIMER_1_CHANNELS) {
        timer_set(TIMER_1, timer - TIMER_0_CHANNELS, offset);
    }

#if TIMER_2_EN == 1
    else if ((timer - TIMER_0_CHANNELS - TIMER_1_CHANNELS) < TIMER_2_CHANNELS) {
        timer_set(TIMER_2, timer - TIMER_0_CHANNELS - TIMER_1_CHANNELS, offset);
    }

#endif
#else
#if TIMER_2_EN == 1
    else if ((timer - TIMER_0_CHANNELS) < TIMER_2_CHANNELS) {
        timer_set(TIMER_2, timer - TIMER_0_CHANNELS, offset);
    }

#endif
#endif
#else
#if TIMER_1_EN == 1

    if (timer < TIMER_1_CHANNELS) {
        timer_set(TIMER_1, timer, offset);
    }

#if TIMER_2_EN == 1
    else if ((timer - TIMER_1_CHANNELS) < TIMER_2_CHANNELS) {
        timer_set(TIMER_2, timer - TIMER_1_CHANNELS, offset);
    }

#endif
#else
#if TIMER_2_EN == 1

    if (timer < TIMER_2_CHANNELS) {
        timer_set(TIMER_2, timer, offset);
#endif
#endif
#endif
}

void hwtimer_arch_set_absolute(unsigned long value, short timer) {
#if TIMER_0_EN == 1

    if (timer < TIMER_0_CHANNELS) {
        timer_set_absolute(TIMER_0, timer, value);
    }

#if TIMER_1_EN == 1
    else if ((timer - TIMER_0_CHANNELS) < TIMER_1_CHANNELS) {
        timer_set_absolute(TIMER_1, timer - TIMER_0_CHANNELS, value);
    }

#if TIMER_2_EN == 1
    else if ((timer - TIMER_0_CHANNELS - TIMER_1_CHANNELS) < TIMER_2_CHANNELS) {
        timer_set_absolute(TIMER_2, timer - TIMER_0_CHANNELS - TIMER_1_CHANNELS, value);
    }

#endif
#else
#if TIMER_2_EN == 1
    else if ((timer - TIMER_0_CHANNELS) < TIMER_2_CHANNELS) {
        timer_set_absolute(TIMER_2, timer - TIMER_0_CHANNELS, value);
    }

#endif
#endif
#else
#if TIMER_1_EN == 1

    if (timer < TIMER_1_CHANNELS) {
        timer_set_absolute(TIMER_1, timer, value);
    }

#if TIMER_2_EN == 1
    else if ((timer - TIMER_1_CHANNELS) < TIMER_2_CHANNELS) {
        timer_set_absolute(TIMER_2, timer - TIMER_1_CHANNELS, value);
    }

#endif
#else
#if TIMER_2_EN == 1

    if (timer < TIMER_2_CHANNELS) {
        timer_set_absolute(TIMER_2, timer, value);
    }

#endif
#endif
#endif
}

void hwtimer_arch_unset(short timer) {
#if TIMER_0_EN == 1

    if (timer < TIMER_0_CHANNELS) {
        timer_clear(TIMER_0, timer);
    }

#if TIMER_1_EN == 1
    else if ((timer - TIMER_0_CHANNELS) < TIMER_1_CHANNELS) {
        timer_clear(TIMER_1, timer - TIMER_0_CHANNELS);
    }

#if TIMER_2_EN == 1
    else if ((timer - TIMER_0_CHANNELS - TIMER_1_CHANNELS) < TIMER_2_CHANNELS) {
        timer_clear(TIMER_2, timer - TIMER_0_CHANNELS - TIMER_1_CHANNELS);
    }

#endif
#else
#if TIMER_2_EN == 1
    else if ((timer - TIMER_0_CHANNELS) < TIMER_2_CHANNELS) {
        timer_clear(TIMER_2, timer - TIMER_0_CHANNELS);
    }

#endif
#endif
#else
#if TIMER_1_EN == 1

    if (timer < TIMER_1_CHANNELS) {
        timer_clear(TIMER_1, timer);
    }

#if TIMER_2_EN == 1
    else if ((timer - TIMER_1_CHANNELS) < TIMER_2_CHANNELS) {
        timer_clear(TIMER_2, timer - TIMER_1_CHANNELS);
    }

#endif
#else
#if TIMER_2_EN == 1

    if (timer < TIMER_2_CHANNELS) {
        timer_clear(TIMER_2, timer);
    }

#endif
#endif
#endif
}

unsigned long hwtimer_arch_now(void) {
#if TIMER_0_EN == 1
    return timer_read(TIMER_0);
#else
#if TIMER_1_EN == 1
    return timer_read(TIMER_1);
#else
#if TIMER_2_EN == 1
    return timer_read(TIMER_2);
#else
#error No timer available!
#endif
#endif
#endif
}

#if TIMER_0_EN == 1
void irq_handler0(int channel) {
    timeout_handler((short)(TIMER_0_OFFSET + channel));

    thread_yield();
}
#endif

#if TIMER_1_EN == 1
void irq_handler1(int channel) {
    timeout_handler((short)(TIMER_1_OFFSET + channel));

    thread_yield();
}
#endif

#if TIMER_2_EN == 1
void irq_handler2(int channel) {
    timeout_handler((short)(TIMER_2_OFFSET + channel));

    thread_yield();
}
#endif
