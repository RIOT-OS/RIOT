/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Engineering-Spirit
 *               2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Aurelien Gonce <aurelien.gonce@altran.fr>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"
#include "periph/pwm.h"
#include "periph/gpio.h"

#ifdef PWM_NUMOF

#define CCMR_LEFT           (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | \
                             TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2)
#define CCMR_RIGHT          (TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | \
                             TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2M_0 | \
                             TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);

static inline TIM_TypeDef *dev(pwm_t pwm)
{
    return pwm_config[pwm].dev;
}

uint32_t pwm_init(pwm_t pwm, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    uint32_t bus_clk = periph_apb_clk(pwm_config[pwm].bus);

    /* verify parameters */
    assert((pwm < PWM_NUMOF) && ((freq * res) < bus_clk));

    /* power on the used timer */
    periph_clk_en(pwm_config[pwm].bus, pwm_config[pwm].rcc_mask);
    /* reset configuration and CC channels */
    dev(pwm)->CR1 = 0;
    dev(pwm)->CR2 = 0;
    for (int i = 0; i < TIMER_CHAN; i++) {
        dev(pwm)->CCR[i] = 0;
    }

    /* configure the used pins */
    unsigned i = 0;
    while ((i < TIMER_CHAN) && (pwm_config[pwm].chan[i].pin != GPIO_UNDEF)) {
        gpio_init(pwm_config[pwm].chan[i].pin, GPIO_OUT);
        gpio_init_af(pwm_config[pwm].chan[i].pin, pwm_config[pwm].af);
        i++;
    }

    /* configure the PWM frequency and resolution by setting the auto-reload
     * and prescaler registers */
    dev(pwm)->PSC = (bus_clk / (res * freq)) - 1;
    dev(pwm)->ARR = res - 1;

    /* set PWM mode */
    switch (mode) {
        case PWM_LEFT:
            dev(pwm)->CCMR1 = CCMR_LEFT;
            dev(pwm)->CCMR2 = CCMR_LEFT;
            break;
        case PWM_RIGHT:
            dev(pwm)->CCMR1 = CCMR_RIGHT;
            dev(pwm)->CCMR2 = CCMR_RIGHT;
            break;
        case PWM_CENTER:
            dev(pwm)->CCMR1 = 0;
            dev(pwm)->CCMR2 = 0;
            dev(pwm)->CR1 |= (TIM_CR1_CMS_0 | TIM_CR1_CMS_1);
            break;
    }

    /* enable PWM outputs and start PWM generation */
#ifdef TIM_BDTR_MOE
    dev(pwm)->BDTR = TIM_BDTR_MOE;
#endif
    dev(pwm)->CCER = (TIM_CCER_CC1E | TIM_CCER_CC2E |
                      TIM_CCER_CC3E | TIM_CCER_CC4E);
    dev(pwm)->CR1 |= TIM_CR1_CEN;

    /* return the actual used PWM frequency */
    return (bus_clk / (res * (dev(pwm)->PSC + 1)));
}

uint8_t pwm_channels(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);

    unsigned i = 0;
    while ((i < TIMER_CHAN) && (pwm_config[pwm].chan[i].pin != GPIO_UNDEF)) {
        i++;
    }
    return (uint8_t)i;
}

void pwm_set(pwm_t pwm, uint8_t channel, uint16_t value)
{
    assert((pwm < PWM_NUMOF) &&
           (channel < TIMER_CHAN) &&
           (pwm_config[pwm].chan[channel].pin != GPIO_UNDEF));

    /* norm value to maximum possible value */
    if (value > dev(pwm)->ARR) {
        value = (uint16_t)dev(pwm)->ARR;
    }
    /* set new value */
    dev(pwm)->CCR[pwm_config[pwm].chan[channel].cc_chan] = value;
}

void pwm_poweron(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    periph_clk_en(pwm_config[pwm].bus, pwm_config[pwm].rcc_mask);
    dev(pwm)->CR1 |= TIM_CR1_CEN;
}

void pwm_poweroff(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    dev(pwm)->CR1 &= ~TIM_CR1_CEN;
    periph_clk_dis(pwm_config[pwm].bus, pwm_config[pwm].rcc_mask);
}

#endif /* PWM_NUMOF */
