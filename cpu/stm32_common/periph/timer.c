/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/timer.h"
#include "stmclk.h"

/**
 * @brief   Timer specific additional bus clock presacler
 *
 * This prescale factor is dependent on the actual APBx bus clock divider, if
 * the APBx presacler is != 1, it is set to 2, if the APBx prescaler is == 1, it
 * is set to 1.
 *
 * See reference manuals section 'reset and clock control'.
 */
static const uint8_t apbmul[] = {
#if (CLOCK_APB1 < CLOCK_CORECLOCK)
    [APB1] = 2,
#else
    [APB1] = 1,
#endif
#if (CLOCK_APB2 < CLOCK_CORECLOCK)
    [APB2] = 2
#else
    [APB2] = 1
#endif
};

/**
 * @brief   Interrupt context for each configured timer
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

/**
 * @brief   Get the timer device
 */
static inline TIM_TypeDef *dev(tim_t tim)
{
    return timer_config[tim].dev;
}

int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    /* check if device is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* remember the interrupt context */
    isr_ctx[tim].cb = cb;
    isr_ctx[tim].arg = arg;

    /* enable the peripheral clock */
    periph_clk_en(timer_config[tim].bus, timer_config[tim].rcc_mask);

    switch (timer_config[tim].type) {
    case TIMER_TYPE_GPTIM:
        /* configure the timer as upcounter in continuous mode */
        dev(tim)->CR1  = 0;
        dev(tim)->CR2  = 0;
        dev(tim)->ARR  = timer_config[tim].max;

        /* set prescaler */
        dev(tim)->PSC = (((periph_apb_clk(timer_config[tim].bus) *
                           apbmul[timer_config[tim].bus]) / freq) - 1);
        /* generate an update event to apply our configuration */
        dev(tim)->EGR = TIM_EGR_UG;
        break;
#if defined(LPTIM1)
    case TIMER_TYPE_LPTIM:
        if (freq != 32768ul) {
            return -1;
        }
        stmclk_enable_lfclk();
        EXTI->IMR |= EXTI_IMR_MR23;
        EXTI->RTSR |= EXTI_RTSR_TR23;
        LPTIM1->CR = 0;
        RCC->DCKCFGR2 |= RCC_DCKCFGR2_LPTIM1SEL_1 | RCC_DCKCFGR2_LPTIM1SEL_0;
        LPTIM1->CFGR = 0 /*LPTIM_CFGR_TIMOUT*/;
        LPTIM1->IER = LPTIM_IER_CMPMIE;
        LPTIM1->CR |= LPTIM_CR_ENABLE;
        LPTIM1->ARR = timer_config[tim].max;
        break;
#endif
    default:
        return -1;
    }

    /* enable the timer's interrupt */
    NVIC_EnableIRQ(timer_config[tim].irqn);
    /* reset the counter and start the timer */
    timer_start(tim);

    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    int now = timer_read(tim);
    return timer_set_absolute(tim, channel, now + timeout);
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if (channel >= TIMER_CHAN) {
        return -1;
    }

    switch (timer_config[tim].type) {
    case TIMER_TYPE_GPTIM:
        dev(tim)->CCR[channel] = (value & timer_config[tim].max);
        dev(tim)->SR &= ~(TIM_SR_CC1IF << channel);
        dev(tim)->DIER |= (TIM_DIER_CC1IE << channel);
#if defined(LPTIM1)
    case TIMER_TYPE_LPTIM:
        LPTIM1->CMP = (value & timer_config[tim].max);
        LPTIM1->CR |= LPTIM_CR_CNTSTRT;
        break;
#endif
    }

    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    if (channel >= TIMER_CHAN) {
        return -1;
    }

    switch (timer_config[tim].type) {
    case TIMER_TYPE_GPTIM:
        dev(tim)->DIER &= ~(TIM_DIER_CC1IE << channel);
        break;
#if defined(LPTIM1)
    case TIMER_TYPE_LPTIM:
        break;
#endif
    }

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    switch (timer_config[tim].type) {
    case TIMER_TYPE_GPTIM:
        return (unsigned int)dev(tim)->CNT;
#if defined(LPTIM1)
    case TIMER_TYPE_LPTIM:
        return (unsigned int)LPTIM1->CNT;
#endif
    }

    return 0;
}

void timer_start(tim_t tim)
{
    switch (timer_config[tim].type) {
    case TIMER_TYPE_GPTIM:
        dev(tim)->CR1 |= TIM_CR1_CEN;
        break;
#if defined(LPTIM1)
    case TIMER_TYPE_LPTIM:
        LPTIM1->CR |= LPTIM_CR_ENABLE | LPTIM_CR_CNTSTRT;
        break;
#endif
    }
}

void timer_stop(tim_t tim)
{
    switch (timer_config[tim].type) {
    case TIMER_TYPE_GPTIM:
        dev(tim)->CR1 &= ~(TIM_CR1_CEN);
        break;
#if defined(LPTIM1)
    case TIMER_TYPE_LPTIM:
        LPTIM1->CR &= ~(LPTIM_CR_ENABLE);
        break;
#endif
    }
}

static inline void irq_handler(tim_t tim)
{
    uint32_t status;

    switch (timer_config[tim].type) {
    case TIMER_TYPE_GPTIM:
        status = (dev(tim)->SR & dev(tim)->DIER);

        for (unsigned int i = 0; i < TIMER_CHAN; i++) {
            if (status & (TIM_SR_CC1IF << i)) {
                dev(tim)->DIER &= ~(TIM_DIER_CC1IE << i);
                isr_ctx[tim].cb(isr_ctx[tim].arg, i);
            }
        }
        break;
#if defined(LPTIM1)
    case TIMER_TYPE_LPTIM:
        LPTIM1->ICR = (LPTIM_ICR_ARRMCF | LPTIM_ICR_CMPMCF);
        EXTI->PR |= EXTI_PR_PR23;
        isr_ctx[tim].cb(isr_ctx[tim].arg, 0);
        break;
#endif
    }
    cortexm_isr_end();
}

#ifdef TIMER_0_ISR
void TIMER_0_ISR(void)
{
    irq_handler(0);
}
#endif

#ifdef TIMER_1_ISR
void TIMER_1_ISR(void)
{
    irq_handler(1);
}
#endif

#ifdef TIMER_2_ISR
void TIMER_2_ISR(void)
{
    irq_handler(2);
}
#endif

#ifdef TIMER_3_ISR
void TIMER_3_ISR(void)
{
    irq_handler(3);
}
#endif

#ifdef TIMER_4_ISR
void TIMER_4_ISR(void)
{
    irq_handler(4);
}
#endif
