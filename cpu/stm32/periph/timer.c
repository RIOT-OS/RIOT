/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
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

/**
 * @brief   Get the number of channels of the timer device
 */
static unsigned channel_numof(tim_t tim)
{
    if (timer_config[tim].channel_numof) {
        return timer_config[tim].channel_numof;
    }

    /* backwards compatibility with older periph_conf.h files when all STM32
     * had exactly 4 channels */
    return TIMER_CHANNEL_NUMOF;
}


#ifdef MODULE_PERIPH_TIMER_PERIODIC

/**
 * @brief   Helper macro to get channel bit in timer/channel bitmap
 */
#define CHAN_BIT(tim, chan) (1 << chan) << (TIMER_CHANNEL_NUMOF * (tim & 1))

/**
 * @brief   Bitmap for compare channel disable after match
 */
static uint8_t _oneshot[(TIMER_NUMOF+1)/2];

/**
 * @brief   Clear interrupt enable after the interrupt has fired
 */
static inline void set_oneshot(tim_t tim, int chan)
{
    _oneshot[tim >> 1] |= CHAN_BIT(tim, chan);
}

/**
 * @brief   Enable interrupt with every wrap-around of the timer
 */
static inline void clear_oneshot(tim_t tim, int chan)
{
    _oneshot[tim >> 1] &= ~CHAN_BIT(tim, chan);
}

static inline bool is_oneshot(tim_t tim, int chan)
{
    return _oneshot[tim >> 1] & CHAN_BIT(tim, chan);
}

#else /* !MODULE_PERIPH_TIMER_PERIODIC */

static inline void set_oneshot(tim_t tim, int chan)
{
    (void)tim;
    (void)chan;
}

static inline bool is_oneshot(tim_t tim, int chan)
{
    (void)tim;
    (void)chan;

    return true;
}

#endif /* MODULE_PERIPH_TIMER_PERIODIC */

int timer_init(tim_t tim, uint32_t freq, timer_cb_t cb, void *arg)
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

    /* configure the timer as upcounter in continuous mode */
    dev(tim)->CR1  = 0;
    dev(tim)->CR2  = 0;
    dev(tim)->ARR  = timer_config[tim].max;

    /* set prescaler */
    dev(tim)->PSC = ((periph_timer_clk(timer_config[tim].bus) / freq) - 1);
    /* generate an update event to apply our configuration */
    dev(tim)->EGR = TIM_EGR_UG;

    /* enable the timer's interrupt */
    NVIC_EnableIRQ(timer_config[tim].irqn);
    /* reset the counter and start the timer */
    timer_start(tim);

    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if ((unsigned)channel >= channel_numof(tim)) {
        return -1;
    }

    unsigned irqstate = irq_disable();
    set_oneshot(tim, channel);

#ifdef MODULE_PERIPH_TIMER_PERIODIC
    if (dev(tim)->ARR == TIM_CHAN(tim, channel)) {
        dev(tim)->ARR = timer_config[tim].max;
    }
#endif

    /* clear spurious IRQs */
    dev(tim)->SR &= ~(TIM_SR_CC1IF << channel);

    TIM_CHAN(tim, channel) = (value & timer_config[tim].max);

    /* enable IRQ */
    dev(tim)->DIER |= (TIM_DIER_CC1IE << channel);
    irq_restore(irqstate);

    return 0;
}

uword_t timer_query_freqs_numof(tim_t dev)
{
    (void)dev;
    /* Prescaler values from 0 to UINT16_MAX are supported */
    return UINT16_MAX + 1;
}

uint32_t timer_query_freqs(tim_t dev, uword_t index)
{

    if (index > UINT16_MAX) {
        return 0;
    }

    return periph_timer_clk(timer_config[dev].bus) / (index + 1);
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    unsigned value = (dev(tim)->CNT + timeout) & timer_config[tim].max;

    if ((unsigned)channel >= channel_numof(tim)) {
        return -1;
    }

    unsigned irqstate = irq_disable();
    set_oneshot(tim, channel);

#ifdef MODULE_PERIPH_TIMER_PERIODIC
    if (dev(tim)->ARR == TIM_CHAN(tim, channel)) {
        dev(tim)->ARR = timer_config[tim].max;
    }
#endif

    /* clear spurious IRQs */
    dev(tim)->SR &= ~(TIM_SR_CC1IF << channel);

    TIM_CHAN(tim, channel) = value;

    /* enable IRQ */
    dev(tim)->DIER |= (TIM_DIER_CC1IE << channel);

    /* calculate time till timeout */
    value = (value - dev(tim)->CNT) & timer_config[tim].max;

    if (value > timeout) {
        /* time till timeout is larger than requested --> timer already expired
         * ==> let's make sure we have an IRQ pending :) */
        dev(tim)->EGR |= (TIM_EGR_CC1G << channel);
    }

    irq_restore(irqstate);

    return 0;
}

#ifdef MODULE_PERIPH_TIMER_PERIODIC
int timer_set_periodic(tim_t tim, int channel, unsigned int value, uint8_t flags)
{
    if ((unsigned)channel >= channel_numof(tim)) {
        return -1;
    }

    unsigned irqstate = irq_disable();
    clear_oneshot(tim, channel);

    if (flags & TIM_FLAG_SET_STOPPED) {
        timer_stop(tim);
    }

    if (flags & TIM_FLAG_RESET_ON_SET) {
        /* setting COUNT gives us an interrupt on all channels */
        dev(tim)->CNT = 0;

        /* wait for the interrupt & clear it */
        while(dev(tim)->SR == 0) {}
        dev(tim)->SR = 0;
    }

    TIM_CHAN(tim, channel) = value;

    /* clear spurious IRQs */
    dev(tim)->SR &= ~(TIM_SR_CC1IF << channel);

    /* enable IRQ */
    dev(tim)->DIER |= (TIM_DIER_CC1IE << channel);

    if (flags & TIM_FLAG_RESET_ON_MATCH) {
        dev(tim)->ARR = value;
    }
    irq_restore(irqstate);

    return 0;
}
#endif /* MODULE_PERIPH_TIMER_PERIODIC */

int timer_clear(tim_t tim, int channel)
{
    if ((unsigned)channel >= channel_numof(tim)) {
        return -1;
    }

    unsigned irqstate = irq_disable();
    dev(tim)->DIER &= ~(TIM_DIER_CC1IE << channel);
    irq_restore(irqstate);

#ifdef MODULE_PERIPH_TIMER_PERIODIC
    if (dev(tim)->ARR == TIM_CHAN(tim, channel)) {
        dev(tim)->ARR = timer_config[tim].max;
    }
#endif

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    return (unsigned int)dev(tim)->CNT;
}

void timer_start(tim_t tim)
{
    unsigned irqstate = irq_disable();
    dev(tim)->CR1 |= TIM_CR1_CEN;
    irq_restore(irqstate);
}

void timer_stop(tim_t tim)
{
    unsigned irqstate = irq_disable();
    dev(tim)->CR1 &= ~(TIM_CR1_CEN);
    irq_restore(irqstate);
}

static inline void irq_handler(tim_t tim)
{
    uint32_t top = dev(tim)->ARR;
    uint32_t status = dev(tim)->SR & dev(tim)->DIER;

    /* clear interrupts which we are about to service */
    /* Note, the flags in the SR register can be cleared by software, but
     * setting them has no effect on the register. Only the hardware can set
     * them. */
    dev(tim)->SR = ~status;

    for (unsigned int i = 0; status; i++) {
        uint32_t msk = TIM_SR_CC1IF << i;

        /* check if interrupt flag is set */
        if ((status & msk) == 0) {
            continue;
        }
        status &= ~msk;

        /* interrupt flag gets set for all channels > ARR */
        if (TIM_CHAN(tim, i) > top) {
            continue;
        }

        /* disable Interrupt */
        if (is_oneshot(tim, i)) {
            dev(tim)->DIER &= ~msk;
        }

        isr_ctx[tim].cb(isr_ctx[tim].arg, i);
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
