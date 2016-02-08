/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_pwm
 *
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
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

/* FTM channel look up tables */
#if PWM_0_EN
static const uint8_t ftm0chan[] = {
#if PWM_0_CHANNELS > 0
    PWM_0_FTMCHAN_CH0,
#endif
#if PWM_0_CHANNELS > 1
    PWM_0_FTMCHAN_CH1,
#endif
#if PWM_0_CHANNELS > 2
    PWM_0_FTMCHAN_CH2,
#endif
#if PWM_0_CHANNELS > 3
    PWM_0_FTMCHAN_CH3,
#endif
#if PWM_0_CHANNELS > 4
    PWM_0_FTMCHAN_CH4,
#endif
#if PWM_0_CHANNELS > 5
    PWM_0_FTMCHAN_CH5,
#endif
#if PWM_0_CHANNELS > 6
    PWM_0_FTMCHAN_CH6,
#endif
#if PWM_0_CHANNELS > 7
    PWM_0_FTMCHAN_CH7,
#endif
};
#endif
#if PWM_1_EN
static const uint8_t ftm1chan[] = {
#if PWM_1_CHANNELS > 0
    PWM_1_FTMCHAN_CH0,
#endif
#if PWM_1_CHANNELS > 1
    PWM_1_FTMCHAN_CH1,
#endif
#if PWM_1_CHANNELS > 2
    PWM_1_FTMCHAN_CH2,
#endif
#if PWM_1_CHANNELS > 3
    PWM_1_FTMCHAN_CH3,
#endif
#if PWM_1_CHANNELS > 4
    PWM_1_FTMCHAN_CH4,
#endif
#if PWM_1_CHANNELS > 5
    PWM_1_FTMCHAN_CH5,
#endif
#if PWM_1_CHANNELS > 6
    PWM_1_FTMCHAN_CH6,
#endif
#if PWM_1_CHANNELS > 7
    PWM_1_FTMCHAN_CH7,
#endif
};
#endif

int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution)
{
    FTM_Type *ftm;
    int channels = 0;
    uint32_t pwm_clk = 0;
    const uint8_t *ftmchan = NULL;

    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            channels = PWM_0_CHANNELS;
            pwm_clk = PWM_0_CLK;
            ftm = PWM_0_DEV;
            ftmchan = &ftm0chan[0];
            break;
#endif
#if PWM_1_EN

        case PWM_1:
            channels = PWM_1_CHANNELS;
            pwm_clk = PWM_1_CLK;
            ftm = PWM_1_DEV;
            ftmchan = &ftm1chan[0];
            break;
#endif

        default:
            return -1;
    }

    switch (mode) {
        case PWM_LEFT:
        case PWM_RIGHT:
        case PWM_CENTER:
            break;

        default:
            return -1;
    }

    if (resolution > (PWM_MAX_VALUE + 1) || (resolution * frequency) > pwm_clk) {
        return -2;
    }

    /* Try to find a good prescaler value */
    /* The prescaler divides the module clock by a power of two, between 2^0 and 2^7 */
    uint8_t prescaler = 0;
    /* (resolution * frequency) is the number of timer ticks per second */
    while ((pwm_clk >> prescaler) > (resolution * frequency)) {
        ++prescaler;
        if (prescaler > 7) {
            /* Module clock is too fast to reach the requested frequency using the
             * hardware supported prescaler values */
            /* Note: The frequency might be reachable if the requested resolution
             * is increased. */
            return -2;
        }
    }
    /* The chosen prescaler yields a timer frequency which is the
     * nearest possible frequency less than the requested frequency */

    /* Turn on the peripheral */
    pwm_poweron(dev);

    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            PWM_0_PORT_CLKEN();
            #if PWM_0_CHANNELS > 0
                PWM_0_PORT_CH0->PCR[PWM_0_PIN_CH0] = PORT_PCR_MUX(PWM_0_PIN_AF_CH0);
            #endif
            #if PWM_0_CHANNELS > 1
                PWM_0_PORT_CH1->PCR[PWM_0_PIN_CH1] = PORT_PCR_MUX(PWM_0_PIN_AF_CH1);
            #endif
            #if PWM_0_CHANNELS > 2
                PWM_0_PORT_CH2->PCR[PWM_0_PIN_CH2] = PORT_PCR_MUX(PWM_0_PIN_AF_CH2);
            #endif
            #if PWM_0_CHANNELS > 3
                PWM_0_PORT_CH3->PCR[PWM_0_PIN_CH3] = PORT_PCR_MUX(PWM_0_PIN_AF_CH3);
            #endif
            #if PWM_0_CHANNELS > 4
                PWM_0_PORT_CH4->PCR[PWM_0_PIN_CH4] = PORT_PCR_MUX(PWM_0_PIN_AF_CH4);
            #endif
            #if PWM_0_CHANNELS > 5
                PWM_0_PORT_CH5->PCR[PWM_0_PIN_CH5] = PORT_PCR_MUX(PWM_0_PIN_AF_CH5);
            #endif
            #if PWM_0_CHANNELS > 6
                PWM_0_PORT_CH6->PCR[PWM_0_PIN_CH6] = PORT_PCR_MUX(PWM_0_PIN_AF_CH6);
            #endif
            #if PWM_0_CHANNELS > 7
                PWM_0_PORT_CH7->PCR[PWM_0_PIN_CH7] = PORT_PCR_MUX(PWM_0_PIN_AF_CH7);
            #endif
            break;
#endif
#if PWM_1_EN

        case PWM_1:
            PWM_1_PORT_CLKEN();
            #if PWM_1_CHANNELS > 0
                PWM_1_PORT_CH0->PCR[PWM_1_PIN_CH0] = PORT_PCR_MUX(PWM_1_PIN_AF_CH0);
            #endif
            #if PWM_1_CHANNELS > 1
                PWM_1_PORT_CH1->PCR[PWM_1_PIN_CH1] = PORT_PCR_MUX(PWM_1_PIN_AF_CH1);
            #endif
            #if PWM_1_CHANNELS > 2
                PWM_1_PORT_CH2->PCR[PWM_1_PIN_CH2] = PORT_PCR_MUX(PWM_1_PIN_AF_CH2);
            #endif
            #if PWM_1_CHANNELS > 3
                PWM_1_PORT_CH3->PCR[PWM_1_PIN_CH3] = PORT_PCR_MUX(PWM_1_PIN_AF_CH3);
            #endif
            #if PWM_1_CHANNELS > 4
                PWM_1_PORT_CH4->PCR[PWM_1_PIN_CH4] = PORT_PCR_MUX(PWM_1_PIN_AF_CH4);
            #endif
            #if PWM_1_CHANNELS > 5
                PWM_1_PORT_CH5->PCR[PWM_1_PIN_CH5] = PORT_PCR_MUX(PWM_1_PIN_AF_CH5);
            #endif
            #if PWM_1_CHANNELS > 6
                PWM_1_PORT_CH6->PCR[PWM_1_PIN_CH6] = PORT_PCR_MUX(PWM_1_PIN_AF_CH6);
            #endif
            #if PWM_1_CHANNELS > 7
                PWM_1_PORT_CH7->PCR[PWM_1_PIN_CH7] = PORT_PCR_MUX(PWM_1_PIN_AF_CH7);
            #endif
            break;
#endif

        default:
            return -1;
    }

    /* disable write protect for changing settings */
    ftm->MODE = FTM_MODE_WPDIS_MASK;

    /* reset timer match value */
    for (int i = 0; i < channels; i++) {
        ftm->CONTROLS[i].CnV = 0;
    }

    /* reset timer configuration registers */
    ftm->COMBINE = 0;
    ftm->CNTIN = 0;
    ftm->SWOCTRL = 0;

    /* set prescale and mod registers to matching values for resolution and frequency */
    ftm->SC = FTM_SC_PS(prescaler);
    ftm->MOD = resolution - 1;

    /* set PWM mode */
    uint32_t mode_mask = 0;
    switch (mode) {
        case PWM_LEFT:
            mode_mask = (1 << FTM_CnSC_MSB_SHIFT) | (1 << FTM_CnSC_ELSB_SHIFT);
            break;

        case PWM_RIGHT:
            mode_mask = (1 << FTM_CnSC_MSB_SHIFT) | (1 << FTM_CnSC_ELSA_SHIFT);
            break;

        case PWM_CENTER:
            mode_mask = (1 << FTM_CnSC_MSB_SHIFT);
            ftm->SC |= (1 << FTM_SC_CPWMS_SHIFT);
            break;
    }
    for (int i = 0; i < channels; i++) {
        /* cppcheck thinks ftmchan may be NULL here, but the variable is
         * assigned in all non-returning branches of the switch at the top of
         * this function. */
        /* cppcheck-suppress nullPointer ftmchan */
        ftm->CONTROLS[ftmchan[i]].CnSC = mode_mask;
    }

    /* enable timer ergo the PWM generation */
    pwm_start(dev);

    /* Return actual frequency */
    return (pwm_clk / (1 << prescaler)) / resolution;
}

int pwm_set(pwm_t dev, int channel, unsigned int value)
{
    FTM_Type *ftm;
    const uint8_t *ftmchan = NULL;

    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            if (channel > PWM_0_CHANNELS) {
                return -1;
            }
            ftm = PWM_0_DEV;
            ftmchan = &ftm0chan[0];
            break;
#endif
#if PWM_1_EN

        case PWM_1:
            if (channel > PWM_1_CHANNELS) {
                return -1;
            }
            ftm = PWM_1_DEV;
            ftmchan = &ftm1chan[0];
            break;
#endif

        default:
            return -1;
    }

    /* clamp value to maximum possible value */
    if (value > PWM_MAX_VALUE) {
        value = PWM_MAX_VALUE;
    }

    /* cppcheck thinks ftmchan may be NULL here, but the variable is
     * assigned in all non-returning branches of the switch at the top of
     * this function. */
    /* cppcheck-suppress nullPointer */
    ftm->CONTROLS[ftmchan[channel]].CnV = value;

    return 0;
}

void pwm_start(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            PWM_0_DEV->SC |= FTM_SC_CLKS(1);
            break;
#endif
#if PWM_1_EN

        case PWM_1:
            PWM_1_DEV->SC |= FTM_SC_CLKS(1);
            break;
#endif
    }
}

void pwm_stop(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            PWM_0_DEV->SC &= ~FTM_SC_CLKS_MASK;
            break;
#endif
#if PWM_1_EN

        case PWM_1:
            PWM_1_DEV->SC &= ~FTM_SC_CLKS_MASK;
            break;
#endif
    }
}

void pwm_poweron(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            PWM_0_CLKEN();
            break;
#endif
#if PWM_1_EN

        case PWM_1:
            PWM_1_CLKEN();
            break;
#endif
    }
}

void pwm_poweroff(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            PWM_0_CLKDIS();
            break;
#endif
#if PWM_1_EN

        case PWM_1:
            PWM_1_CLKDIS();
            break;
#endif
    }
}

#endif /* PWM_NUMOF */
