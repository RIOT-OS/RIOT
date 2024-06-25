/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *               2022 SSV Software Systems GmbH
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
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * @}
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "pm_layered.h"

#include "periph/timer.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Timer state memory
 */
static timer_isr_ctx_t config[TIMER_NUMOF];

static uint32_t _oneshot;

/* Number of right-shifts to perform on the input frequency to get the output
 * frequency the prescaler will divide it down to */
static const uint8_t _prescaler_shifts[] = {
    [TC_CTRLA_PRESCALER_DIV1_Val] = 0,
    [TC_CTRLA_PRESCALER_DIV2_Val] = 1,
    [TC_CTRLA_PRESCALER_DIV4_Val] = 2,
    [TC_CTRLA_PRESCALER_DIV8_Val] = 3,
    [TC_CTRLA_PRESCALER_DIV16_Val] = 4,
    [TC_CTRLA_PRESCALER_DIV64_Val] = 6,
    [TC_CTRLA_PRESCALER_DIV256_Val] = 8,
    [TC_CTRLA_PRESCALER_DIV1024_Val] = 10,
};

static_assert(ARRAY_SIZE(_prescaler_shifts) == (TC_CTRLA_PRESCALER_DIV1024_Val + 1),
              "_prescaler_shifts needs an update for the selected MCU");

static inline void set_oneshot(tim_t tim, int chan)
{
    _oneshot |= (1 << chan) << (TIMER_CHANNEL_NUMOF * tim);
}

static inline void clear_oneshot(tim_t tim, int chan)
{
    _oneshot &= ~((1 << chan) << (TIMER_CHANNEL_NUMOF * tim));
}

static inline bool is_oneshot(tim_t tim, int chan)
{
    return _oneshot & ((1 << chan) << (TIMER_CHANNEL_NUMOF * tim));
}

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

static uint8_t _get_prescaler(uint32_t freq_out, uint32_t freq_in)
{
    for (uint8_t scale = 0; scale < ARRAY_SIZE(_prescaler_shifts); scale++) {
        if ((freq_in >> _prescaler_shifts[scale]) == freq_out) {
            return scale;
        }
    }

    return UINT8_MAX;
}

/* TOP value is CC0 */
static inline void _set_mfrq(tim_t tim)
{
#ifdef TC_WAVE_WAVEGEN_MFRQ
    dev(tim)->WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;
#else
    uint32_t reg = dev(tim)->CTRLA.reg;
    dev(tim)->CTRLA.reg = ((reg & ~TC_CTRLA_WAVEGEN_Msk) |
                           TC_CTRLA_WAVEGEN(TC_CTRLA_WAVEGEN_MFRQ_Val));
#endif
}

/* TOP value is MAX timer value */
static inline void _set_nfrq(tim_t tim)
{
#ifdef TC_WAVE_WAVEGEN_NFRQ
    dev(tim)->WAVE.reg = TC_WAVE_WAVEGEN_NFRQ;
#else
    uint32_t reg = dev(tim)->CTRLA.reg;
    dev(tim)->CTRLA.reg = ((reg & ~TC_CTRLA_WAVEGEN_Msk) |
                           TC_CTRLA_WAVEGEN(TC_CTRLA_WAVEGEN_NFRQ_Val));
#endif
}

uword_t timer_query_freqs_numof(tim_t dev)
{
    assert(dev < TIMER_NUMOF);
    (void)dev;
    return ARRAY_SIZE(_prescaler_shifts);
}

uint32_t timer_query_freqs(tim_t dev, uword_t index)
{
    assert(dev < TIMER_NUMOF);
    const tc32_conf_t *cfg = &timer_config[dev];
    if (index >= ARRAY_SIZE(_prescaler_shifts)) {
        return 0;
    }
    return sam0_gclk_freq(cfg->gclk_src) >> _prescaler_shifts[index];
}

/**
 * @brief Setup the given timer
 */
int timer_init(tim_t tim, uint32_t freq, timer_cb_t cb, void *arg)
{
    const tc32_conf_t *cfg = &timer_config[tim];
    uint8_t scale = _get_prescaler(freq, sam0_gclk_freq(cfg->gclk_src));

    /* make sure given device is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* make sure the prescaler is within range */
    if (scale > TC_CTRLA_PRESCALER_DIV1024_Val) {
        DEBUG("[timer %d] scale %d is out of range\n", tim, scale);
        return -1;
    }

    sam0_gclk_enable(cfg->gclk_src);
#ifdef MCLK
    GCLK->PCHCTRL[cfg->gclk_id].reg = GCLK_PCHCTRL_GEN(cfg->gclk_src) | GCLK_PCHCTRL_CHEN;
    *cfg->mclk |= cfg->mclk_mask;
#else
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(cfg->gclk_src) | cfg->gclk_ctrl;
    PM->APBCMASK.reg |= cfg->pm_mask;
#endif

    /* make sure the timer is not running */
    timer_stop(tim);

    /* reset the timer */
    dev(tim)->CTRLA.reg |= TC_CTRLA_SWRST;
    while (dev(tim)->CTRLA.reg & TC_CTRLA_SWRST) {}

    dev(tim)->CTRLA.reg = cfg->flags
#ifdef TC_CTRLA_WAVEGEN_NFRQ
                  | TC_CTRLA_WAVEGEN_NFRQ
#endif
                  | TC_CTRLA_PRESCALER(scale)
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

    set_oneshot(tim, channel);

    return 0;
}

int timer_set_periodic(tim_t tim, int channel, unsigned int value, uint8_t flags)
{
    DEBUG("Setting timer %i channel %i to %i (repeating)\n", tim, channel, value);

    timer_stop(tim);

    /* set timeout value */
    switch (channel) {
    case 0:
        /* clear interrupt */
        dev(tim)->INTFLAG.reg = TC_INTFLAG_MC0;

        if (flags & TIM_FLAG_RESET_ON_MATCH) {
            _set_mfrq(tim);
        } else {
            _set_nfrq(tim);
        }

        _set_cc(tim, 0, value);
        dev(tim)->INTENSET.reg = TC_INTENSET_MC0;
        break;
    case 1:

        /* only CC0 can be used to set TOP */
        if (flags & TIM_FLAG_RESET_ON_MATCH) {
            assert(0);
            return -1;
        }

        dev(tim)->INTFLAG.reg = TC_INTFLAG_MC1;
        _set_cc(tim, 1, value);
        dev(tim)->INTENSET.reg = TC_INTENSET_MC1;
        break;
    default:
        return -1;
    }

    if (flags & TIM_FLAG_RESET_ON_SET) {
        dev(tim)->COUNT.reg = 0;
    }

    if (!(flags & TIM_FLAG_SET_STOPPED)) {
        timer_start(tim);
    }

    clear_oneshot(tim, channel);

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
    if (!(dev(tim)->CTRLA.reg & TC_CTRLA_ENABLE)) {
        return 0;
    }

    /* request synchronisation */
#ifdef TC_CTRLBSET_CMD_READSYNC_Val
    uint32_t cmd;
    dev(tim)->CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
     /* work around a possible hardware bug where it takes some
        cycles for the timer peripheral to set the SYNCBUSY/READSYNC bit
        after writing the READSYNC bit

        The problem was observed on SAME54.
      */
    do {
        cmd = ((dev(tim)->CTRLBSET.reg & TC_CTRLBSET_CMD_Msk) >> TC_CTRLBSET_CMD_Pos);
    } while(cmd == TC_CTRLBSET_CMD_READSYNC_Val);

#else
    dev(tim)->READREQ.reg = TC_READREQ_RREQ | TC_READREQ_ADDR(TC_COUNT32_COUNT_OFFSET);
#endif

    wait_synchronization(tim);

    return dev(tim)->COUNT.reg;
}

void timer_stop(tim_t tim)
{
#if IS_ACTIVE(MODULE_PM_LAYERED) && defined(SAM0_TIMER_PM_BLOCK)
    /* unblock power mode if the timer is running */
    if (dev(tim)->CTRLA.reg & TC_CTRLA_ENABLE) {
        DEBUG("[timer %d] pm_unblock\n", tim);
        pm_unblock(SAM0_TIMER_PM_BLOCK);
    }
#endif

    dev(tim)->CTRLA.reg &= ~TC_CTRLA_ENABLE;
    wait_synchronization(tim);
}

void timer_start(tim_t tim)
{
    wait_synchronization(tim);

#if IS_ACTIVE(MODULE_PM_LAYERED) && defined(SAM0_TIMER_PM_BLOCK)
    /* block power mode if the timer is not running, yet */
    if (!(dev(tim)->CTRLA.reg & TC_CTRLA_ENABLE)) {
        DEBUG("[timer %d] pm_block\n", tim);
        pm_block(SAM0_TIMER_PM_BLOCK);
    }
#endif

    dev(tim)->CTRLA.reg |= TC_CTRLA_ENABLE;
}

static inline void timer_isr(tim_t tim)
{
    TcCount32 *tc  = dev(tim);
    uint8_t status = tc->INTFLAG.reg;

    /* Acknowledge all interrupts */
    tc->INTFLAG.reg = status;

    if ((status & TC_INTFLAG_MC0) && (tc->INTENSET.reg & TC_INTENSET_MC0)) {

        if (is_oneshot(tim, 0)) {
            tc->INTENCLR.reg = TC_INTENCLR_MC0;
        }

        if (config[tim].cb) {
            config[tim].cb(config[tim].arg, 0);
        }
    }

    if ((status & TC_INTFLAG_MC1) && (tc->INTENSET.reg & TC_INTENSET_MC1)) {

        if (is_oneshot(tim, 1)) {
            tc->INTENCLR.reg = TC_INTENCLR_MC1;
        }

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
