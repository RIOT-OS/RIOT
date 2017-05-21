/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_zynq7000
 * @{
 *
 * @file        hwtimer_arch.c
 * @brief       Implementation of the kernels hwtimer interface
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */
#include "hwtimer_cpu.h"
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
#if TIMER_3_EN == 1
void irq_handler3(int channel);
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
#if TIMER_3_EN == 1
    timer_init(TIMER_3, 1, &irq_handler3);
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
#if TIMER_3_EN == 1
    timer_irq_enable(TIMER_3);
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
#if TIMER_3_EN == 1
    timer_irq_disable(TIMER_3);
#endif
}

void hwtimer_arch_set(unsigned long offset, short timer)
{

#if TIMER_0_EN == 1
    if (timer < TIMER_0_CHANNELS) {
        timer_set(TIMER_0, timer, offset);
        return;
    }
#endif

#if TIMER_1_EN == 1
    if ((timer - PROXY_TIMER_0_CHANNELS) < TIMER_1_CHANNELS) {
        timer_set(TIMER_1, timer - PROXY_TIMER_0_CHANNELS, offset);
        return;
    }
#endif

#if TIMER_2_EN == 1
    if ((timer - PROXY_TIMER_0_CHANNELS - PROXY_TIMER_1_CHANNELS) < TIMER_2_CHANNELS) {
        timer_set(TIMER_2, timer - PROXY_TIMER_0_CHANNELS - PROXY_TIMER_1_CHANNELS, offset);
        return;
    }
#endif

#if TIMER_3_EN == 1
    if ((timer - PROXY_TIMER_0_CHANNELS - PROXY_TIMER_1_CHANNELS - PROXY_TIMER_2_CHANNELS) < TIMER_2_CHANNELS) {
        timer_set(TIMER_3, timer - PROXY_TIMER_0_CHANNELS - PROXY_TIMER_1_CHANNELS - PROXY_TIMER_2_CHANNELS, offset);
        return;
    }
#endif
}

void hwtimer_arch_set_absolute(unsigned long value, short timer) {
#if TIMER_0_EN == 1
    if (timer < TIMER_0_CHANNELS) {
        timer_set_absolute(TIMER_0, timer, value);
        return;
    }
#endif

#if TIMER_1_EN == 1
    if ((timer - PROXY_TIMER_0_CHANNELS) < TIMER_1_CHANNELS) {
        timer_set_absolute(TIMER_1, timer - PROXY_TIMER_0_CHANNELS, value);
        return;
    }
#endif

#if TIMER_2_EN == 1
    if ((timer - PROXY_TIMER_0_CHANNELS - PROXY_TIMER_1_CHANNELS) < TIMER_2_CHANNELS) {
        timer_set_absolute(TIMER_2, timer - PROXY_TIMER_0_CHANNELS - PROXY_TIMER_1_CHANNELS, value);
        return;
    }
#endif

#if TIMER_3_EN == 1
    if ((timer - PROXY_TIMER_0_CHANNELS - PROXY_TIMER_1_CHANNELS - PROXY_TIMER_2_CHANNELS) < TIMER_2_CHANNELS) {
        timer_set_absolute(TIMER_3, timer - PROXY_TIMER_0_CHANNELS - PROXY_TIMER_1_CHANNELS - PROXY_TIMER_2_CHANNELS, value);
        return;
    }
#endif
}

void hwtimer_arch_unset(short timer) {
#if TIMER_0_EN == 1
    if (timer < TIMER_0_CHANNELS) {
        timer_clear(TIMER_0, timer);
    }
#endif

#if TIMER_1_EN == 1
    if ((timer - PROXY_TIMER_0_CHANNELS) < TIMER_1_CHANNELS) {
        timer_clear(TIMER_1, timer - PROXY_TIMER_0_CHANNELS);
    }
#endif

#if TIMER_2_EN == 1
    if ((timer - PROXY_TIMER_0_CHANNELS - PROXY_TIMER_1_CHANNELS) < TIMER_2_CHANNELS) {
        timer_clear(TIMER_2, timer - PROXY_TIMER_0_CHANNELS - PROXY_TIMER_1_CHANNELS);
    }
#endif

#if TIMER_3_EN == 1
    if ((timer - PROXY_TIMER_0_CHANNELS - PROXY_TIMER_1_CHANNELS - PROXY_TIMER_2_CHANNELS) < TIMER_2_CHANNELS) {
        timer_clear(TIMER_3, timer - PROXY_TIMER_0_CHANNELS - PROXY_TIMER_1_CHANNELS - PROXY_TIMER_2_CHANNELS);
    }
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
#if TIMER_3_EN == 1
    return timer_read(TIMER_3);
#else
#error No timer available!
#endif
#endif
#endif
#endif
}

#if TIMER_0_EN == 1
void irq_handler0(int channel) {
    timeout_handler((short)(TIMER_0_OFFSET + channel));
}
#endif

#if TIMER_1_EN == 1
void irq_handler1(int channel) {
    timeout_handler((short)(TIMER_1_OFFSET + channel));
}
#endif

#if TIMER_2_EN == 1
void irq_handler2(int channel) {
    timeout_handler((short)(TIMER_2_OFFSET + channel));
}
#endif

#if TIMER_3_EN == 1
void irq_handler3(int channel) {
    timeout_handler((short)(TIMER_3_OFFSET + channel));
}
#endif
