/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26x0
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation for the CC26x0
 *
 * @author      Leon M. George <leon@georgemail.eu>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"

/**
 * @brief   Allocate memory for the interrupt context
 */
static timer_isr_ctx_t ctx[TIMER_NUMOF];

/**
 * @brief           Get the GPT register base for a timer
 *
 * @param[in] tim   index of the timer
 *
 * @return          base address
 */
static inline gpt_reg_t *dev(tim_t tim)
{
    return timer_config[tim].dev;
}

int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    /* make sure given timer is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* enable the times clock */
    PRCM->GPTCLKGR |= (1 << timer_config[tim].num);
    PRCM->CLKLOADCTL = CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) {}

    /* disable (and reset) timer */
    dev(tim)->CTL = 0;

    /* save context */
    ctx[tim].cb = cb;
    ctx[tim].arg = arg;

    /* configure timer to 16-bit, periodic, up-counting */
    dev(tim)->CFG  = GPT_CFG_16T;
    dev(tim)->TAMR = (GPT_TXMR_TXMR_PERIODIC | GPT_TXMR_TXCDIR_UP | GPT_TXMR_TXMIE);

    /* set the timer speed */
    dev(tim)->TAPR = (RCOSC48M_FREQ / freq) - 1;
    /* enable global timer interrupt and start the timer */
    timer_irq_enable(tim);
    dev(tim)->CTL = GPT_CTL_TAEN;

    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    return timer_set_absolute(tim, channel, timer_read(tim) + timeout);
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if (channel != 0) {
        return -1;
    }

    dev(tim)->TAMATCHR = value;
    dev(tim)->IMR |= GPT_IMR_TAMIM;

    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    if (channel != 0) {
        return -1;
    }

    dev(tim)->IMR &= ~(GPT_IMR_TAMIM);

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    return dev(tim)->TAV & 0xFFFF;
}

void timer_stop(tim_t tim)
{
    dev(tim)->CTL = 0;
}

void timer_start(tim_t tim)
{
    dev(tim)->CTL = GPT_CTL_TAEN;
}

void timer_irq_enable(tim_t tim)
{
    NVIC_EnableIRQ(GPTIMER_0A_IRQN + (2 * timer_config[tim].num));
}

void timer_irq_disable(tim_t tim)
{
    NVIC_DisableIRQ(GPTIMER_0A_IRQN + (2 * timer_config[tim].num));
}

/**
 * @brief           handle interrupt for a timer
 *
 * @param[in] tim   index of the timer
 */
static inline void isr_handler(tim_t tim)
{
    uint32_t mis = dev(tim)->MIS;
    dev(tim)->ICLR = mis;

    if (mis & GPT_IMR_TAMIM) {
        dev(tim)->IMR &= ~GPT_IMR_TAMIM;
        ctx[tim].cb(ctx[tim].arg, 0);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef TIMER_0_ISR
void TIMER_0_ISR(void)
{
    isr_handler(0);
}
#endif

#ifdef TIMER_1_ISR
void TIMER_1_ISR(void)
{
    isr_handler(1);
}
#endif

#ifdef TIMER_2_ISR
void TIMER_2_ISR(void)
{
    isr_handler(2);
}
#endif

#ifdef TIMER_3_ISR
void TIMER_3_ISR(void)
{
    isr_handler(3);
}
#endif
