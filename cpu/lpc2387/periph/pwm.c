/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     lpc2387
 * @{
 *
 * @file
 * @brief       CPU specific low-level PWM driver implementation for the LPC2387
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "bitarithm.h"
#include "lpc2387.h"
#include "periph/pwm.h"
#include "periph_conf.h"


#define PCPWM1      BIT6

/**
 * @note The PWM is always initialized with left-aligned mode.
 *
 * TODO: add center and right aligned modes
 */
int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            /* select function PWM[3] for pins */
            PWM_0_PORT &= ~((3 << PWM_0_CH0_PIN * 2) |
                            (3 << PWM_0_CH1_PIN * 2) |
                            (3 << PWM_0_CH2_PIN * 2));
            PWM_0_PORT |= (PWM_0_FUNC << PWM_0_CH0_PIN * 2) |
                          (PWM_0_FUNC << PWM_0_CH1_PIN * 2) |
                          (PWM_0_FUNC << PWM_0_CH2_PIN * 2);

            /* power on PWM1 */
            PCONP |= BIT6;

            /* select PWM1 clock */
            PCLKSEL0 &= ~(BIT13);
            PCLKSEL0 |= (BIT12);

            /* reset PWM1s counter */
            PWM1TCR = BIT1;

            /* set prescaler */
            PWM1PR = (F_CPU / (frequency * resolution)) - 1;

            /* set match register */
            PWM1MR0 = resolution;
            PWM_0_CH0_MR = 0;
            PWM_0_CH1_MR = 0;
            PWM_0_CH2_MR = 0;

            /* reset timer counter on MR0 match */
            PWM1MCR = BIT1;

            /* enable PWM1 channel 3, 4 and 5 */
            PWM1PCR = (1 << (8 + PWM_0_CH0)) | (1 << (8 + PWM_0_CH1)) | (1 << (8 + PWM_0_CH2));

            /* enable PWM1 timer in PWM mode */
            PWM1TCR = BIT0 + BIT3;

            /* update match registers */
            PWM1LER = BIT0 | (1 << PWM_0_CH0) | (1 << PWM_0_CH1) | (1 << PWM_0_CH2);
            break;
#endif
        case PWM_UNDEFINED:
        default:
            return -1;
    }

    return 0;
}

int pwm_set(pwm_t dev, int channel, unsigned int value)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            switch (channel) {
                case 0:
                    PWM_0_CH0_MR = value;
                    PWM1LER |= (1 << PWM_0_CH0);
                    break;
                case 1:
                    PWM_0_CH1_MR = value;
                    PWM1LER |= (1 << PWM_0_CH1);
                    break;
                case 2:
                    PWM_0_CH2_MR = value;
                    PWM1LER |= (1 << PWM_0_CH2);
                    break;
                default:
                    return -2;
                    break;
            }
            break;
#endif
        case PWM_UNDEFINED:
        default:
            return -1;
    }

    return 0;
}

int pwm_start(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PCONP |= PCPWM1;        /* enable PWM1 device */
            break;
#endif
        case PWM_UNDEFINED:
        default:
            return -1;
    }

    return 0;
}

int pwm_stop(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PCONP &= ~PCPWM1;       /* disable PWM1 device */
            break;
#endif
        case PWM_UNDEFINED:
        default:
            return -1;
    }

    return 0;
}
