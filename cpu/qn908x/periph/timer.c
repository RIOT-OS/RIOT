/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_qn908x
 * @ingroup     drivers_periph_timer
 *
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * This driver leverages the Freescale/NXP implementation distributed with the
 * SDK.
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include <stdlib.h>

#include "bitarithm.h"
#include "board.h"
#include "cpu.h"
#include "periph/timer.h"
#include "periph_conf.h"

#include "vendor/drivers/fsl_clock.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Interrupt context information for configured timers.
 */
static timer_isr_ctx_t isr_ctx[FSL_FEATURE_SOC_CTIMER_COUNT];

/**
 * @brief   CTIMER peripheral base pointers.
 */
static CTIMER_Type* const ctimers[FSL_FEATURE_SOC_CTIMER_COUNT] =
    CTIMER_BASE_PTRS;

/**
 * @brief   CTIMER IRQ numbers.
 */
static IRQn_Type const ctimers_irqn[FSL_FEATURE_SOC_CTIMER_COUNT] =
    CTIMER_IRQS;

/**
 * @brief   CTIMER Clocks.
 */
static const clock_ip_name_t ctimers_clocks[FSL_FEATURE_SOC_CTIMER_COUNT] =
    CTIMER_CLOCKS;

/**
 * @brief   Check the board config to make sure we do not exceed max number of
 *          timers
 */
#if TIMER_NUMOF > FSL_FEATURE_SOC_CTIMER_COUNT
#error "ERROR in board timer configuration: too many timers defined"
#endif

uword_t timer_query_freqs_numof(tim_t dev)
{
    assert(dev < TIMER_NUMOF);
    (void)dev;
    return 256;
}

uint32_t timer_query_freqs(tim_t dev, uword_t index)
{
    assert(dev < TIMER_NUMOF);
    (void)dev;

    if (index >= UINT8_MAX) {
        return 0;
    }

    return CLOCK_GetFreq(kCLOCK_ApbClk) / (index + 1);
}

int timer_init(tim_t tim, uint32_t freq, timer_cb_t cb, void *arg)
{
    DEBUG("timer_init(%u, %" PRIu32 ")\n", tim, freq);
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    isr_ctx[tim].cb = cb;
    isr_ctx[tim].arg = arg;

    CLOCK_EnableClock(ctimers_clocks[tim]);

    CTIMER_Type *dev = ctimers[tim];

    /* CTIMER blocks are driven from the APB clock. */
    uint32_t core_freq = CLOCK_GetFreq(kCLOCK_ApbClk);
    uint32_t prescale = (core_freq + freq / 2) / freq - 1;
    if (prescale == (uint32_t)(-1)) {
        DEBUG("timer_init: Frequency %" PRIu32 " is too fast for core_freq=%" PRIu32,
              freq, core_freq);
        return -1;
    }

    dev->CTCR = CTIMER_CTCR_CTMODE(0); /* timer mode */
    dev->PR = CTIMER_PR_PRVAL(prescale);
    /* Enable timer interrupts in the NVIC. */
    NVIC_EnableIRQ(ctimers_irqn[tim]);

    /* Timer should be started after init. */
    dev->TCR |= CTIMER_TCR_CEN_MASK;
    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    DEBUG("timer_set_absolute(%u, %u, %u)\n", tim, channel, value);
    if ((tim >= TIMER_NUMOF) || (channel >= TIMER_CHANNEL_NUMOF)) {
        return -1;
    }
    CTIMER_Type* const dev = ctimers[tim];
    dev->MR[channel] = value;
    dev->MCR |= (CTIMER_MCR_MR0I_MASK << (channel * 3));
    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int value)
{
    DEBUG("timer_set(%u, %u, %u)\n", tim, channel, value);
    if ((tim >= TIMER_NUMOF) || (channel >= TIMER_CHANNEL_NUMOF)) {
        return -1;
    }
    CTIMER_Type* const dev = ctimers[tim];

    /* no IRQ will be generated on value == 0, so bump it here */
    value = (value != 0) ? value : 1;

    unsigned irq_state = irq_disable();

    /* briefly pause timer */
    ctimers[tim]->TCR &= ~CTIMER_TCR_CEN_MASK;
    /* set absolute timeout based on given value and enable IRQ */
    dev->MR[channel] = dev->TC + value;
    dev->MCR |= (CTIMER_MCR_MR0I_MASK << (channel * 3));
    /* and resume timer */
    ctimers[tim]->TCR |= CTIMER_TCR_CEN_MASK;
    irq_restore(irq_state);

    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    DEBUG("timer_clear(%u, %d)\n", tim, channel);
    if ((tim >= TIMER_NUMOF) || (channel >= TIMER_CHANNEL_NUMOF)) {
        return -1;
    }
    CTIMER_Type* const dev = ctimers[tim];
    dev->MCR &= ~(CTIMER_MCR_MR0I_MASK << (channel * 3));
    return 0;
}

unsigned int timer_read(tim_t tim)
{
    DEBUG("timer_read(%u) --> %" PRIu32 "\n", tim, ctimers[tim]->TC);
    return ctimers[tim]->TC;
}

void timer_start(tim_t tim)
{
    DEBUG("timer_start(%u)\n", tim);
    ctimers[tim]->TCR |= CTIMER_TCR_CEN_MASK;
}

void timer_stop(tim_t tim)
{
    DEBUG("timer_stop(%u)\n", tim);
    ctimers[tim]->TCR &= ~CTIMER_TCR_CEN_MASK;
}

static inline void isr_ctimer_n(CTIMER_Type *dev, uint32_t ctimer_num)
{
    DEBUG("isr_ctimer_%" PRIu32 " flags=0x%" PRIx32 "\n",
          ctimer_num, dev->IR);
    unsigned state = dev->IR & ((1 << TIMER_CHANNEL_NUMOF) - 1);
    while (state) {
        uint8_t channel;
        state = bitarithm_test_and_clear(state, &channel);
        /* Note: setting the bit to 1 in the flag register will clear the
         * bit. */
        dev->IR = 1u << channel;
        dev->MCR &= ~(CTIMER_MCR_MR0I_MASK << (channel * 3));
        isr_ctx[ctimer_num].cb(isr_ctx[ctimer_num].arg, channel);
    }
    cortexm_isr_end();
}

#ifdef CTIMER0
void isr_ctimer0(void) { isr_ctimer_n(CTIMER0, 0); }
#endif /* CTIMER0 */

#ifdef CTIMER1
void isr_ctimer1(void) { isr_ctimer_n(CTIMER1, 1); }
#endif /* CTIMER1 */

#ifdef CTIMER2
void isr_ctimer2(void) { isr_ctimer_n(CTIMER2, 2); }
#endif /* CTIMER2 */

#ifdef CTIMER3
void isr_ctimer3(void) { isr_ctimer_n(CTIMER3, 3); }
#endif /* CTIMER3 */
