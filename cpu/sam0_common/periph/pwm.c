/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_samd21
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

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph/pwm.h"

static inline Tcc *_tcc(pwm_t dev)
{
    return pwm_config[dev].tim.dev;
}

static inline uint8_t _chan(pwm_t dev, int chan)
{
    return pwm_config[dev].chan[chan].chan;
}

static uint8_t _get_prescaler(unsigned int target, int *scale)
{
    if (target == 0) {
        return 0xff;
    }

    if (target >= 512) {
        *scale = 1024;
        return TCC_CTRLA_PRESCALER_DIV1024_Val;
    }
    if (target >= 128) {
        *scale = 256;
        return TCC_CTRLA_PRESCALER_DIV256_Val;
    }
    if (target >= 32) {
        *scale = 64;
        return TCC_CTRLA_PRESCALER_DIV64_Val;
    }
    if (target >= 12) {
        *scale = 16;
        return TCC_CTRLA_PRESCALER_DIV16_Val;
    }
    if (target >= 6) {
        *scale = 8;
        return TCC_CTRLA_PRESCALER_DIV8_Val;
    }
    if (target >= 3) {
        *scale = 4;
        return TCC_CTRLA_PRESCALER_DIV4_Val;
    }
    *scale = target;
    return target - 1;
}

static uint8_t _get_cc_numof(Tcc *tcc)
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
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN_GCLK7
                      | GCLK_CLKCTRL_ID(cfg->tim.gclk_id);
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

    /* reset TCC module */
    _tcc(dev)->CTRLA.reg = TCC_CTRLA_SWRST;
    while (_tcc(dev)->SYNCBUSY.reg & TCC_SYNCBUSY_SWRST) {}

    /* set PWM mode */
    switch (mode) {
        case PWM_LEFT:
            _tcc(dev)->CTRLBCLR.reg = TCC_CTRLBCLR_DIR;     /* count up */
            break;
        case PWM_RIGHT:
            _tcc(dev)->CTRLBSET.reg = TCC_CTRLBSET_DIR;     /* count down */
            break;
        case PWM_CENTER:        /* currently not supported */
        default:
            return 0;
    }
    while (_tcc(dev)->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {}

    /* configure the TCC device */
    _tcc(dev)->CTRLA.reg = TCC_CTRLA_PRESCSYNC_GCLK_Val
                         | TCC_CTRLA_PRESCALER(prescaler);

    /* select the waveform generation mode -> normal PWM */
    _tcc(dev)->WAVE.reg = (TCC_WAVE_WAVEGEN_NPWM);
    while (_tcc(dev)->SYNCBUSY.reg & TCC_SYNCBUSY_WAVE) {}

    /* set the selected period */
    _tcc(dev)->PER.reg = (res - 1);
    while (_tcc(dev)->SYNCBUSY.reg & TCC_SYNCBUSY_PER) {}

    /* start PWM operation */
    _tcc(dev)->CTRLA.reg |= TCC_CTRLA_ENABLE;

    /* return the actual frequency the PWM is running at */
    return f_real;
}

uint8_t pwm_channels(pwm_t dev)
{
    return pwm_config[dev].chan_numof;
}

void pwm_set(pwm_t dev, uint8_t channel, uint16_t value)
{
    if ((channel >= pwm_config[dev].chan_numof) ||
        (pwm_config[dev].chan[channel].pin == GPIO_UNDEF)) {
        return;
    }

    uint8_t chan = _chan(dev, channel);

    /* TODO: use OTMX for pin remapping */
    chan %= _get_cc_numof(_tcc(dev));

    _tcc(dev)->CC[chan].reg = value;
    while (_tcc(dev)->SYNCBUSY.reg & (TCC_SYNCBUSY_CC0 << chan)) {}
}

void pwm_poweron(pwm_t dev)
{
    poweron(dev);
    _tcc(dev)->CTRLA.reg |= (TCC_CTRLA_ENABLE);
}

void pwm_poweroff(pwm_t dev)
{
    _tcc(dev)->CTRLA.reg &= ~(TCC_CTRLA_ENABLE);
    poweroff(dev);
}
