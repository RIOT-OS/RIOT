/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_lpc23xx
 * @ingroup     drivers_periph_pwm
 * @{
 *
 * @file
 * @brief       CPU specific low-level PWM driver implementation for the LPC23XX family
 *
 * The current implementation is somewhat limited: it supports only a single
 * hardware PWM device (fixed to PWM1) and it is fixed on supporting 3 channels.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"
#include "bitarithm.h"
#include "periph/pwm.h"

/**
 * @note The PWM is always initialized with left-aligned mode.
 *
 * TODO: add center and right aligned modes
 */
uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    if ((dev != PWM_DEV(0)) && (mode != PWM_LEFT)) {
        return 0;
    }

    /* select function PWM[3] for pins */
    PWM_PORT &= ~((3 << PWM_CH0_PIN * 2) |
                  (3 << PWM_CH1_PIN * 2) |
                  (3 << PWM_CH2_PIN * 2));
    PWM_PORT |= (PWM_FUNC << PWM_CH0_PIN * 2) |
                (PWM_FUNC << PWM_CH1_PIN * 2) |
                (PWM_FUNC << PWM_CH2_PIN * 2);

    /* power on PWM1 */
    PCONP |= PCPWM1;

    /* select PWM1 clock */
    PCLKSEL0 &= ~(BIT13);
    PCLKSEL0 |= (BIT12);

    /* reset PWM1s counter */
    PWM1TCR = BIT1;

    /* set prescaler */
    PWM1PR = (CLOCK_CORECLOCK / (freq * res)) - 1;

    /* set match register */
    PWM1MR0 = res;
    PWM_CH0_MR = 0;
    PWM_CH1_MR = 0;
    PWM_CH2_MR = 0;

    /* reset timer counter on MR0 match */
    PWM1MCR = BIT1;

    /* enable PWM1 channel 3, 4 and 5 */
    PWM1PCR = (1 << (8 + PWM_CH0)) | (1 << (8 + PWM_CH1)) | (1 << (8 + PWM_CH2));

    /* enable PWM1 timer in PWM mode */
    PWM1TCR = BIT0 + BIT3;

    /* update match registers */
    PWM1LER = BIT0 | (1 << PWM_CH0) | (1 << PWM_CH1) | (1 << PWM_CH2);

    return freq;
}

uint8_t pwm_channels(pwm_t dev)
{
    (void)dev;
    assert(dev == PWM_DEV(0));
    return PWM_CHANNELS;
}

void pwm_set(pwm_t dev, uint8_t channel, uint16_t value)
{
    (void)dev;
    assert((dev == PWM_DEV(0)) && (channel < 3));

    switch (channel) {
        case 0:
            PWM_CH0_MR = value;
            PWM1LER |= (1 << PWM_CH0);
            break;
        case 1:
            PWM_CH1_MR = value;
            PWM1LER |= (1 << PWM_CH1);
            break;
        case 2:
            PWM_CH2_MR = value;
            PWM1LER |= (1 << PWM_CH2);
            break;
    }
}

void pwm_poweron(pwm_t dev)
{
    (void)dev;
    assert(dev == PWM_DEV(0));
    PCONP |= PCPWM1;
    PWM1TCR |= BIT0;
}

void pwm_poweroff(pwm_t dev)
{
    (void)dev;
    assert(dev == PWM_DEV(0));
    PWM1TCR &= ~(BIT0);
    PCONP &= ~(PCPWM1);
}
