/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f1
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "periph/pwm.h"
#include "periph_conf.h"

/* ignore file in case no PWM devices are defined */
#if PWM_0_EN || PWM_1_EN || 1

int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution)
{
    TIM_TypeDef *tim = NULL;
    GPIO_TypeDef *port = NULL;
    uint32_t pins[PWM_MAX_CHANNELS];
    uint32_t pwm_clk = 0;
    int channels = 0;

    pwm_poweron(dev);

    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            tim = PWM_0_DEV;
            port = PWM_0_PORT;
            pins[0] = PWM_0_PIN_CH0;
#if (PWM_0_CHANNELS > 1)
            pins[1] = PWM_0_PIN_CH1;
#endif
#if (PWM_0_CHANNELS > 2)
            pins[2] = PWM_0_PIN_CH2;
#endif
#if (PWM_0_CHANNELS > 3)
            pins[3] = PWM_0_PIN_CH3;
#endif
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
#if (PWM_1_CHANNELS > 1)
            pins[1] = PWM_1_PIN_CH1;
#endif
#if (PWM_1_CHANNELS > 2)
            pins[2] = PWM_1_PIN_CH2;
#endif
#if (PWM_1_CHANNELS > 3)
            pins[3] = PWM_1_PIN_CH3;
#endif
            af = PWM_1_PIN_AF;
            channels = PWM_1_CHANNELS;
            pwm_clk = PWM_1_CLK;
            PWM_1_PORT_CLKEN();
            break;
#endif
    }

    /* setup pins for high speed alternate function */
    for (int i = 0; i < channels; i++) {
        /* calculate where the MODE and CNF must be updated */
        port->CR[pins[i] >= 8] &= ~(0xfl << (4 * (pins[i] - ((pins[i] >= 8) * 8))));
        port->CR[pins[i] >= 8] |= (0xbl << (4 * (pins[i] - ((pins[i] >= 8) * 8))));
    }

#if PWM_0_PERIPH_AF || PWM_1_PERIPH_AF
    /* timer 12 to 14 and timer 9 to 15 must be re-mapped on MAPR2 register of AFIO */
    __IO uint32_t *MAPR = &(AFIO->MAPR);
    if ((tim > TIM12 && tim < TIM14) || (tim > TIM15 && tim < TIM11)) {
        MAPR = &(AFIO->MAPR2);
    }
#endif

#if PWM_0_PERIPH_AF
    /* clear re-mapping of the peripheral and set the new re-mapping */
    *MAPR &= ~(PWM_0_AFIO_MAPR_BITMASK << PWM_0_AFIO_MAPR_OFFSET);
    *MAPR |= (PWM_0_PERIPH_AF << PWM_0_AFIO_MAPR_OFFSET);
#endif

#if PWM_1_PERIPH_AF
    /* clear re-mapping of the peripheral and set the new re-mapping */
    *MAPR &= ~(PWM_1_AFIO_MAPR_BITMASK << PWM_1_AFIO_MAPR_OFFSET);
    *MAPR |= (PWM_1_PERIPH_AF << PWM_1_AFIO_MAPR_OFFSET);
#endif

    /* reset the c/c and timer configuration register */
    switch (channels) {
        case 4:
            tim->CCR4 = 0;
        /* no break */
        case 3:
            tim->CCR3 = 0;
            tim->CR2 = 0;
        /* no break */
        case 2:
            tim->CCR2 = 0;
        /* no break */
        case 1:
            tim->CCR1 = 0;
            tim->CR1 = 0;
            break;
    }

    /* set the prescale and auto-reload register to matching values for resolution and frequency */
    if (resolution > 0xffff || (resolution * frequency) > pwm_clk) {
        return -2;
    }
    tim->PSC = (pwm_clk / (resolution * frequency)) - 1;
    tim->ARR = resolution - 1;

    /* set PWM mode */
    switch (mode) {
        case PWM_LEFT:
            tim->CCMR1 = (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
                          TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);
            tim->CCMR2 = (TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
                          TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2);
            break;

        case PWM_RIGHT:
            tim->CCMR1 = (TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
                          TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);
            tim->CCMR2 = (TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
                          TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2);
            break;

        case PWM_CENTER:
            tim->CCMR1 = 0;
            tim->CCMR2 = 0;
            tim->CR1 |= (TIM_CR1_CMS_0 | TIM_CR1_CMS_1);
            break;
    }

    /* enable output on PWM pins */
    tim->CCER = (TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E);

    /* enable PWM outputs */
    tim->BDTR = TIM_BDTR_MOE;

    /* enable timer ergo the PWM generation */
    pwm_start(dev);

    return frequency;
}

int pwm_set(pwm_t dev, int channel, unsigned int value)
{
    TIM_TypeDef *tim = NULL;

    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            tim = PWM_0_DEV;
            if (channel >= PWM_0_CHANNELS) {
                return -1;
            }
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            tim = PWM_1_DEV;
            if (channel >= PWM_1_CHANNELS) {
                return -1;
            }
            break;
#endif
    }

    /* norm value to maximum possible value */
    if (value > tim->ARR) {
        value = (unsigned int) tim->ARR;
    }

    /* set the PWM value to the channel comperator */
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
            return -1;
    }

    return 0;
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

#endif /* PWM_0_EN || PWM_1_EN */
