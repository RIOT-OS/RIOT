/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     lpc11u34
 * @{
 *
 * @file
 * @brief       CPU specific low-level PWM driver implementation for LPC11U34
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 *
 * @}
 */

#include "bitarithm.h"
#include "periph/gpio.h"
#include "periph/pwm.h"
#include "board.h"
#include "periph_conf.h"

/* guard file in case no PWM device is defined */
#if (PWM_0_EN || PWM_1_EN)

/**
 * @note    The LPC11U34 doesn't support centerized alignements
 */
int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            /* This CPU doesn't support a centerized alignement */
            if (mode == PWM_CENTER) {
                return -1;
            }
            /* Check if the frequency and resolution is applicable */
            if (F_CPU/(resolution*frequency) <= 0) {
                return -2;
            }
#if PWM_0_CH0_EN
            PWM_0_CH0_IOCON = (PWM_0_CH0_IOCON & ~(BIT7 | 7)) | PWM_0_CH0_AF;
#endif
#if PWM_0_CH1_EN
            PWM_0_CH1_IOCON = (PWM_0_CH1_IOCON & ~(BIT7 | 7)) | PWM_0_CH1_AF;
#endif
#if PWM_0_CH2_EN
            PWM_0_CH2_IOCON = (PWM_0_CH2_IOCON & ~(BIT7 | 7)) | PWM_0_CH2_AF;
#endif
            /* The configuration involve that the peripheral is powered */
            pwm_poweron(dev);
            /* Enable timer and keep it in reset state */
            PWM_0_DEV->TCR = BIT0 | BIT1;
            /* Set the prescaler (F_CPU / resolution) */
            PWM_0_DEV->PR = (F_CPU/(resolution*frequency));
            /* Reset timer on MR3 */
            PWM_0_DEV->MCR = BIT10;

            /* Set PWM period */
            PWM_0_DEV->MR0 = (resolution);
            PWM_0_DEV->MR1 = (resolution);
            PWM_0_DEV->MR2 = (resolution);
            PWM_0_DEV->MR3 = (resolution)-1;

            /* Set mode for channels 0..2 */
            PWM_0_DEV->EMR |= ((mode+1) << 4);
            PWM_0_DEV->EMR |= ((mode+1) << 6);
            PWM_0_DEV->EMR |= ((mode+1) << 8);

            /* Enable PWM channels 0..2 */
            PWM_0_DEV->PWMC = BIT0 | BIT1 | BIT2;
#endif /* PWM_0_EN */
#if PWM_1_EN
        case PWM_1:
            /* This CPU doesn't support a centerized alignement */
            if (mode == PWM_CENTER) {
                return -1;
            }
            /* Check if the frequency and resolution is applicable */
            if (F_CPU/(resolution*frequency) <= 0) {
                return -2;
            }
#if PWM_1_CH0_EN
            PWM_1_CH0_IOCON = (PWM_1_CH0_IOCON & ~(BIT7 | 7)) | PWM_1_CH0_AF;
#endif
#if PWM_1_CH1_EN
            PWM_1_CH1_IOCON = (PWM_1_CH1_IOCON & ~(BIT7 | 7)) | PWM_1_CH1_AF;
#endif
#if PWM_1_CH2_EN
            PWM_1_CH2_IOCON = (PWM_1_CH2_IOCON & ~(BIT7 | 7)) | PWM_1_CH2_AF;
#endif
            /* The configuration involve that the peripheral is powered */
            pwm_poweron(dev);
            /* Enable timer and keep it in reset state */
            PWM_1_DEV->TCR = BIT0 | BIT1;
            /* Set the prescaler (F_CPU / resolution) */
            PWM_1_DEV->PR = (F_CPU/(resolution*frequency));
            /* Reset timer on MR3 */
            PWM_1_DEV->MCR = BIT10;

            /* Set PWM period */
            PWM_1_DEV->MR0 = (resolution);
            PWM_1_DEV->MR1 = (resolution);
            PWM_1_DEV->MR2 = (resolution);
            PWM_1_DEV->MR3 = (resolution)-1;

            /* Set mode for channels 0..2 */
            PWM_1_DEV->EMR |= ((mode+1) << 4);
            PWM_1_DEV->EMR |= ((mode+1) << 6);
            PWM_1_DEV->EMR |= ((mode+1) << 8);

            /* Enable PWM channels 0..2 */
            PWM_1_DEV->PWMC = BIT0 | BIT1 | BIT2;
#endif /* PWM_1_EN */
    }

    return frequency;
}

int pwm_set(pwm_t dev, int channel, unsigned int value)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            if (channel <= 2) {
                PWM_0_DEV->MR[channel] = PWM_0_DEV->MR3 - value;
            }
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            if (channel <= 2) {
                PWM_1_DEV->MR[channel] = PWM_1_DEV->MR3 - value;
            }
            break;
#endif
    }

    return 0;
}

void pwm_start(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            /* Start the counter */
            PWM_0_DEV->TCR &= ~BIT1;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            /* Start the counter */
            PWM_1_DEV->TCR &= ~BIT1;
            break;
#endif
    }
}

void pwm_stop(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            /* Stop the counter */
            PWM_0_DEV->TCR |= BIT1;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            /* Stop the counter */
            PWM_1_DEV->TCR |= BIT1;
            break;
#endif
    }
}

void pwm_poweron(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            /* Enable clock for PWM_0 */
            LPC_SYSCON->SYSAHBCLKCTRL |= PWM_0_CLK;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            /* Enable clock for PWM_1 */
            LPC_SYSCON->SYSAHBCLKCTRL |= PWM_1_CLK;
            break;
#endif
    }
}

void pwm_poweroff(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            /* Disable clock for PWM_0 */
            LPC_SYSCON->SYSAHBCLKCTRL &= ~PWM_0_CLK;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            /* Disable clock for PWM_1 */
            LPC_SYSCON->SYSAHBCLKCTRL &= ~PWM_1_CLK;
            break;
#endif
    }
}
#endif /* (PWM_0_EN || PWM_1_EN) */
