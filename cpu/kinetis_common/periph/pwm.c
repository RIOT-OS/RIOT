/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015-2016 Eistec AB
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

/* FTM channel look up tables */
#if PWM_0_EN
static const uint8_t ftm0chan[] = {
#if PWM_0_CHANNELS > 0
    PWM_0_CH0_FTMCHAN,
#endif
#if PWM_0_CHANNELS > 1
    PWM_0_CH1_FTMCHAN,
#endif
#if PWM_0_CHANNELS > 2
    PWM_0_CH2_FTMCHAN,
#endif
#if PWM_0_CHANNELS > 3
    PWM_0_CH3_FTMCHAN,
#endif
#if PWM_0_CHANNELS > 4
    PWM_0_CH4_FTMCHAN,
#endif
#if PWM_0_CHANNELS > 5
    PWM_0_CH5_FTMCHAN,
#endif
#if PWM_0_CHANNELS > 6
    PWM_0_CH6_FTMCHAN,
#endif
#if PWM_0_CHANNELS > 7
    PWM_0_CH7_FTMCHAN,
#endif
};
#endif
#if PWM_1_EN
static const uint8_t ftm1chan[] = {
#if PWM_1_CHANNELS > 0
    PWM_1_CH0_FTMCHAN,
#endif
#if PWM_1_CHANNELS > 1
    PWM_1_CH1_FTMCHAN,
#endif
#if PWM_1_CHANNELS > 2
    PWM_1_CH2_FTMCHAN,
#endif
#if PWM_1_CHANNELS > 3
    PWM_1_CH3_FTMCHAN,
#endif
#if PWM_1_CHANNELS > 4
    PWM_1_CH4_FTMCHAN,
#endif
#if PWM_1_CHANNELS > 5
    PWM_1_CH5_FTMCHAN,
#endif
#if PWM_1_CHANNELS > 6
    PWM_1_CH6_FTMCHAN,
#endif
#if PWM_1_CHANNELS > 7
    PWM_1_CH7_FTMCHAN,
#endif
};
#endif

uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
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
            return 0;
    }

    switch (mode) {
        case PWM_LEFT:
        case PWM_RIGHT:
        case PWM_CENTER:
            break;

        default:
            return 0;
    }

    if ((unsigned int)res > (PWM_MAX_VALUE + 1) || (res * freq) > pwm_clk) {
        return 0;
    }

    /* Try to find a good prescaler value */
    /* The prescaler divides the module clock by a power of two, between 2^0 and 2^7 */
    uint8_t prescaler = 0;
    /* (resolution * frequency) is the number of timer ticks per second */
    while ((pwm_clk >> prescaler) > (res * freq)) {
        ++prescaler;
        if (prescaler > 7) {
            /* Module clock is too fast to reach the requested frequency using the
             * hardware supported prescaler values */
            /* Note: The frequency might be reachable if the requested resolution
             * is increased. */
            return 0;
        }
    }
    /* The chosen prescaler yields a timer frequency which is the
     * nearest possible frequency less than the requested frequency */

    /* Turn on the peripheral */
    pwm_poweron(dev);

    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            #if PWM_0_CHANNELS > 0
                gpio_init_port(PWM_0_CH0_GPIO, PORT_PCR_MUX(PWM_0_CH0_AF));
            #endif
            #if PWM_0_CHANNELS > 1
                gpio_init_port(PWM_0_CH1_GPIO, PORT_PCR_MUX(PWM_0_CH1_AF));
            #endif
            #if PWM_0_CHANNELS > 2
                gpio_init_port(PWM_0_CH2_GPIO, PORT_PCR_MUX(PWM_0_CH2_AF));
            #endif
            #if PWM_0_CHANNELS > 3
                gpio_init_port(PWM_0_CH3_GPIO, PORT_PCR_MUX(PWM_0_CH3_AF));
            #endif
            #if PWM_0_CHANNELS > 4
                gpio_init_port(PWM_0_CH4_GPIO, PORT_PCR_MUX(PWM_0_CH4_AF));
            #endif
            #if PWM_0_CHANNELS > 5
                gpio_init_port(PWM_0_CH5_GPIO, PORT_PCR_MUX(PWM_0_CH5_AF));
            #endif
            #if PWM_0_CHANNELS > 6
                gpio_init_port(PWM_0_CH6_GPIO, PORT_PCR_MUX(PWM_0_CH6_AF));
            #endif
            #if PWM_0_CHANNELS > 7
                gpio_init_port(PWM_0_CH7_GPIO, PORT_PCR_MUX(PWM_0_CH7_AF));
            #endif
            break;
#endif
#if PWM_1_EN

        case PWM_1:
            #if PWM_1_CHANNELS > 0
                gpio_init_port(PWM_1_CH0_GPIO, PORT_PCR_MUX(PWM_1_CH0_AF));
            #endif
            #if PWM_1_CHANNELS > 1
                gpio_init_port(PWM_1_CH1_GPIO, PORT_PCR_MUX(PWM_1_CH1_AF));
            #endif
            #if PWM_1_CHANNELS > 2
                gpio_init_port(PWM_1_CH2_GPIO, PORT_PCR_MUX(PWM_1_CH2_AF));
            #endif
            #if PWM_1_CHANNELS > 3
                gpio_init_port(PWM_1_CH3_GPIO, PORT_PCR_MUX(PWM_1_CH3_AF));
            #endif
            #if PWM_1_CHANNELS > 4
                gpio_init_port(PWM_1_CH4_GPIO, PORT_PCR_MUX(PWM_1_CH4_AF));
            #endif
            #if PWM_1_CHANNELS > 5
                gpio_init_port(PWM_1_CH5_GPIO, PORT_PCR_MUX(PWM_1_CH5_AF));
            #endif
            #if PWM_1_CHANNELS > 6
                gpio_init_port(PWM_1_CH6_GPIO, PORT_PCR_MUX(PWM_1_CH6_AF));
            #endif
            #if PWM_1_CHANNELS > 7
                gpio_init_port(PWM_1_CH7_GPIO, PORT_PCR_MUX(PWM_1_CH7_AF));
            #endif
            break;
#endif

        default:
            return 0;
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
    ftm->MOD = res - 1;

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
    return (pwm_clk / (1 << prescaler)) / res;
}

uint8_t pwm_channels(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            return PWM_0_CHANNELS;
#endif
#if PWM_1_EN
        case PWM_1:
            return PWM_1_CHANNELS;
#endif
        default:
            return 0;
    }
}

void pwm_set(pwm_t dev, uint8_t channel, uint16_t value)
{
    FTM_Type *ftm;
    const uint8_t *ftmchan = NULL;

    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            if (channel > PWM_0_CHANNELS) {
                return;
            }
            ftm = PWM_0_DEV;
            ftmchan = &ftm0chan[0];
            break;
#endif
#if PWM_1_EN

        case PWM_1:
            if (channel > PWM_1_CHANNELS) {
                return;
            }
            ftm = PWM_1_DEV;
            ftmchan = &ftm1chan[0];
            break;
#endif

        default:
            return;
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
