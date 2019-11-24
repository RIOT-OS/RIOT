/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file        timer.c
 * @brief       Low-level timer driver implementation
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"

#include "periph/timer.h"
#include "periph_conf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief Timer state memory
 */
static timer_isr_ctx_t config[TIMER_NUMOF];

static inline TcCount32 *dev(tim_t tim)
{
    return &timer_config[tim].dev->COUNT32;
}

static inline TcCount16 *dev16(tim_t tim)
{
    return &timer_config[tim].dev->COUNT16;
}

static inline TcCount8 *dev8(tim_t tim)
{
    return &timer_config[tim].dev->COUNT8;
}

static inline void wait_synchronization(tim_t tim)
{
#if defined(TC_SYNCBUSY_MASK)
    /* SYNCBUSY is a register */
    while ((dev(tim)->SYNCBUSY.reg) != 0) {}
#elif defined(TC_STATUS_SYNCBUSY)
    /* SYNCBUSY is a bit */
    while ((dev(tim)->STATUS.reg & TC_STATUS_SYNCBUSY) != 0) {}
#else
#error Unsupported device
#endif
}

/* enable timer interrupts */
static inline void _irq_enable(tim_t tim)
{
    NVIC_EnableIRQ(timer_config[tim].irq);
}

/**
 * @brief Setup the given timer
 */
int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    (void) freq;
    const tc32_conf_t *cfg = &timer_config[tim];

    /* make sure given device is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* make sure the timer is not running */
    timer_stop(tim);

#ifdef MCLK
    GCLK->PCHCTRL[cfg->gclk_id].reg = cfg->gclk_src | GCLK_PCHCTRL_CHEN;
    *cfg->mclk |= cfg->mclk_mask;
#else
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | cfg->gclk_src | cfg->gclk_ctrl;
    PM->APBCMASK.reg |= cfg->pm_mask;
#endif

    /* reset the timer */
    dev(tim)->CTRLA.bit.SWRST = 1;
    while (dev(tim)->CTRLA.bit.SWRST) {}

    dev(tim)->CTRLA.reg = cfg->flags
#ifdef TC_CTRLA_WAVEGEN_NFRQ
                  | TC_CTRLA_WAVEGEN_NFRQ
#endif
                  | cfg->prescaler
                  | TC_CTRLA_PRESCSYNC_RESYNC;

#ifdef TC_WAVE_WAVEGEN_NFRQ
    dev(tim)->WAVE.reg = TC_WAVE_WAVEGEN_NFRQ;
#endif

    wait_synchronization(tim);

    dev(tim)->INTENCLR.reg = TC_INTENCLR_MASK;

    /* save callback */
    config[tim].cb = cb;
    config[tim].arg = arg;

    timer_start(tim);

    /* enable interrupts for given timer */
    _irq_enable(tim);

    return 0;
}

static void _set_cc(tim_t tim, int cc, unsigned int value)
{
    const uint16_t flags = timer_config[tim].flags;

    if (flags & TC_CTRLA_MODE_COUNT32) {
        dev(tim)->CC[cc].reg = value;
        return;
    }

    if (flags & TC_CTRLA_MODE_COUNT8) {
        dev8(tim)->CC[cc].reg = value;
        return;
    }

    /* 16 bit is the default */
    dev16(tim)->CC[cc].reg = value;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    DEBUG("Setting timer %i channel %i to %i\n", tim, channel, value);

    /* set timeout value */
    switch (channel) {
    case 0:
        dev(tim)->INTFLAG.reg = TC_INTFLAG_MC0;
        _set_cc(tim, 0, value);
        dev(tim)->INTENSET.reg = TC_INTENSET_MC0;
        break;
    case 1:
        dev(tim)->INTFLAG.reg = TC_INTFLAG_MC1;
        _set_cc(tim, 1, value);
        dev(tim)->INTENSET.reg = TC_INTENSET_MC1;
        break;
    default:
        return -1;
     }

    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    switch (channel) {
    case 0:
        dev(tim)->INTFLAG.reg = TC_INTFLAG_MC0;
        dev(tim)->INTENCLR.reg = TC_INTENCLR_MC0;
        break;
    case 1:
        dev(tim)->INTFLAG.reg = TC_INTFLAG_MC1;
        dev(tim)->INTENCLR.reg = TC_INTENCLR_MC1;
        break;
    default:
        return -1;
    }

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    /* WORKAROUND to prevent being stuck there if timer not init */
    if (!dev(tim)->CTRLA.bit.ENABLE) {
        return 0;
    }

    /* request syncronisation */
#ifdef TC_CTRLBSET_CMD_READSYNC_Val
    dev(tim)->CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
     /* work around a possible hardware bug where it takes some
        cycles for the timer peripheral to set the SYNCBUSY/READSYNC bit
        after writing the READSYNC bit

        The problem was observed on SAME54.
      */
    while(dev(tim)->CTRLBSET.bit.CMD == TC_CTRLBSET_CMD_READSYNC_Val) {}
#else
    dev(tim)->READREQ.reg = TC_READREQ_RREQ | TC_READREQ_ADDR(TC_COUNT32_COUNT_OFFSET);
#endif

    wait_synchronization(tim);

    return dev(tim)->COUNT.reg;
}

void timer_stop(tim_t tim)
{
    dev(tim)->CTRLA.bit.ENABLE = 0;
}

void timer_start(tim_t tim)
{
    dev(tim)->CTRLA.bit.ENABLE = 1;
}

static inline void timer_isr(tim_t tim)
{
    TcCount32 *tc  = dev(tim);
    uint8_t status = tc->INTFLAG.reg;

    /* Acknowledge all interrupts */
    tc->INTFLAG.reg = status;

    if ((status & TC_INTFLAG_MC0) && tc->INTENSET.bit.MC0) {
        tc->INTENCLR.reg = TC_INTENCLR_MC0;
        if (config[tim].cb) {
            config[tim].cb(config[tim].arg, 0);
        }
    }
    if ((status & TC_INTFLAG_MC1) && tc->INTENSET.bit.MC1) {
        tc->INTENCLR.reg = TC_INTENCLR_MC1;
        if (config[tim].cb) {
            config[tim].cb(config[tim].arg, 1);
        }
    }
}

#ifdef TIMER_0_ISR
void TIMER_0_ISR(void)
{
    timer_isr(0);
    cortexm_isr_end();
}
#endif
#ifdef TIMER_1_ISR
void TIMER_1_ISR(void)
{
    timer_isr(1);
    cortexm_isr_end();
}
#endif
#ifdef TIMER_2_ISR
void TIMER_2_ISR(void)
{
    timer_isr(2);
    cortexm_isr_end();
}
#endif
#ifdef TIMER_3_ISR
void TIMER_3_ISR(void)
{
    timer_isr(3);
    cortexm_isr_end();
}
#endif
