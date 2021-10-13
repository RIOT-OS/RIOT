/*
 * Copyright (C) 2016 Leon George
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26xx_cc13xx
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation for the CC26x0
 *
 * @author      Leon M. George <leon@georgemail.eu>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "assert.h"
#include "board.h"
#include "cpu.h"
#include "periph_conf.h"
#include "periph/timer.h"

#include "cc26xx_cc13xx_power.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define LOAD_VALUE              (0xffff)

#define TIMER_A_IRQ_MASK        (0x000000ff)
#define TIMER_B_IRQ_MASK        (0x0000ff00)

#define TIMER_IRQ_PRIO          (1U)

typedef struct {
    uint16_t mask;
    uint16_t flag;
} _isr_cfg_t;

static _isr_cfg_t chn_isr_cfg[] = {
    { .mask = TIMER_A_IRQ_MASK, .flag = GPT_IMR_TAMIM },
    { .mask = TIMER_B_IRQ_MASK, .flag = GPT_IMR_TBMIM }
};

/**
 * @name function prototypes
 * @{
 */
static void irq_handler(tim_t tim, int channel);
/** @} */

/**
 * @brief   Allocate memory for timer interrupt context(s)
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

/**
 * @brief   Enable global interrupts for timer channel(s)
 *
 * @param[in] tim   index of the timer
 */
static void _irq_enable(tim_t tim)
{
    assert(tim < TIMER_NUMOF);

    /* enable global timer interrupt for channel A */
    IRQn_Type irqn = GPTIMER_0A_IRQN + (2 * tim);
    NVIC_SetPriority(irqn, TIMER_IRQ_PRIO);
    NVIC_EnableIRQ(irqn);
    /* and channel B, if enabled */
    if(timer_config[tim].chn == 2) {
        irqn++;
        NVIC_SetPriority(irqn, TIMER_IRQ_PRIO);
        NVIC_EnableIRQ(irqn);
    }
}

/**
 * @brief   Get the GPT register base for a timer
 *
 * @param[in] tim   index of the timer
 *
 * @return          base address
 */
static inline gpt_reg_t *dev(tim_t tim)
{
    assert(tim < TIMER_NUMOF);

    return ((gpt_reg_t *)(GPT0_BASE | (((uint32_t)tim) << 12)));
}

int timer_init(tim_t tim, uint32_t freq, timer_cb_t cb, void *arg)
{
    DEBUG("timer_init(%u, %lu)\n", tim, freq);
    /* make sure given timer is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* enable the timer clock */
    power_clock_enable_gpt(tim);

    /* disable (and reset) timer */
    dev(tim)->CTL = 0;

    /* save context */
    isr_ctx[tim].cb = cb;
    isr_ctx[tim].arg = arg;

    uint32_t chan_mode = (GPT_TXMR_TXMR_PERIODIC | GPT_TXMR_TXMIE);
    uint32_t prescaler = 0;
    if (timer_config[tim].cfg == GPT_CFG_32T) {
        if (timer_config[tim].chn > 1) {
            DEBUG("timer_init: in 32Bit mode single channel only!\n");
            return -1;
        }
        if (freq != RCOSC48M_FREQ) {
            DEBUG("timer_init: in 32Bit mode freq must equal system clock!\n");
            return -1;
        }
        chan_mode |= GPT_TXMR_TXCDIR_UP;
    }
    else if (timer_config[tim].cfg == GPT_CFG_16T) {
        /* prescaler only available in 16Bit mode */
        prescaler = RCOSC48M_FREQ;
        prescaler += freq / 2;
        prescaler /= freq;
        if (prescaler > 0) {
            prescaler--;
        }
        if (prescaler > 255) {
            prescaler = 255;
        }
        dev(tim)->TAILR = LOAD_VALUE;
        dev(tim)->TAPR = prescaler;
    }
    else {
        DEBUG("timer_init: invalid timer config must be 16 or 32Bit mode!\n");
        return -1;
    }
    /* configure channels and start timer */
    dev(tim)->CFG  = timer_config[tim].cfg;
    dev(tim)->CTL = GPT_CTL_TAEN;
    dev(tim)->TAMR = chan_mode;

    if (timer_config[tim].chn == 2) {
        /* set the timer speed */
        dev(tim)->TBPR = prescaler;
        dev(tim)->TBMR = chan_mode;
        dev(tim)->TBILR = LOAD_VALUE;
        dev(tim)->CTL = GPT_CTL_TAEN | GPT_CTL_TBEN;
    }
    /* enable timer IRQs */
    _irq_enable(tim);

    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    DEBUG("timer_set_absolute(%u, %u, %u)\n", tim, channel, value);

    if ((tim >= TIMER_NUMOF) || (channel >= timer_config[tim].chn)) {
        return -1;
    }

    dev(tim)->ICLR = chn_isr_cfg[channel].flag;
    if (channel == 0) {
        dev(tim)->TAMATCHR = (timer_config[tim].cfg == GPT_CFG_32T) ?
                             value : (LOAD_VALUE - value);
    }
    else {
        dev(tim)->TBMATCHR = (timer_config[tim].cfg == GPT_CFG_32T) ?
                             value : (LOAD_VALUE - value);
    }
    dev(tim)->IMR |= chn_isr_cfg[channel].flag;

    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    DEBUG("timer_clear(%u, %d)\n", tim, channel);
    if ((tim >= TIMER_NUMOF) || (channel >= timer_config[tim].chn)) {
        return -1;
    }
    /* clear interrupt flags */
    dev(tim)->IMR &= ~(chn_isr_cfg[channel].flag);

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    DEBUG("timer_read(%u)\n", tim);
    if (tim >= TIMER_NUMOF) {
        return 0;
    }
    if (timer_config[tim].cfg == GPT_CFG_32T) {
        return dev(tim)->TAV;
    }
    return LOAD_VALUE - (dev(tim)->TAV & 0xFFFF);
}

void timer_stop(tim_t tim)
{
    DEBUG("timer_stop(%u)\n", tim);
    if (tim < TIMER_NUMOF) {
        dev(tim)->CTL = 0;
    }
}

void timer_start(tim_t tim)
{
    DEBUG("timer_start(%u)\n", tim);

    if (tim < TIMER_NUMOF) {
        if (timer_config[tim].chn == 1) {
            dev(tim)->CTL = GPT_CTL_TAEN;
        }
        else if (timer_config[tim].chn == 2) {
            dev(tim)->CTL = GPT_CTL_TAEN | GPT_CTL_TBEN;
        }
    }
}

/**
 * @brief   Timer interrupt handler
 *
 * @param[in] tim   index of the timer
 * @param[in] chn   channel number (0=A, 1=B)
 */
static void irq_handler(tim_t tim, int channel)
{
    assert(tim < TIMER_NUMOF);
    assert(channel < timer_config[tim].chn);

    uint32_t mis;
    /* Latch the active interrupt flags */
    mis = dev(tim)->MIS & chn_isr_cfg[channel].mask;
    /* Clear the latched interrupt flags */
    dev(tim)->ICLR = mis;

    if (mis & chn_isr_cfg[channel].flag) {
        /* Disable further match interrupts for this timer/channel */
        dev(tim)->IMR &= ~chn_isr_cfg[channel].flag;
        /* Invoke the callback function */
        isr_ctx[tim].cb(isr_ctx[tim].arg, channel);
    }

    cortexm_isr_end();
}

void isr_timer0_chan0(void) {irq_handler(0, 0);}
void isr_timer0_chan1(void) {irq_handler(0, 1);}
void isr_timer1_chan0(void) {irq_handler(1, 0);}
void isr_timer1_chan1(void) {irq_handler(1, 1);}
void isr_timer2_chan0(void) {irq_handler(2, 0);}
void isr_timer2_chan1(void) {irq_handler(2, 1);}
void isr_timer3_chan0(void) {irq_handler(3, 0);}
void isr_timer3_chan1(void) {irq_handler(3, 1);}
