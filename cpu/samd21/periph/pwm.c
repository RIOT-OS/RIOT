/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
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
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "periph/pwm.h"
#include "periph_conf.h"

/* ignore file in case no PWM devices are defined */
#if PWM_NUMOF

static int round_to_prescaler(unsigned int frequency, unsigned int resolution, int *presc);


int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution)
{
    Tcc* pwm_dev = NULL;
    PortGroup* pwm_port = NULL;

    uint32_t pwm_pins[PWM_MAX_CHANNELS];
    int pwm_clk_id = 0;
    int pwm_clk_ref = 0;
    int channels = 0;

    int presc;
    int presc_num;

    /* round to an available prescaler/clock divider */
    presc_num = round_to_prescaler(frequency, resolution, &presc);

    /* check if frequency and resolution were applicable */
    if (presc_num < 0) {
        return -2;
    }

    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            pwm_dev = PWM_0_DEV;
            pwm_port = &PWM_0_PORT;
            pwm_pins[0] = PWM_0_PIN_CH0;
#if (PWM_0_CHANNELS > 1)
            pwm_pins[1] = PWM_0_PIN_CH1;
#endif
            channels = PWM_0_CHANNELS;
            pwm_clk_id = PWM_0_CLK_ID;
            pwm_clk_ref = PWM_0_REF_F;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            pwm_dev = PWM_1_DEV;
            pwm_port = &PWM_1_PORT;
            pwm_pins[0] = PWM_1_PIN_CH0;
#if (PWM_1_CHANNELS > 1)
            pwm_pins[1] = PWM_1_PIN_CH1;
#endif
            channels = PWM_1_CHANNELS;
            pwm_clk_id = PWM_1_CLK_ID;
            pwm_clk_ref = PWM_1_REF_F;
            break;
#endif
    }

    /* setup pins */
    for (int i = 0; i < channels; i++) {
        pwm_port->DIRSET.reg = (1 << pwm_pins[i]);  /* direction is output */
        /* enable PMUX for pins and set to config E. See spec p. 12 */
        pwm_port->WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG
                                        | PORT_WRCONFIG_WRPMUX
                                        | PORT_WRCONFIG_PMUX(0x4) /* Peripheral function E */
                                        | PORT_WRCONFIG_PMUXEN
                                        | (1 << (pwm_pins[i] % 16));
    }

    /* turn on power manager */
    pwm_poweron(dev);

    /* configure GCLK0 to feed TCC */;
    GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | (pwm_clk_id << GCLK_CLKCTRL_ID_Pos)));
    while (GCLK->STATUS.bit.SYNCBUSY);

    /* disable to configure CTRLA */
    pwm_stop(dev);

    /* reset timer */
    pwm_dev->CTRLA.bit.SWRST = 1;
    while (pwm_dev->CTRLA.bit.SWRST);

    /* reload or reset the counter on next generic clock */
    pwm_dev->CTRLA.bit.PRESCSYNC = TCC_CTRLA_PRESCSYNC_GCLK_Val;
    /* 8MHz  source divided by presc */
    pwm_dev->CTRLA.bit.PRESCALER = presc;

    /* the timer/counter is counting up */
    pwm_dev->CTRLBCLR.reg |= TCC_CTRLBSET_DIR;
    /* reload or reset the counter on next generic clock */
    pwm_dev->CTRLBCLR.reg |= TCC_CTRLBSET_ONESHOT;
    while (pwm_dev->SYNCBUSY.bit.CTRLB);

    /* select the waveform generation operation */
    pwm_dev->WAVE.bit.WAVEGEN = TCC_WAVE_WAVEGEN_NPWM_Val; /* normal PWM */
    while (pwm_dev->SYNCBUSY.bit.WAVE);

    /* the period time is controlled by the PER */
    pwm_dev->PER.bit.PER = (pwm_clk_ref / (presc_num * frequency)) - 1;
    while (pwm_dev->SYNCBUSY.bit.PER);

    /* set PWM mode */
    switch (mode) {
        case PWM_LEFT:
            break;
        case PWM_RIGHT:
            break;
        case PWM_CENTER:
            break;
        //default:
        //    return -1;
    }

    pwm_start(dev);

    return 0;
}

int pwm_set(pwm_t dev, int channel, unsigned int value)
{
    Tcc* pwm_dev = NULL;

    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            pwm_dev = PWM_0_DEV;
            if (channel >= PWM_0_CHANNELS) {
                return -1;
            }
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            pwm_dev = PWM_1_DEV;
            if (channel >= PWM_1_CHANNELS) {
                return -1;
            }
            break;
#endif
    }

    /* norm value to maximum possible value */
    if (value > pwm_dev->PER.bit.PER) {
        value = (unsigned int) pwm_dev->PER.bit.PER;
    }

    switch (channel) {
        case 0:
            pwm_dev->INTFLAG.bit.MC0 = 1;
            pwm_dev->CC[0].bit.CC = value;
            pwm_dev->INTENSET.bit.MC0 = 1;
            break;
        case 1:
            pwm_dev->INTFLAG.bit.MC1 = 1;
            pwm_dev->CC[1].bit.CC = value;
            pwm_dev->INTENSET.bit.MC1 = 1;
            break;
        default:
            return -1;
    }

    return 0;
}

void pwm_start(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PWM_0_DEV->CTRLA.bit.ENABLE = 1;
            while(PWM_0_DEV->SYNCBUSY.bit.ENABLE);
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PWM_1_DEV->CTRLA.bit.ENABLE = 1;
            while(PWM_1_DEV->SYNCBUSY.bit.ENABLE);
            break;
#endif
    }
}

void pwm_stop(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PWM_0_DEV->CTRLA.bit.ENABLE = 0;
            while(PWM_0_DEV->SYNCBUSY.bit.ENABLE);
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PWM_1_DEV->CTRLA.bit.ENABLE = 0;
            while(PWM_1_DEV->SYNCBUSY.bit.ENABLE);
            break;
#endif
    }
}

void pwm_poweron(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PM->APBCMASK.reg |= PWM_0_PM;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PM->APBCMASK.reg |= PWM_1_PM;
            break;
#endif
    }
}

void pwm_poweroff(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PM->APBCMASK.reg &= ~PWM_0_PM;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PM->APBCMASK.reg &= ~PWM_1_PM;
            break;
#endif
    }
}

static int round_to_prescaler(unsigned int frequency, unsigned int resolution, int *presc) {

    uint32_t presc_comp;
    int presc_num;

    presc_comp = (PWM_0_REF_F / ( resolution * frequency)) -1;

    /* this is just an estimation about a max. tolerable rounding error */ 
    if (presc_comp >= 1500) {
        return -2;
    }

    if (presc_comp >= 640) {
        *presc = TCC_CTRLA_PRESCALER_DIV1024_Val;
        presc_num = 1024;
    }
    if (presc_comp < 640) {
        *presc = TCC_CTRLA_PRESCALER_DIV256_Val;
        presc_num = 256;
    }
    if (presc_comp < 160) {
        *presc = TCC_CTRLA_PRESCALER_DIV64_Val;
        presc_num = 64;
    }
    if (presc_comp < 30) {
        *presc = TCC_CTRLA_PRESCALER_DIV16_Val;
        presc_num = 16;
    }
    if (presc_comp < 12) {
        *presc = TCC_CTRLA_PRESCALER_DIV8_Val;
        presc_num = 8;
    }
    if (presc_comp < 6) {
        *presc = TCC_CTRLA_PRESCALER_DIV4_Val;
        presc_num = 4;
    }
    if (presc_comp < 3) {
        *presc = TCC_CTRLA_PRESCALER_DIV2_Val;
        presc_num = 2;
    }
    if (presc_comp <= 1) {
        *presc = TCC_CTRLA_PRESCALER_DIV1_Val;
        presc_num = 1;
    }
    return presc_num;
}

#endif /* PWM_NUMOF */
