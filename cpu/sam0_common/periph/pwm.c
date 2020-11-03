/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_pwm
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph/pwm.h"

/* default to using TCC if nothing else is defined */
#if !defined(PWM_USE_TC) && !defined(PWM_USE_TCC) && defined(REV_TCC)
#define PWM_USE_TCC 1
/* use TC if no TCC is available */
#elif !defined(PWM_USE_TC) && !defined(PWM_USE_TCC) && defined(REV_TC)
#define PWM_USE_TC  1
#endif

/* dummy defines to not litter the code with ifdefs if no TCC is available */
#ifndef REV_TCC
typedef TcCount8 Tcc;
#define TCC_SYNCBUSY_CC0 TC_SYNCBUSY_CC0
#define TCC_CTRLA_ENABLE TC_CTRLA_ENABLE
#endif

static inline Tc *_tc(pwm_t dev)
{
#ifdef REV_TC
    return pwm_config[dev].tim.dev.tc;
#else
    (void) dev;
    return NULL;
#endif
}

static inline Tcc *_tcc(pwm_t dev)
{
#ifdef REV_TCC
    return pwm_config[dev].tim.dev.tcc;
#else
    (void) dev;
    return NULL;
#endif
}

static inline uint8_t _chan(pwm_t dev, int chan)
{
    return pwm_config[dev].chan[chan].chan;
}

static inline bool _use_tc(pwm_t dev)
{
    return IS_ACTIVE(PWM_USE_TC) &&
           pwm_config[dev].tim.type == TIMER_TYPE_TC;
}

static inline bool _use_tcc(pwm_t dev)
{
    return IS_ACTIVE(PWM_USE_TCC) &&
           pwm_config[dev].tim.type == TIMER_TYPE_TCC;
}

static inline void _check_defines(void)
{
#ifdef TCC_CTRLA_PRESCALER_DIV1024_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV1024_Val == TC_CTRLA_PRESCALER_DIV1024_Val,
                  "TCC and TC prescaler not equal");
#endif
#ifdef TCC_CTRLA_PRESCALER_DIV256_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV256_Val == TC_CTRLA_PRESCALER_DIV256_Val,
                  "TCC and TC prescaler not equal");
#endif
#ifdef TCC_CTRLA_PRESCALER_DIV64_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV64_Val == TC_CTRLA_PRESCALER_DIV64_Val,
                  "TCC and TC prescaler not equal");
#endif
#ifdef TCC_CTRLA_PRESCALER_DIV16_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV16_Val == TC_CTRLA_PRESCALER_DIV16_Val,
                  "TCC and TC prescaler not equal");
#endif
#ifdef TCC_CTRLA_PRESCALER_DIV8_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV8_Val == TC_CTRLA_PRESCALER_DIV8_Val,
                  "TCC and TC prescaler not equal");
#endif
#ifdef TCC_CTRLA_PRESCALER_DIV4_Val
    static_assert(TCC_CTRLA_PRESCALER_DIV4_Val == TC_CTRLA_PRESCALER_DIV4_Val,
                  "TCC and TC prescaler not equal");
#endif
}

static uint8_t _get_prescaler(unsigned int target, int *scale)
{
    _check_defines();

    if (target == 0) {
        return 0xff;
    }

    if (target >= 512) {
        *scale = 1024;
        return TC_CTRLA_PRESCALER_DIV1024_Val;
    }
    if (target >= 128) {
        *scale = 256;
        return TC_CTRLA_PRESCALER_DIV256_Val;
    }
    if (target >= 32) {
        *scale = 64;
        return TC_CTRLA_PRESCALER_DIV64_Val;
    }
    if (target >= 12) {
        *scale = 16;
        return TC_CTRLA_PRESCALER_DIV16_Val;
    }
    if (target >= 6) {
        *scale = 8;
        return TC_CTRLA_PRESCALER_DIV8_Val;
    }
    if (target >= 3) {
        *scale = 4;
        return TC_CTRLA_PRESCALER_DIV4_Val;
    }

    *scale = target;
    return target - 1;
}

static uint8_t _tcc_get_cc_numof(Tcc *tcc)
{
    switch ((uintptr_t) tcc) {
#ifdef TCC0_CC_NUM
    case (uintptr_t)TCC0:
        return TCC0_CC_NUM;
#endif
#ifdef TCC1_CC_NUM
    case (uintptr_t)TCC1:
        return TCC1_CC_NUM;
#endif
#ifdef TCC2_CC_NUM
    case (uintptr_t)TCC2:
        return TCC2_CC_NUM;
#endif
#ifdef TCC3_CC_NUM
    case (uintptr_t)TCC3:
        return TCC3_CC_NUM;
#endif
#ifdef TCC4_CC_NUM
    case (uintptr_t)TCC4:
        return TCC4_CC_NUM;
#endif
#ifdef TCC5_CC_NUM
    case (uintptr_t)TCC5:
        return TCC5_CC_NUM;
#endif
    }

    assert(0);
    return 0;
}

static void poweron(pwm_t dev)
{
    const pwm_conf_t *cfg = &pwm_config[dev];

    sam0_gclk_enable(cfg->gclk_src);
#ifdef MCLK
    GCLK->PCHCTRL[cfg->tim.gclk_id].reg = GCLK_PCHCTRL_GEN(cfg->gclk_src)
                                        | GCLK_PCHCTRL_CHEN;
    *cfg->tim.mclk |= cfg->tim.mclk_mask;
#else
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN
                      | GCLK_CLKCTRL_GEN(cfg->gclk_src)
                      | GCLK_CLKCTRL_ID(cfg->tim.gclk_id);
    PM->APBCMASK.reg |= cfg->tim.pm_mask;
#endif
}

static void poweroff(pwm_t dev)
{
    const pwm_conf_t *cfg = &pwm_config[dev];

#ifdef MCLK
    GCLK->PCHCTRL[cfg->tim.gclk_id].reg = 0;
    *cfg->tim.mclk &= ~cfg->tim.mclk_mask;
#else
    PM->APBCMASK.reg &= ~cfg->tim.pm_mask;
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN(SAM0_GCLK_DISABLED)
                      | GCLK_CLKCTRL_ID(cfg->tim.gclk_id);
#endif
}

static void _tc_init(Tc *tc, pwm_mode_t mode, uint8_t prescaler, uint8_t res)
{
    /* reset TC module */
    tc->COUNT8.CTRLA.bit.SWRST = 1;
    while (tc->COUNT8.CTRLA.bit.SWRST) {}

    /* set PWM mode */
    switch (mode) {
        case PWM_LEFT:
            tc->COUNT8.CTRLBCLR.reg = TC_CTRLBCLR_DIR;     /* count up */
            break;
        case PWM_RIGHT:
            tc->COUNT8.CTRLBSET.reg = TC_CTRLBSET_DIR;     /* count down */
            break;
        case PWM_CENTER:        /* currently not supported */
        default:
            assert(0);
            return;
    }

    /* configure the TC device */
    tc->COUNT8.CTRLA.reg = TC_CTRLA_PRESCSYNC_GCLK_Val
                          | TC_CTRLA_PRESCALER(prescaler)
                          | TC_CTRLA_MODE_COUNT8
#ifdef TC_CTRLA_WAVEGEN_NPWM
                          | TC_CTRLA_WAVEGEN_NPWM;
#else
                          ;

    /* select the waveform generation mode -> normal PWM */
    tc->COUNT8.WAVE.reg = (TC_WAVE_WAVEGEN_NPWM);
#endif

    /* set the selected period */
    tc->COUNT8.PER.reg = (res - 1);

#ifdef TC_STATUS_SYNCBUSY
    while (tc->COUNT8.STATUS.bit.SYNCBUSY) {}
#else
    while (tc->COUNT8.SYNCBUSY.reg) {}
#endif

    /* start PWM operation */
    tc->COUNT8.CTRLA.reg |= TC_CTRLA_ENABLE;
}

static void _tcc_init(Tcc *tcc, pwm_mode_t mode, uint8_t prescaler, uint16_t res)
{
#ifndef REV_TCC
    (void) tcc;
    (void) mode;
    (void) prescaler;
    (void) res;
#else
    /* reset TCC module */
    tcc->CTRLA.reg = TCC_CTRLA_SWRST;
    while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_SWRST) {}

    /* set PWM mode */
    switch (mode) {
        case PWM_LEFT:
            tcc->CTRLBCLR.reg = TCC_CTRLBCLR_DIR;     /* count up */
            break;
        case PWM_RIGHT:
            tcc->CTRLBSET.reg = TCC_CTRLBSET_DIR;     /* count down */
            break;
        case PWM_CENTER:        /* currently not supported */
        default:
            assert(0);
            return;
    }
    while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {}

    /* configure the TCC device */
    tcc->CTRLA.reg = TCC_CTRLA_PRESCSYNC_GCLK_Val
                   | TCC_CTRLA_PRESCALER(prescaler);

    /* select the waveform generation mode -> normal PWM */
    tcc->WAVE.reg = (TCC_WAVE_WAVEGEN_NPWM);
    while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_WAVE) {}

    /* set the selected period */
    tcc->PER.reg = (res - 1);
    while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_PER) {}

    /* start PWM operation */
    tcc->CTRLA.reg |= TCC_CTRLA_ENABLE;
#endif
}

uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    uint8_t prescaler;
    int scale = 1;
    uint32_t f_real;

    if ((unsigned int)dev >= PWM_NUMOF) {
        return 0;
    }

    /* TC only supports Period in 8-bit mode */
    if (pwm_config[dev].tim.type == TIMER_TYPE_TC && res > 255) {
        res = 0xff;
    }

    const uint32_t f_src = sam0_gclk_freq(pwm_config[dev].gclk_src);

    /* calculate the closest possible clock presacler */
    prescaler = _get_prescaler(f_src / (freq * res), &scale);
    if (prescaler == 0xff) {
        return 0;
    }
    f_real = f_src / (scale * res);

    /* configure the used pins */
    for (unsigned i = 0; i < pwm_config[dev].chan_numof; i++) {
        if (pwm_config[dev].chan[i].pin != GPIO_UNDEF) {
            gpio_init(pwm_config[dev].chan[i].pin, GPIO_OUT);
            gpio_init_mux(pwm_config[dev].chan[i].pin, pwm_config[dev].chan[i].mux);
        }
    }

    /* power on the device */
    poweron(dev);

    if (_use_tc(dev)) {
        _tc_init(_tc(dev), mode, prescaler, res);
    }
    else if (_use_tcc(dev)) {
        _tcc_init(_tcc(dev), mode, prescaler, res);
    }

    /* return the actual frequency the PWM is running at */
    return f_real;
}

uint8_t pwm_channels(pwm_t dev)
{
    return pwm_config[dev].chan_numof;
}

static void _tc_set(Tc *tc, uint8_t chan, uint16_t value)
{
    /* TC only ever has two channels */
    chan %= 2;
    tc->COUNT8.CC[chan].reg = value;

#ifdef TC_STATUS_SYNCBUSY
    while (tc->COUNT8.STATUS.bit.SYNCBUSY) {}
#else
    while (tc->COUNT8.SYNCBUSY.reg & (TC_SYNCBUSY_CC0 << chan)) {}
#endif
}

static void _tcc_set(Tcc *tcc, uint8_t chan, uint16_t value)
{
    /* TODO: use OTMX for pin remapping */
    chan %= _tcc_get_cc_numof(tcc);
    tcc->CC[chan].reg = value;
    while (tcc->SYNCBUSY.reg & (TCC_SYNCBUSY_CC0 << chan)) {}
}

void pwm_set(pwm_t dev, uint8_t channel, uint16_t value)
{
    if ((channel >= pwm_config[dev].chan_numof) ||
        (pwm_config[dev].chan[channel].pin == GPIO_UNDEF)) {
        return;
    }

    if (_use_tc(dev)) {
        _tc_set(_tc(dev), _chan(dev, channel), value);
    }
    else if (_use_tcc(dev)) {
        _tcc_set(_tcc(dev), _chan(dev, channel), value);
    }
}

void pwm_poweron(pwm_t dev)
{
    poweron(dev);

    if (_use_tc(dev)) {
        _tc(dev)->COUNT8.CTRLA.reg |= (TC_CTRLA_ENABLE);
    }
    else if (_use_tcc(dev)) {
        _tcc(dev)->CTRLA.reg |= (TCC_CTRLA_ENABLE);
    }
}

void pwm_poweroff(pwm_t dev)
{
    if (_use_tc(dev)) {
        _tc(dev)->COUNT8.CTRLA.reg &= ~(TC_CTRLA_ENABLE);
    }
    else if (_use_tcc(dev)) {
        _tcc(dev)->CTRLA.reg &= ~(TCC_CTRLA_ENABLE);
    }

    poweroff(dev);
}
