/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * This implementation does only support one fixed timer, as defined in the
 * boards periph_conf.h file.
 *
 * @todo        Generalize to handle more timers and make them configurable
 *              through the board's `periph_conf.h`
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include "compiler_hints.h"
#include "cpu.h"
#include "periph/timer.h"
#include "periph_conf.h"
#include "periph_cpu.h"

/**
 * @brief   Interrupt context for each configured timer
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

/* Hack to count the number of ISR vectors provided by the board */
enum {
#ifdef TIMER0_ISR_CC0
  TIMER_ISR_COUNT_HELPER_0,
#endif
#ifdef TIMER1_ISR_CC0
  TIMER_ISR_COUNT_HELPER_1,
#endif
  TIMER_ISR_NUMOF
};

static_assert((TXID_DIV_MAX << TXID_DIV_Pos) == TXID_DIV_Msk, "TXID_DIV_MAX or TXID_DIV_Pos is incorrect.");
static_assert(TIMER_ISR_NUMOF == TIMER_NUMOF,
              "For each provided timer a corresponding IRQ number needs to be provided by the board.");

static uint32_t abs_diff(uint32_t a, uint32_t b)
{
    if (a >= b) {
        return a - b;
    }

    return b - a;
}

static uint16_t prescale(msp430_timer_clock_source_t clock_source, uint32_t freq)
{
    uint32_t clock_freq;
    assume((clock_source == TIMER_CLOCK_SOURCE_AUXILIARY_CLOCK) ||
           (clock_source == TIMER_CLOCK_SOURCE_SUBMAIN_CLOCK));
    switch (clock_source) {
    default:
    case TIMER_CLOCK_SOURCE_AUXILIARY_CLOCK:
        clock_freq = msp430_auxiliary_clock_freq();
        break;
    case TIMER_CLOCK_SOURCE_SUBMAIN_CLOCK:
        clock_freq = msp430_submain_clock_freq();
        break;
    }

    unsigned prescaler = 0;
    uint32_t best_diff = UINT32_MAX;

    for (unsigned i = 0; i <= TXID_DIV_MAX; i++) {
        uint32_t prescaled_freq = clock_freq >> i;
        uint32_t off = abs_diff(freq, prescaled_freq);
        if (off < best_diff) {
            best_diff = off;
            prescaler = i;
        }
    }

    return clock_source | (prescaler << TXID_DIV_Pos);
}

int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    assume((unsigned)dev < TIMER_NUMOF);
    msp430_timer_t *msptimer = timer_conf[dev].timer;

    /* reset the timer A configuration */
    msptimer->CTL = TACLR;
    /* save callback */
    isr_ctx[dev].cb = cb;
    isr_ctx[dev].arg = arg;
    /* compute prescaler */
    uint16_t ctl = prescale(timer_conf[dev].clock_source, freq);
    msptimer->CTL = ctl;
    /* configure CC channels */
    for (unsigned i = 0; i < timer_query_channel_numof(dev); i++) {
        msptimer->CCTL[i] = 0;
    }
    /* start the timer in continuous mode */
    msptimer->CTL = ctl | TXMC_CONT;
    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    assume((unsigned)dev < TIMER_NUMOF);
    msp430_timer_t *msptimer = timer_conf[dev].timer;

    if ((unsigned)channel >= timer_query_channel_numof(dev)) {
        return -1;
    }

    msptimer->CCR[channel] = value;
    msptimer->CCTL[channel] &= ~(CCIFG);
    msptimer->CCTL[channel] |=  CCIE;
    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    assume((unsigned)dev < TIMER_NUMOF);
    msp430_timer_t *msptimer = timer_conf[dev].timer;

    if ((unsigned)channel >= timer_query_channel_numof(dev)) {
        return -1;
    }
    msptimer->CCTL[channel] &= ~(CCIE);
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    assume((unsigned)dev < TIMER_NUMOF);
    msp430_timer_t *msptimer = timer_conf[dev].timer;
    return msptimer->R;
}

void timer_start(tim_t dev)
{
    assume((unsigned)dev < TIMER_NUMOF);
    msp430_timer_t *msptimer = timer_conf[dev].timer;
    msptimer->CTL |= TXMC_CONT;
}

void timer_stop(tim_t dev)
{
    assume((unsigned)dev < TIMER_NUMOF);
    msp430_timer_t *msptimer = timer_conf[dev].timer;
    msptimer->CTL &= ~(TXMC_MASK);
}

__attribute__((pure))
uword_t timer_query_freqs_numof(tim_t dev)
{
    assume((unsigned)dev < TIMER_NUMOF);
    /* Smallest div value is 0, so number is max + 1 */
    return TXID_DIV_MAX + 1;
}

__attribute__((pure))
uword_t timer_query_channel_numof(tim_t dev)
{
    assume((unsigned)dev < TIMER_NUMOF);
    if (timer_conf[dev].timer == &TIMER_A) {
        return 3;
    }

    return 7;
}

uint32_t timer_query_freqs(tim_t dev, uword_t index)
{
    assume((unsigned)dev < TIMER_NUMOF);
    const msp430_timer_clock_source_t clock_source = timer_conf[dev].clock_source;
    assume((clock_source == TIMER_CLOCK_SOURCE_AUXILIARY_CLOCK) ||
           (clock_source == TIMER_CLOCK_SOURCE_SUBMAIN_CLOCK));

    if (index > TXID_DIV_MAX) {
        return 0;
    }

    uint32_t clock_freq;
    switch (clock_source) {
    default:
    case TIMER_CLOCK_SOURCE_AUXILIARY_CLOCK:
        clock_freq = msp430_auxiliary_clock_freq();
        break;
    case TIMER_CLOCK_SOURCE_SUBMAIN_CLOCK:
        clock_freq = msp430_submain_clock_freq();
        break;
    }

    return clock_freq >> index;
}

static void timer_isr(tim_t dev, unsigned chan)
{
    assume((unsigned)dev < TIMER_NUMOF);
    msp430_timer_t *msptimer = timer_conf[dev].timer;
    /* disable IRQ */
    msptimer->CCTL[chan] &= ~(CCIE);
    isr_ctx[dev].cb(isr_ctx[dev].arg, chan);

}
static void timer_isr_cc0(tim_t dev)
{
    assume((unsigned)dev < TIMER_NUMOF);
    timer_isr(dev, 0);
}

static void timer_isr_ccx(tim_t dev)
{
    assume((unsigned)dev < TIMER_NUMOF);
    unsigned chan = *timer_conf[dev].irq_flags >> 1U;
    if (chan >= timer_query_channel_numof(dev)) {
        /* timer overflown */
    }
    else {
        /* CC matched */
        timer_isr(dev, chan);
    }
}

ISR(TIMER0_ISR_CC0, isr_timer0_cc0)
{
    __enter_isr();

    timer_isr_cc0(0);

    __exit_isr();
}

ISR(TIMER0_ISR_CCX, isr_timer0_ccx)
{
    __enter_isr();

    timer_isr_ccx(0);

    __exit_isr();
}

#ifdef TIMER1_ISR_CC0
ISR(TIMER1_ISR_CC0, isr_timer1_cc0)
{
    __enter_isr();

    timer_isr_cc0(1);

    __exit_isr();
}

ISR(TIMER1_ISR_CCX, isr_timer1_ccx)
{
    __enter_isr();

    timer_isr_ccx(1);

    __exit_isr();
}
#endif
