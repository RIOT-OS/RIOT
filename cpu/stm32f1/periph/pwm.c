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

/* ignore file in case no PWM devices are defined */
#if PWM_EN

#define MAX_CHANNELS                (sizeof(((pwm_conf_t*)0)->chan) / sizeof(pwm_conf_chan_t))

static inline TIM_TypeDef* tim(pwm_t dev)
{
    return pwm_config[dev].dev;
}

static inline const pwm_conf_chan_t* chan(pwm_t dev, int ch)
{
    return (pwm_config[dev].chan + ch);
}

static inline volatile uint16_t* ccr(pwm_t dev, int ch)
{
    return (&(tim(dev)->CCR1)) + (chan(dev, ch)->chan << 1);
}

static inline volatile uint32_t* apb(pwm_t dev)
{
    return (&(RCC->APB1ENR)) - (pwm_config[dev].apb - 1);
}

uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    int channels;
    uint32_t pwm_clk;
    const pwm_conf_t *conf = pwm_config + dev;

    /* check arguments */
    if (dev >= PWM_NUMOF) {
        return 0;
    }

    /* power on the timer */
    pwm_poweron(dev);

    /* calculate the clock source of the timer */
    pwm_clk = CLOCK_CORECLOCK / conf->apb;

    /* setup pins for high speed alternate function
     *
     * The pin configuration can be found on page 171:
     *  http://www.st.com/web/en/resource/technical/document/reference_manual/CD00171190.pdf
     */
    for (channels = 0; channels < MAX_CHANNELS; ++channels) {
        /* check if pin is used */
        if (!(conf->chan[channels].pin)) {
            break;
        }

        /* calculate where the MODE and CNF must be updated */
        gpio_init_af(conf->chan[channels].pin, GPIO_AF_OUT_PP);
    }

    /* configure peripheral remapping */
    if (conf->remap) {
        /* alternative function mapping can be found on page 178
         * http://www.st.com/web/en/resource/technical/document/reference_manual/CD00171190.pdf
         */
        /* timer 12 to 14 and timer 9 to 15 must be re-mapped on MAPR2 register of AFIO */
        volatile uint32_t *MAPR = &(AFIO->MAPR);
        if ((conf->dev > TIM12 && conf->dev < TIM14) || (conf->dev > TIM15 && conf->dev < TIM11)) {
            MAPR = &(AFIO->MAPR2);
        }

        /* clear re-mapping of the peripheral and set the new re-mapping */
        // TODO add support for not reset ? *MAPR &= ~(PWM_0_AFIO_MAPR_BITMASK << PWM_0_AFIO_MAPR_OFFSET);
        *MAPR |= (conf->remap);
    }

    /* reset the c/c and timer configuration register */
    conf->dev->CR1 = 0;
    conf->dev->CR2 = 0;
    for (int idx = 0; idx < channels; ++idx) {
        *ccr(dev, idx) = 0;
    }

    /* set the pre-scale and auto-reload register to matching values
     * for resolution and frequency
     */
    if (res > UINT16_MAX || (res * freq) > pwm_clk) {
        return -2;
    }
    conf->dev->PSC = (pwm_clk / (res * freq)) - 1;
    conf->dev->ARR = res - 1;
    /* calculate the actual PWM frequency */
    freq = (pwm_clk / (res * (conf->dev->PSC + 1)));

    /* set PWM mode start assuming (mode == PWM_LEFT) */
    uint16_t ccmr_val = (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
                        TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);

    if (mode == PWM_RIGHT) {
        ccmr_val |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC2M_0;
    }
    else if (mode == PWM_CENTER) {
        ccmr_val = 0;
        conf->dev->CR1 |= TIM_CR1_CMS_0 | TIM_CR1_CMS_1;
    }
    conf->dev->CCMR1 = ccmr_val;
    conf->dev->CCMR2 = ccmr_val;

    /* enable output on PWM pins */
    conf->dev->CCER = (TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E);

    /* enable PWM outputs */
    conf->dev->BDTR = TIM_BDTR_MOE;

    /* enable timer ergo the PWM generation */
    pwm_start(dev);

    return freq;
}

uint8_t pwm_channels(pwm_t dev)
{
    /* check if valid device */
    if (dev >= PWM_NUMOF) {
        return 0;
    }

    /* count available channels */
    for (int idx = 0; idx < MAX_CHANNELS; ++idx) {
        if (!(chan(dev, idx)->pin)) {
            return idx;
        }
    }

    /* all channels are used */
    return MAX_CHANNELS;
}

void pwm_set(pwm_t dev, uint8_t channel, uint16_t value)
{
    /* check if valid device */
    if (dev >= PWM_NUMOF || channel >= MAX_CHANNELS
        || !(chan(dev, channel)->pin)) {
        return;
    }

    /* norm value to maximum possible value */
    TIM_TypeDef *htim = tim(dev);
    if (value > htim->ARR) {
        value = (unsigned int) htim->ARR;
    }

    /* set the timer compare to the value */
    *ccr(dev, channel) = value;
}

void pwm_start(pwm_t dev)
{
    /* check if valid device */
    if (dev >= PWM_NUMOF) {
        return;
    }

    /* enable the timer */
    tim(dev)->CR1 |= TIM_CR1_CEN;
}

void pwm_stop(pwm_t dev)
{
    /* check if valid device */
    if (dev >= PWM_NUMOF) {
        return;
    }

    /* disable the timer */
    tim(dev)->CR1 &= ~(TIM_CR1_CEN);
}

void pwm_poweron(pwm_t dev)
{
    /* check if valid device */
    if (dev >= PWM_NUMOF) {
        return;
    }

    *apb(dev) |= (1 << pwm_config[dev].rcc);
}

void pwm_poweroff(pwm_t dev)
{
    /* check if valid device */
    if (dev >= PWM_NUMOF) {
        return;
    }

    *apb(dev) &= ~(1 << pwm_config[dev].rcc);
}

#endif /* PWM_0_EN || PWM_1_EN */
