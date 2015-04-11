/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
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
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "periph/pwm.h"
#include "periph_conf.h"

#include "hwtimer.h"

/* ignore file in case no PWM devices are defined */
#if PWM_NUMOF

int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution)
{
    FTM_Type *tim = NULL;
    PORT_Type *port[PWM_MAX_CHANNELS];
    /* cppcheck-suppress unassignedVariable */
    uint8_t pins[PWM_MAX_CHANNELS];
    uint8_t af[PWM_MAX_CHANNELS];
    /* cppcheck-suppress unassignedVariable */
    uint8_t ftmchan[PWM_MAX_CHANNELS];
    int channels = 0;
    uint32_t pwm_clk = 0;

    pwm_poweron(dev);

    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            tim = PWM_0_DEV;
            port[0] = PWM_0_PORT_CH0;
            port[1] = PWM_0_PORT_CH1;
            port[2] = PWM_0_PORT_CH2;
            port[3] = PWM_0_PORT_CH3;
            pins[0] = PWM_0_PIN_CH0;
            pins[1] = PWM_0_PIN_CH1;
            pins[2] = PWM_0_PIN_CH2;
            pins[3] = PWM_0_PIN_CH3;
            ftmchan[0] = PWM_0_FTMCHAN_CH0;
            ftmchan[1] = PWM_0_FTMCHAN_CH1;
            ftmchan[2] = PWM_0_FTMCHAN_CH2;
            ftmchan[3] = PWM_0_FTMCHAN_CH3;
            af[0] = PWM_0_PIN_AF_CH0;
            af[1] = PWM_0_PIN_AF_CH1;
            af[2] = PWM_0_PIN_AF_CH2;
            af[3] = PWM_0_PIN_AF_CH3;
            channels = PWM_0_CHANNELS;
            pwm_clk = PWM_0_CLK;
            PWM_0_PORT_CLKEN();
            break;
#endif

        default:
            return -1;
    }

    if (channels > PWM_MAX_CHANNELS) {
        return -1;
    }

    /* cppcheck-suppress nullPointer */
    tim->MODE = (1 << FTM_MODE_WPDIS_SHIFT);

    /* setup pins, reset timer match value */
    for (int i = 0; i < channels; i++) {
        port[i]->PCR[pins[i]] = PORT_PCR_MUX(af[i]);
        /* cppcheck-suppress nullPointer */
        tim->CONTROLS[i].CnV = 0;
    }

    /* reset timer configuration registers */
    /* cppcheck-suppress nullPointer */
    tim->COMBINE = 0;

    /* set prescale and mod registers to matching values for resolution and frequency */
    if (resolution > 0xffff || (resolution * frequency) > pwm_clk) {
        return -2;
    }

    /* cppcheck-suppress nullPointer */
    tim->SC = FTM_SC_PS((pwm_clk / (resolution * frequency)) - 1);
    /* cppcheck-suppress nullPointer */
    tim->MOD = resolution;

    /* set PWM mode */
    switch (mode) {
        case PWM_LEFT:
            for (int i = 0; i < channels; i++) {
                /* cppcheck-suppress nullPointer */
                tim->CONTROLS[ftmchan[i]].CnSC = (1 << FTM_CnSC_MSB_SHIFT |
                                                  1 << FTM_CnSC_ELSB_SHIFT);
            }

            break;

        case PWM_RIGHT:
            for (int i = 0; i < channels; i++) {
                /* cppcheck-suppress nullPointer */
                tim->CONTROLS[ftmchan[i]].CnSC = (1 << FTM_CnSC_MSB_SHIFT |
                                                  1 << FTM_CnSC_ELSA_SHIFT);
            }

            break;

        case PWM_CENTER:
            for (int i = 0; i < channels; i++) {
                /* cppcheck-suppress nullPointer */
                tim->CONTROLS[ftmchan[i]].CnSC = (1 << FTM_CnSC_MSB_SHIFT);
            }

            /* cppcheck-suppress nullPointer */
            tim->SC |= (1 << FTM_SC_CPWMS_SHIFT);
            break;
    }

    /* enable timer ergo the PWM generation */
    pwm_start(dev);

    return 0;
}

int pwm_set(pwm_t dev, int channel, unsigned int value)
{
    FTM_Type *tim = NULL;

    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            tim = PWM_0_DEV;
            break;
#endif
#if PWM_1_EN

        case PWM_1:
            tim = PWM_1_DEV;
            break;
#endif

        default:
            return -1;
    }

    /* norm value to maximum possible value */
    if (value > 0xffff) {
        value = 0xffff;
    }

    switch (channel) {
        case 0:
            /* cppcheck-suppress nullPointer */
            tim->CONTROLS[PWM_0_FTMCHAN_CH0].CnV = value;
            break;

        case 1:
            /* cppcheck-suppress nullPointer */
            tim->CONTROLS[PWM_0_FTMCHAN_CH1].CnV = value;
            break;

        case 2:
            /* cppcheck-suppress nullPointer */
            tim->CONTROLS[PWM_0_FTMCHAN_CH2].CnV = value;
            break;

        case 3:
            /* cppcheck-suppress nullPointer */
            tim->CONTROLS[PWM_0_FTMCHAN_CH3].CnV = value;
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
