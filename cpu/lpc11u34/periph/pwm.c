/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "bitarithm.h"
#include "periph/pwm.h"

/* guard file in case no PWM device is defined */
#ifdef PWM_NUMOF

static inline LPC_CTxxBx_Type *dev(pwm_t pwm)
{
    return pwm_config[pwm].dev;
}

/**
 * @note    The LPC11U34 doesn't support centerized alignements
 */
uint32_t pwm_init(pwm_t pwm, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    assert((pwm < PWM_NUMOF) && (mode != PWM_CENTER));

    /* make sure the given frequency settings are applicable */
    if (CLOCK_CORECLOCK < (res * freq)) {
        return 0;
    }

    /* setup pins */
    for (unsigned i = 0; i < PWM_CHAN_NUMOF; i++) {
        if (pwm_config[pwm].pins[i]) {
            *(pwm_config[pwm].pins[i]) = pwm_config[pwm].af;
        }
    }

    /* power on and configure the timer */
    LPC_SYSCON->SYSAHBCLKCTRL |= pwm_config[pwm].clk_bit;
    /* enable the timer and keep it in reset state */
    dev(pwm)->TCR = BIT0 | BIT1;
    /* set prescaler */
    dev(pwm)->PR = (CLOCK_CORECLOCK / (res * freq));
    /* reset timer on MR3 */
    dev(pwm)->MCR = BIT10;

    /* set PWM period */
    dev(pwm)->MR0 = res;
    dev(pwm)->MR1 = res;
    dev(pwm)->MR2 = res;
    dev(pwm)->MR3 = (res - 1);

    /* set mode for channels 0, 1, and 2 */
    dev(pwm)->EMR = (((mode + 1) << 4) | ((mode + 1) << 6) | ((mode + 1) << 8));

    /* enable channels 0, 1, and 2 */
    dev(pwm)->PWMC = (BIT0 | BIT1 | BIT2);

    return freq;
}

uint8_t pwm_channels(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    return (uint8_t)PWM_CHAN_NUMOF;
}

void pwm_set(pwm_t pwm, uint8_t channel, uint16_t value)
{
    assert((pwm < PWM_NUMOF) && (channel < PWM_CHAN_NUMOF));
    dev(pwm)->MR[channel] = dev(pwm)->MR3 - value;
}

void pwm_poweron(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    LPC_SYSCON->SYSAHBCLKCTRL |= pwm_config[pwm].clk_bit;
    dev(pwm)->TCR &= ~(BIT1);
}

void pwm_poweroff(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    dev(pwm)->TCR |= (BIT1);
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(pwm_config[pwm].clk_bit);
}

#endif /* PWM_NUMOF */
