/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation for the CC2538 CPU
 *
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "vendor/hw_gptimer.h"
#include "vendor/hw_memmap.h"

#include "board.h"
#include "cpu.h"
#include "periph/timer.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define LOAD_VALUE_16_BIT       (UINT16_MAX)
#define LOAD_VALUE_32_BIT       (UINT32_MAX)

#define TIMER_A_IRQ_MASK        (0x000000ff)
#define TIMER_B_IRQ_MASK        (0x0000ff00)

/* GPTIMER_TnMR Bits */
#define GPTIMER_TnMR_TnMIE       GPTIMER_TAMR_TAMIE
#define GPTIMER_TnMR_TnCDIR      GPTIMER_TAMR_TACDIR

typedef struct {
    uint16_t mask;
    uint16_t flag;
} _isr_cfg_t;

static const _isr_cfg_t chn_isr_cfg[] = {
    { .mask = TIMER_A_IRQ_MASK, .flag = GPTIMER_IMR_TAMIM },
    { .mask = TIMER_B_IRQ_MASK, .flag = GPTIMER_IMR_TBMIM }
};

/**
 * @brief Timer state memory
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

/* pending timer compare values TxMATCHR */
static union {
    uint16_t u16[2];    /* TIMERA, TIMERB 16bit mode */
    uint32_t u32;       /* extended TIMERA 32bit mode */
} _set_values[TIMER_NUMOF];

/* 2 channels per timer, TIMER_NUMOF <= 4 */
static uint8_t _set_timers;

static void _set_absolute_disabled(tim_t tim, int chan, unsigned int value)
{
     /* each timer can have two channels*/
    _set_timers |= ((chan + 1) << (2 * tim));

    if (timer_config[tim].cfg == GPTMCFG_32_BIT_TIMER) {
        _set_values[tim].u32 = value;
    } else {
        _set_values[tim].u16[chan] = value;
    }
}

static void _set_pending(tim_t tim)
{
    /* create mask to get set channels of the current timer */
    const unsigned ch1_msk = (1 << (2 * tim));
    const unsigned ch2_msk = (2 << (2 * tim));

    if (_set_timers & ch1_msk) {
        _set_timers &= ~ch1_msk;

        if (timer_config[tim].cfg == GPTMCFG_32_BIT_TIMER) {
            timer_set_absolute(tim, 0, _set_values[tim].u32);
            return;
        } else {
            timer_set_absolute(tim, 0, _set_values[tim].u16[0]);
        }
    }
    if (_set_timers & ch2_msk) {
        _set_timers &= ~ch2_msk;
        timer_set_absolute(tim, 1, _set_values[tim].u16[1]);
    }
}

/* enable timer interrupts */
static inline void _irq_enable(tim_t tim)
{
    DEBUG("%s(%u)\n", __FUNCTION__, tim);

    if (tim < TIMER_NUMOF) {
        IRQn_Type irqn;
        switch (tim) {
            case 0:
                irqn = GPTIMER_0A_IRQn;
                break;
            case 1:
                irqn = GPTIMER_1A_IRQn;
                break;
            case 2:
                irqn = GPTIMER_2A_IRQn;
                break;
            case 3:
                irqn = GPTIMER_3A_IRQn;
                break;
            default:
                assume(0);
                return;
        }
        NVIC_SetPriority(irqn, TIMER_IRQ_PRIO);
        NVIC_EnableIRQ(irqn);

        if (timer_config[tim].chn == 2) {
            irqn++;
            NVIC_SetPriority(irqn, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(irqn);
        }
    }
}

static inline void _timer_clock_enable(tim_t tim)
{
    DEBUG("%s\n", __FUNCTION__);

    /* enable GPT(tim) clock in active mode */
    SYS_CTRL->RCGCGPT |= (1UL << tim);
    /* enable GPT(tim) clock in sleep mode */
    SYS_CTRL->SCGCGPT |= (1UL << tim);
    /* enable GPT(tim) clock in PM0 (system clock always powered down
        in PM1-3) */
    SYS_CTRL->DCGCGPT |= (1UL << tim);
    /* wait for the clock enabling to take effect */
    while (!(SYS_CTRL->RCGCGPT & (1UL << tim)) || \
           !(SYS_CTRL->SCGCGPT & (1UL << tim)) || \
           !(SYS_CTRL->DCGCGPT & (1UL << tim))
           ) {}

    /* set pending timers */
    _set_pending(tim);
}

static inline void _timer_clock_disable(tim_t tim)
{
    DEBUG("%s\n", __FUNCTION__);

    /* gate GPT(tim) clock in active mode */
    SYS_CTRL->RCGCGPT &= ~(1UL << tim);
    /* gate GPT(tim) clock in sleep mode */
    SYS_CTRL->SCGCGPT &= ~(1UL << tim);
    /* gate GPT(tim) clock in PM0 (system clock always powered down
       in PM1-3) */
    SYS_CTRL->DCGCGPT &= ~(1UL << tim);
    /* Wait for the clock gating to take effect */
    while ((SYS_CTRL->RCGCGPT & (1UL << tim)) || \
           (SYS_CTRL->SCGCGPT & (1UL << tim)) || \
           (SYS_CTRL->DCGCGPT & (1UL << tim))
           ) {}
}

static inline cc2538_gptimer_t *dev(tim_t tim)
{
    assert(tim < TIMER_NUMOF);

    return ((cc2538_gptimer_t *)(GPTIMER0_BASE | (((uint32_t)tim) << 12)));
}

/**
 * @brief Setup the given timer
 *
 */
int timer_init(tim_t tim, uint32_t freq, timer_cb_t cb, void *arg)
{
    DEBUG("%s(%u, %lu, %p, %p)\n", __FUNCTION__, tim, freq, cb, arg);

    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* Save the callback function: */
    isr_ctx[tim].cb = cb;
    isr_ctx[tim].arg = arg;

    /* enable timer clock in active, sleep or PM0 */
    _timer_clock_enable(tim);

    /* Disable this timer before configuring it: */
    dev(tim)->CTL = 0;

    uint32_t prescaler = 0;
    uint32_t chan_mode = GPTIMER_TnMR_TnMIE | GPTIMER_PERIODIC_MODE;
    /* Count down in GPTMCFG_16_BIT_TIMER so prescaler is a true prescaler */
    /* Count up in GPTMCFG_32_BIT_TIMER since prescaler is irrelevant */
    if (timer_config[tim].cfg == GPTMCFG_32_BIT_TIMER) {
        chan_mode |= GPTIMER_TnMR_TnCDIR;

        if (timer_config[tim].chn > 1) {
            DEBUG("Invalid timer_config. Multiple channels are available only \
                   in 16-bit mode.");
            return -1;
        }

        if (freq != sys_clock_freq()) {
            DEBUG("In 32-bit mode, the GPTimer frequency must equal the system \
                  clock frequency (%u).\n", (unsigned)sys_clock_freq());
            return -1;
        }
    }
    else if (timer_config[tim].cfg == GPTMCFG_16_BIT_TIMER) {
        prescaler = sys_clock_freq();
        prescaler += freq / 2;
        prescaler /= freq;
        if (prescaler > 0) {
            prescaler--;
        }
        if (prescaler > 255) {
            prescaler = 255;
        }
        dev(tim)->TAPR = prescaler;
        dev(tim)->TBPR = prescaler;
    }
    else {
        DEBUG("timer_init: invalid timer config must be 16 or 32Bit mode!\n");
        return -1;
    }

    dev(tim)->CFG = timer_config[tim].cfg;
    /* enable and configure GPTM(tim) timer A */
    dev(tim)->TAMR = chan_mode;
    dev(tim)->TAILR = (timer_config[tim].cfg == GPTMCFG_32_BIT_TIMER) ?
                      LOAD_VALUE_32_BIT : LOAD_VALUE_16_BIT;
    dev(tim)->CTL |= GPTIMER_CTL_TAEN;

    if (timer_config[tim].chn > 1) {
        /* Enable and configure GPTM(tim) timer B */
        dev(tim)->TBMR = chan_mode;
        dev(tim)->TBILR = LOAD_VALUE_16_BIT;
        dev(tim)->CTL |= GPTIMER_CTL_TBEN;
    }

    /* Enable interrupts for given timer: */
    _irq_enable(tim);

    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    DEBUG("%s(%u, %u, %u)\n", __FUNCTION__, tim, channel, value);

    if ((tim >= TIMER_NUMOF) || (channel >= (int)timer_config[tim].chn) ) {
        return -1;
    }

    /* GPT timer needs to be gated to write to registers, no need to
       check all xCGCGPT since they are set and unset at the same time */
    bool timer_on = (SYS_CTRL->RCGCGPT & (1UL << tim));
    /* if timer is stopped then set the desired timer compare values (TxMARCHR)
       the next time the timer is started */
    if (!timer_on) {
        _set_absolute_disabled(tim, channel, value);
        return 0;
    }

    /* clear any pending match interrupts */
    dev(tim)->ICR = chn_isr_cfg[channel].flag;
    if (channel == 0) {
        dev(tim)->TAMATCHR = (timer_config[tim].cfg == GPTMCFG_32_BIT_TIMER) ?
                             value : (LOAD_VALUE_16_BIT - value);
    }
    else {
        dev(tim)->TBMATCHR = (LOAD_VALUE_16_BIT - value);
    }
    dev(tim)->IMR |= chn_isr_cfg[channel].flag;

    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    DEBUG("%s(%u, %u)\n", __FUNCTION__, tim, channel);

    if ((tim >= TIMER_NUMOF) || (channel >= (int)timer_config[tim].chn)) {
        return -1;
    }
    /* clear interrupt flags */
    dev(tim)->IMR &= ~(chn_isr_cfg[channel].flag);

    return 0;
}

/*
 * The timer channels 1 and 2 are configured to run with the same speed and
 * have the same value (they run in parallel), so only on of them is returned.
 */
unsigned int timer_read(tim_t tim)
{
    DEBUG("%s(%u)\n", __FUNCTION__, tim);

    if (tim >= TIMER_NUMOF) {
        return 0;
    }

    if (timer_config[tim].cfg == GPTMCFG_32_BIT_TIMER) {
        return dev(tim)->TAV;
    }
    else {
        return LOAD_VALUE_16_BIT - (dev(tim)->TAV & 0xffff);
    }
}

/*
 * For stopping the counting of all channels.
 */
void timer_stop(tim_t tim)
{
    DEBUG("%s(%u)\n", __FUNCTION__, tim);

    _timer_clock_disable(tim);

    if (tim < TIMER_NUMOF) {
        if (timer_config[tim].chn == 1) {
            dev(tim)->CTL &= ~GPTIMER_CTL_TAEN;
        }
        else if (timer_config[tim].chn == 2) {
            dev(tim)->CTL &= ~(GPTIMER_CTL_TBEN | GPTIMER_CTL_TAEN);
        }
    }

}

void timer_start(tim_t tim)
{
    DEBUG("%s(%u)\n", __FUNCTION__, tim);

    _timer_clock_enable(tim);

    if (tim < TIMER_NUMOF) {
        if (timer_config[tim].chn == 1) {
            dev(tim)->CTL |= GPTIMER_CTL_TAEN;
        }
        else if (timer_config[tim].chn == 2) {
            dev(tim)->CTL |= GPTIMER_CTL_TBEN | GPTIMER_CTL_TAEN;
        }
    }
}

/**
 * @brief   timer interrupt handler
 *
 * @param[in] num   GPT instance number
 * @param[in] chn   channel number (0=A, 1=B)
 */
static void irq_handler(tim_t tim, int channel)
{
    DEBUG("%s(%u,%d)\n", __FUNCTION__, tim, channel);
    assert(tim < TIMER_NUMOF);
    assert(channel < (int)timer_config[tim].chn);

    uint32_t mis;
    /* Latch the active interrupt flags */
    mis = dev(tim)->MIS & chn_isr_cfg[channel].mask;
    /* Clear the latched interrupt flags */
    dev(tim)->ICR = mis;

    if (mis & chn_isr_cfg[channel].flag) {
        /* Disable further match interrupts for this timer/channel */
        dev(tim)->IMR &= ~chn_isr_cfg[channel].flag;
        /* Invoke the callback function */
        isr_ctx[tim].cb(isr_ctx[tim].arg, channel);
    }

    cortexm_isr_end();
}

void isr_timer0_chan0(void)
{
    irq_handler(0, 0);
}
void isr_timer0_chan1(void)
{
    irq_handler(0, 1);
}
void isr_timer1_chan0(void)
{
    irq_handler(1, 0);
}
void isr_timer1_chan1(void)
{
    irq_handler(1, 1);
}
void isr_timer2_chan0(void)
{
    irq_handler(2, 0);
}
void isr_timer2_chan1(void)
{
    irq_handler(2, 1);
}
void isr_timer3_chan0(void)
{
    irq_handler(3, 0);
}
void isr_timer3_chan1(void)
{
    irq_handler(3, 1);
}
