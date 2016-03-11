/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32f3
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "periph_conf.h"

/* guard file in case no PWM device is defined */
#if (PWM_0_EN || PWM_1_EN)

/* pull the PWM header inside the guards for now. Guards will be removed on
 * adapting this driver implementation... */
#include "periph/pwm.h"

uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    TIM_TypeDef *tim = NULL;
    GPIO_TypeDef *port = NULL;
    uint32_t pins[PWM_MAX_CHANNELS];
    uint32_t af;
    int channels;
    uint32_t pwm_clk;

    pwm_poweron(dev);

    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            tim = PWM_0_DEV;
            port = PWM_0_PORT;
            pins[0] = PWM_0_PIN_CH0;
            pins[1] = PWM_0_PIN_CH1;
            pins[2] = PWM_0_PIN_CH2;
            pins[3] = PWM_0_PIN_CH3;
            af = PWM_0_PIN_AF;
            channels = PWM_0_CHANNELS;
            pwm_clk = PWM_0_CLK;
            PWM_0_PORT_CLKEN();
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            tim = PWM_1_DEV;
            port = PWM_1_PORT;
            pins[0] = PWM_1_PIN_CH0;
            pins[1] = PWM_1_PIN_CH1;
            pins[2] = PWM_1_PIN_CH2;
            pins[3] = PWM_1_PIN_CH3;
            af = PWM_1_PIN_AF;
            channels = PWM_1_CHANNELS;
            pwm_clk = PWM_1_CLK;
            PWM_1_PORT_CLKEN();
            break;
#endif
        default:
            pwm_poweroff(dev);
            return 0;
    }

    /* setup pins: alternate function */
    for (int i = 0; i < channels; i++) {
        port->MODER &= ~(3 << (pins[i] * 2));
        port->MODER |= (2 << (pins[i] * 2));
        if (pins[i] < 8) {
            port->AFR[0] &= ~(0xf << (pins[i] * 4));
            port->AFR[0] |= (af << (pins[i] * 4));
        }
        else {
            port->AFR[1] &= ~(0xf << ((pins[i] - 8) * 4));
            port->AFR[1] |= (af << ((pins[i] - 8) * 4));
        }
    }

    /* reset timer configuration registers */
    tim->CR1 = 0;
    tim->CR2 = 0;
    tim->CCMR1 = 0;
    tim->CCMR2 = 0;

    /* set prescale and auto-reload registers to matching values for resolution and frequency */
    if ((res > 0xffff) || ((res * freq) > pwm_clk)) {
        return 0;
    }
    tim->PSC = (pwm_clk / (res * freq)) - 1;
    tim->ARR = res - 1;
    freq = (pwm_clk / (res * (tim->PSC + 1)));

    /* set PWM mode */
    switch (mode) {
        case PWM_LEFT:
            tim->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
                           TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);
            tim->CCMR2 |= (TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
                           TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2);
            break;
        case PWM_RIGHT:
            tim->CCMR1 |= (TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
                           TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);
            tim->CCMR2 |= (TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
                           TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2);
            break;
        case PWM_CENTER:
            tim->CR1 |= (TIM_CR1_CMS_0 | TIM_CR1_CMS_1);
            break;
    }

    /* enable output on PWM pins */
    tim->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E);

    /* enable PWM generation */
    pwm_start(dev);

    return freq;
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
    TIM_TypeDef *tim = NULL;

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
            return;
    }

    switch (channel) {
        case 0:
            tim->CCR1 = value;
            break;
        case 1:
            tim->CCR2 = value;
            break;
        case 2:
            tim->CCR3 = value;
            break;
        case 3:
            tim->CCR4 = value;
            break;
        default:
            return;
    }
}

void pwm_start(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PWM_0_DEV->CR1 |= TIM_CR1_CEN;
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PWM_1_DEV->CR1 |= TIM_CR1_CEN;
            break;
#endif
    }
}

void pwm_stop(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PWM_0_DEV->CR1 &= ~(TIM_CR1_CEN);
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PWM_1_DEV->CR1 &= ~(TIM_CR1_CEN);
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

#endif /* (PWM_0_EN || PWM_1_EN) */
