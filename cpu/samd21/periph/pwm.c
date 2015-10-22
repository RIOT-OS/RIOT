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
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "log.h"
#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph/pwm.h"


/* ignore file in case no PWM devices are defined */
#if PWM_NUMOF

static inline int _num(pwm_t dev)
{
    return ((int)(pwm_config[dev].dev) & 0xc00) >> 10;
}

static inline Tcc *_tcc(pwm_t dev)
{
    return pwm_config[dev].dev;
}

static inline uint8_t _chan(pwm_t dev, int chan)
{
    return pwm_config[dev].chan[chan].chan;
}

static int _clk_id(pwm_t dev)
{
    if (_num(dev) == 2) {
        return TCC2_GCLK_ID;
    }
    return TCC0_GCLK_ID;
}

static uint8_t get_prescaler(unsigned int target, int *scale)
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

int pwm_init(pwm_t dev, pwm_mode_t mode,
             unsigned int frequency, unsigned int resolution)
{
    uint8_t prescaler;
    int scale = 1;
    int f_real;

    if (dev >= PWM_NUMOF) {
        return -1;
    }

    /* calculate the closest possible clock presacler */
    prescaler = get_prescaler(F_CPU / (frequency * resolution), &scale);
    if (prescaler == 0xff) {
        return -2;
    }
    f_real = (F_CPU / (scale * resolution));

    /* configure the used pins */
    for (int i = 0; i < PWM_MAX_CHANNELS; i++) {
        gpio_init(pwm_config[dev].chan[i].pin, GPIO_DIR_OUT, GPIO_NOPULL);
        gpio_init_mux(pwm_config[dev].chan[i].pin, pwm_config[dev].chan[i].mux);
    }

    /* power on the device */
    pwm_poweron(dev);

    /* reset TCC module */
    _tcc(dev)->CTRLA.reg = TCC_CTRLA_SWRST;
    while (_tcc(dev)->SYNCBUSY.reg & TCC_SYNCBUSY_SWRST);
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
            return -1;
    }
    while (_tcc(dev)->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB);

    /* configure the TCC device */
    _tcc(dev)->CTRLA.reg = (TCC_CTRLA_PRESCSYNC_GCLK_Val
                            | TCC_CTRLA_PRESCALER(prescaler));
    /* select the waveform generation mode -> normal PWM */
    _tcc(dev)->WAVE.reg = (TCC_WAVE_WAVEGEN_NPWM);
    while (_tcc(dev)->SYNCBUSY.reg & TCC_SYNCBUSY_WAVE);
    /* set the selected period */
    _tcc(dev)->PER.reg = (resolution - 1);
    while (_tcc(dev)->SYNCBUSY.reg & TCC_SYNCBUSY_PER);
    /* start PWM operation */
    pwm_start(dev);
    /* return the actual frequency the PWM is running at */
    return f_real;
}

int pwm_set(pwm_t dev, int channel, unsigned int value)
{
    if (channel >= PWM_MAX_CHANNELS) {
        return -1;
    }
    _tcc(dev)->CC[_chan(dev, channel)].reg = value;
    while (_tcc(dev)->SYNCBUSY.reg & (TCC_SYNCBUSY_CC0 << _chan(dev, channel)));
    return 0;
}

void pwm_start(pwm_t dev)
{
    _tcc(dev)->CTRLA.reg |= (TCC_CTRLA_ENABLE);
}

void pwm_stop(pwm_t dev)
{
    _tcc(dev)->CTRLA.reg &= ~(TCC_CTRLA_ENABLE);
}

void pwm_poweron(pwm_t dev)
{
    int num = _num(dev);
    if (num < 0) {
        return;
    }
    PM->APBCMASK.reg |= (PM_APBCMASK_TCC0 << num);
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN |
                         GCLK_CLKCTRL_GEN_GCLK0 |
                         GCLK_CLKCTRL_ID(_clk_id(dev)));
    while (GCLK->STATUS.bit.SYNCBUSY);
}

void pwm_poweroff(pwm_t dev)
{
    int num = _num(dev);
    if (num < 0) {
        return;
    }
    PM->APBCMASK.reg &= ~(PM_APBCMASK_TCC0 << num);
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_GEN_GCLK7 |
                         GCLK_CLKCTRL_ID(_clk_id(dev)));
    while (GCLK->STATUS.bit.SYNCBUSY);
}

#endif /* PWM_NUMOF */
