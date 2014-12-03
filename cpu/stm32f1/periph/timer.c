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
 * @file        timer.c
 * @brief       Low-level timer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>

#include "cpu.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/timer.h"

#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* guard file in case no TIMER device is defined */
#if TIMER_0_EN || TIMER_1_EN

static inline void irq_handler(tim_t timer, TIM_TypeDef *dev0, TIM_TypeDef *dev1);

typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * Timer state memory
 */
static timer_conf_t config[TIMER_NUMOF];

int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    TIM_TypeDef *timer0;
    TIM_TypeDef *timer1;
    uint8_t     trigger_selector;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            /* enable timer peripheral clock */
            TIMER_0_CLKEN();
            /* set timer's IRQ priority */
            NVIC_SetPriority(TIMER_0_IRQ_CHAN_0, TIMER_0_IRQ_PRIO);
            NVIC_SetPriority(TIMER_0_IRQ_CHAN_1, TIMER_0_IRQ_PRIO);
            /* select timer */
            timer0 = TIMER_0_DEV_0;
            timer1 = TIMER_0_DEV_1;
            trigger_selector = TIMER_0_TRIG_SEL;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            /* enable timer peripheral clock */
            TIMER_1_CLKEN();
            /* set timer's IRQ priority */
            NVIC_SetPriority(TIMER_1_IRQ_CHAN_0, TIMER_1_IRQ_PRIO);
            NVIC_SetPriority(TIMER_1_IRQ_CHAN_1, TIMER_1_IRQ_PRIO);
            /* select timer */
            timer0 = TIMER_1_DEV_0;
            timer1 = TIMER_1_DEV_1;
            trigger_selector = TIMER_1_TRIG_SEL;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    /* set callback function */
    config[dev].cb = callback;

    /* set timer to run in counter mode */
    timer0->CR1 = (TIM_CR1_ARPE | TIM_CR1_URS);
    timer1->CR1 = TIM_CR1_URS;

    /* configure master timer0 */
    /* send update event as trigger output */
    timer0->CR2 |= TIM_CR2_MMS_1;
    /* set auto-reload and prescaler values and load new values */
    timer0->ARR = TIMER_0_MAX_VALUE;
    timer0->PSC = TIMER_0_PRESCALER * ticks_per_us;
    // timer->EGR |= TIM_EGR_UG;

    /* configure slave timer1 */
    /* get input trigger */
    timer1->SMCR |= trigger_selector;
    /* external clock mode 1 */
    timer1->SMCR |= TIM_SMCR_SMS;

    /* enable the timer's interrupt */
    timer_irq_enable(dev);

    /* start the timer */
    timer_start(dev);

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    int now = timer_read(dev);
    return timer_set_absolute(dev, channel, now + timeout - 1);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    TIM_TypeDef *timer0;
    TIM_TypeDef *timer1;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            /* select timer */
            timer0 = TIMER_0_DEV_0;
            timer1 = TIMER_0_DEV_1;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            /* select timer */
            timer0 = TIMER_1_DEV_0;
            timer1 = TIMER_1_DEV_1;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    timer0->DIER &= ~TIM_DIER_UIE;

    switch (channel) {
        case 0:
            timer0->CCR1  = (0xffff & value);
            timer1->CCR1  = (value >> 16);
            timer0->SR   &= ~TIM_SR_CC1IF;
            timer0->DIER |= TIM_DIER_CC1IE;
            DEBUG("Channel 1 set to %x\n", value);
            break;
        case 1:
            timer0->CCR2  = (0xffff & value);
            timer1->CCR2  = (value >> 16);
            timer0->SR   &= ~TIM_SR_CC2IF;
            timer0->DIER |= TIM_DIER_CC2IE;
            DEBUG("Channel 2 set to %x\n", value);
            break;
        case 2:
            timer0->CCR3  = (0xffff & value);
            timer1->CCR3  = (value >> 16);
            timer0->SR   &= ~TIM_SR_CC3IF;
            timer0->DIER |= TIM_DIER_CC3IE;
            DEBUG("Channel 3 set to %x\n", value);
            break;
        case 3:
            timer0->CCR4  = (0xffff & value);
            timer1->CCR4  = (value >> 16);
            timer0->SR   &= ~TIM_SR_CC4IF;
            timer0->DIER |= TIM_DIER_CC4IE;
            DEBUG("Channel 4 set to %x\n", value);
            break;
        default:
            return -1;
    }
    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    TIM_TypeDef *timer0;
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            timer0 = TIMER_0_DEV_0;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            timer0 = TIMER_1_DEV_0;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }
    switch (channel) {
        case 0:
            timer0->DIER &= ~TIM_DIER_CC1IE;
            break;
        case 1:
            timer0->DIER &= ~TIM_DIER_CC2IE;
            break;
        case 2:
            timer0->DIER &= ~TIM_DIER_CC3IE;
            break;
        case 3:
            timer0->DIER &= ~TIM_DIER_CC4IE;
            break;
        default:
            return -1;
    }
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            return (((unsigned int)(0xffff & TIMER_0_DEV_0->CNT)) | (TIMER_0_DEV_1->CNT<<16));
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            return (((unsigned int)(0xffff & TIMER_1_DEV_0->CNT)) | (TIMER_1_DEV_1->CNT<<16));
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return 0;
    }
}

void timer_start(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            /* slave has to be enabled first */
            TIMER_0_DEV_1->CR1 |= TIM_CR1_CEN;
            TIMER_0_DEV_0->CR1 |= TIM_CR1_CEN;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            /* slave has to be enabled first */
            TIMER_1_DEV_1->CR1 |= TIM_CR1_CEN;
            TIMER_1_DEV_0->CR1 |= TIM_CR1_CEN;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_stop(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV_0->CR1 &= ~TIM_CR1_CEN;
            TIMER_0_DEV_1->CR1 &= ~TIM_CR1_CEN;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV_0->CR1 &= ~TIM_CR1_CEN;
            TIMER_1_DEV_1->CR1 &= ~TIM_CR1_CEN;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_enable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            NVIC_EnableIRQ(TIMER_0_IRQ_CHAN_0);
            NVIC_EnableIRQ(TIMER_0_IRQ_CHAN_1);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_EnableIRQ(TIMER_1_IRQ_CHAN_0);
            NVIC_EnableIRQ(TIMER_1_IRQ_CHAN_1);
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_disable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            NVIC_DisableIRQ(TIMER_0_IRQ_CHAN_0);
            NVIC_DisableIRQ(TIMER_0_IRQ_CHAN_1);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_DisableIRQ(TIMER_1_IRQ_CHAN_0);
            NVIC_DisableIRQ(TIMER_1_IRQ_CHAN_1);
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_reset(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV_0->CNT = 0;
            TIMER_0_DEV_1->CNT = 0;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV_0->CNT = 0;
            TIMER_1_DEV_1->CNT = 0;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}


#if TIMER_0_EN
void TIMER_0_ISR_0(void)
{
    DEBUG("\nenter ISR\n");
    irq_handler(TIMER_0, TIMER_0_DEV_0, TIMER_0_DEV_1);
    DEBUG("leave ISR\n\n");
}
#endif

#if TIMER_1_EN
void TIMER_1_ISR_0(void)
{
    irq_handler(TIMER_1, TIMER_1_DEV_0, TIMER_1_DEV_1);
}
#endif

static inline void irq_handler(tim_t timer, TIM_TypeDef *dev0, TIM_TypeDef *dev1)
{
    DEBUG("CNT: %08x SR/DIER: %08x\n", ((dev1->CNT<<16) | (0xffff & dev0->CNT)),
                                       ((dev0->SR<<16) | (0xffff & dev0->DIER)));

    if ((dev0->SR & TIM_SR_CC1IF) && (dev0->DIER & TIM_DIER_CC1IE)) {
        /* clear interrupt anyway */
        dev0->SR &= ~TIM_SR_CC1IF;
        /* if higher 16bit also match */
        if (dev1->CNT >= dev1->CCR1) {
            dev0->DIER &= ~TIM_DIER_CC1IE;
            config[timer].cb(0);
        }
        DEBUG("channel 1 CCR: %08x\n", ((dev1->CCR1<<16) | (0xffff & dev0->CCR1)));
    }
    else if ((dev0->SR & TIM_SR_CC2IF) && (dev0->DIER & TIM_DIER_CC2IE)) {
        /* clear interrupt anyway */
        dev0->SR &= ~TIM_SR_CC2IF;
        /* if higher 16bit also match */
        if (dev1->CNT >= dev1->CCR2) {
            dev0->DIER &= ~TIM_DIER_CC2IE;
            config[timer].cb(1);
        }
        DEBUG("channel 2 CCR: %08x\n", ((dev1->CCR2<<16) | (0xffff & dev0->CCR2)));
    }
    else if ((dev0->SR & TIM_SR_CC3IF) && (dev0->DIER & TIM_DIER_CC3IE)) {
        /* clear interrupt anyway */
        dev0->SR &= ~TIM_SR_CC3IF;
        /* if higher 16bit also match */
        if (dev1->CNT >= dev1->CCR3) {
            dev0->DIER &= ~TIM_DIER_CC3IE;
            config[timer].cb(2);
        }
        DEBUG("channel 3 CCR: %08x\n", ((dev1->CCR3<<16) | (0xffff & dev0->CCR3)));
    }
    else if ((dev0->SR & TIM_SR_CC4IF) && (dev0->DIER & TIM_DIER_CC4IE)) {
        /* clear interrupt anyway */
        dev0->SR &= ~TIM_SR_CC4IF;
        /* if higher 16bit also match */
        if (dev1->CNT >= dev1->CCR4) {
            dev0->DIER &= ~TIM_DIER_CC4IE;
            config[timer].cb(3);
        }
        DEBUG("channel 4 CCR: %08x\n", ((dev1->CCR4<<16) | (0xffff & dev0->CCR4)));
    }
    else {
        dev0->SR = 0;
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /* TIMER_0_EN || TIMER_1_EN */
