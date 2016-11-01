/*
 * Copyright (C) 2015 Engineering-Spirit
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l1
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Aurelien Gonce <aurelien.gonce@altran.fr>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "periph/pwm.h"
#include "periph_conf.h"
#include "periph/timer.h"

/* ignore file in case no PWM devices are defined */
#if (PWM_NUMOF > 0)

/**
 * @brief   Get the timer device
 */
static inline TIM_TypeDef *get_tim_dev(pwm_t tim)
{
    return timer_config[tim].dev;
}

/**
 * @brief   Get the pwm device
 */
static inline GPIO_TypeDef *get_pwm_port(pwm_t pwm)
{
    return pwm_config[pwm].port;
}

uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    GPIO_TypeDef *port = get_pwm_port(dev);
    tim_t tim = pwm_config[dev].tim;
    TIM_TypeDef *timer_dev = get_tim_dev(tim);
    uint8_t channels = pwm_channels(tim);
    uint32_t pins[channels];

    /* enable timer peripheral clock */
    pwm_poweron(tim);

    /* pins configuration */
    pins[0] = pwm_config[dev].CH0;
    if (channels > 1) {
        pins[1] = pwm_config[dev].CH1;
    }
    if (channels > 2) {
        pins[2] = pwm_config[dev].CH2;
    }
    if (channels > 3) {
        pins[3] = pwm_config[dev].CH3;
    }

    /* enable pwm peripheral */
    RCC->AHBENR |= pwm_config[dev].rcc_mask;

    /* setup pins: alternate function */
    for (int i = 0; i < channels; i++) {
        port->MODER &= ~(3 << (pins[i] * 2));
        port->MODER |= (2 << (pins[i] * 2));
        if (pins[i] < 8) {
            port->AFR[0] &= ~(0xf << (pins[i] * 4));
            port->AFR[0] |= (pwm_config[dev].AF << (pins[i] * 4));
        } else {
            port->AFR[1] &= ~(0xf << ((pins[i] - 8) * 4));
            port->AFR[1] |= (pwm_config[dev].AF << ((pins[i] - 8) * 4));
        }
    }

    /* Reset C/C and timer configuration register */
    switch (channels) {
        case 4:
            timer_dev->CCR4 = 0;
            /* Fall through */
        case 3:
            timer_dev->CCR3 = 0;
            timer_dev->CR2 = 0;
            /* Fall through */
        case 2:
            timer_dev->CCR2 = 0;
            /* Fall through */
        case 1:
            timer_dev->CCR1 = 0;
            timer_dev->CR1 = 0;
            break;
    }

    /* set prescale and auto-reload registers to matching values for resolution and frequency */
    if (res > 0xffff || (res * freq) > timer_config[tim].freq) {
        return 0;
    }
    timer_dev->PSC = (timer_config[tim].freq / (res * freq)) - 1;
    timer_dev->ARR = res - 1;
    /* calculate the actual PWM frequency */
    freq = (timer_config[tim].freq / (res * (timer_dev->PSC + 1)));

    /* set PWM mode */
    switch (mode) {
        case PWM_LEFT:
            timer_dev->CCMR1 = (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
                                TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);
            if (channels > 2) {
                timer_dev->CCMR2 = (TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
                                    TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2);
            }
            break;
        case PWM_RIGHT:
            timer_dev->CCMR1 = (TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
                                TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);
            if (channels > 2) {
                timer_dev->CCMR2 = (TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
                                    TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2);
            }
            break;
        case PWM_CENTER:
            timer_dev->CCMR1 = 0;
            if (channels > 2) {
                timer_dev->CCMR2 = 0;
            }
            timer_dev->CR1 |= (TIM_CR1_CMS_0 | TIM_CR1_CMS_1);
            break;
    }

    /* enable output on PWM pins */
    timer_dev->CCER = (TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E);

    /* enable timer ergo the PWM generation */
    pwm_start(tim);

    return freq;
}

uint8_t pwm_channels(pwm_t dev) {
    return (timer_config[dev].channels);
}

void pwm_set(pwm_t dev, uint8_t channel, uint16_t value)
{
    tim_t tim = pwm_config[dev].tim;
    TIM_TypeDef *timer_dev = get_tim_dev(tim);

    if (channel >= pwm_channels(tim)) {
        return;
    }

    /* norm value to maximum possible value */
    if (value > timer_dev->ARR) {
        value = (uint16_t) timer_dev->ARR;
    }

    switch (channel) {
        case 0:
            timer_dev->CCR1 = value;
            break;
        case 1:
            timer_dev->CCR2 = value;
            break;
        case 2:
            timer_dev->CCR3 = value;
            break;
        case 3:
            timer_dev->CCR4 = value;
            break;
        default:
            break;
    }
}

void pwm_start(pwm_t dev)
{
    get_tim_dev(dev)->CR1 |= TIM_CR1_CEN;
}

void pwm_stop(pwm_t dev)
{
    get_tim_dev(dev)->CR1 &= ~(TIM_CR1_CEN);
}

void pwm_poweron(pwm_t dev)
{
    periph_clk_en(timer_config[dev].bus, timer_config[dev].rcc_mask);
}

void pwm_poweroff(pwm_t dev)
{
    periph_clk_dis(timer_config[dev].bus, timer_config[dev].rcc_mask);
}

#endif /* PWM_NUMOF > 0*/
