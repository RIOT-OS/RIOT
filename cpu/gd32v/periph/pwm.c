/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Engineering-Spirit
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_gd32v
 * @ingroup     drivers_periph_pwm
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Aurelien Gonce <aurelien.gonce@altran.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"
#include "periph/pwm.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include <stdio.h>

#define ENABLE_DEBUG 0
#include "debug.h"

#define TIM_CHCTL0_CH0COMCT_0   (0x1U << TIMER0_CHCTL0_Output_CH0COMCTL_Pos)
#define TIM_CHCTL0_CH0COMCT_1   (0x2U << TIMER0_CHCTL0_Output_CH0COMCTL_Pos)
#define TIM_CHCTL0_CH0COMCT_2   (0x4U << TIMER0_CHCTL0_Output_CH0COMCTL_Pos)

#define TIM_CHCTL0_CH1COMCT_0   (0x1U << TIMER0_CHCTL0_Output_CH1COMCTL_Pos)
#define TIM_CHCTL0_CH1COMCT_1   (0x2U << TIMER0_CHCTL0_Output_CH1COMCTL_Pos)
#define TIM_CHCTL0_CH1COMCT_2   (0x4U << TIMER0_CHCTL0_Output_CH1COMCTL_Pos)

#define CHCTL0_MODE0    (TIM_CHCTL0_CH0COMCT_1 | TIM_CHCTL0_CH0COMCT_2 | \
                         TIM_CHCTL0_CH1COMCT_1 | TIM_CHCTL0_CH1COMCT_2)

#define CHCTL0_MODE1    (TIM_CHCTL0_CH0COMCT_0 | TIM_CHCTL0_CH0COMCT_1 | \
                         TIM_CHCTL0_CH0COMCT_2 | TIM_CHCTL0_CH1COMCT_0 | \
                         TIM_CHCTL0_CH1COMCT_1 | TIM_CHCTL0_CH1COMCT_2)

static inline TIMER_Type *dev(pwm_t pwm)
{
    return pwm_config[pwm].dev;
}

uint32_t pwm_init(pwm_t pwm, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    uint32_t timer_clk = periph_apb_clk(pwm_config[pwm].bus) * 2;

    /* in PWM_CENTER mode the counter counts up and down at each period
     * so the resolution had to be divided by 2 */
    res *= (mode == PWM_CENTER) ? 2 : 1;

    assert(pwm < PWM_NUMOF);
    if ((freq * res) > timer_clk) {
        DEBUG("[pwm] Requested PWM frequency %" PRIu32 " Hz is too large. "
              "Reducing to %" PRIu32 " Hz per API contract\n",
              freq, timer_clk / res);
        freq = timer_clk / res;
    }

    /* power on the used timer */
    periph_clk_en(pwm_config[pwm].bus, pwm_config[pwm].rcu_mask);
    /* reset configuration and CC channels */
    dev(pwm)->CTL0 = 0;
    dev(pwm)->CTL1 = 0;
    for (unsigned i = 0; i < TIMER_CHANNEL_NUMOF; ++i) {
        TIMER_CHANNEL(pwm, i) = (mode == PWM_RIGHT) ? res : 0;
    }

    /* remap the timer to the configured pins (F1 only) */
    AFIO->PCF0 |= pwm_config[pwm].remap;

    /* configure the used pins */
    unsigned i = 0;
    while ((i < TIMER_CHANNEL_NUMOF) && (pwm_config[pwm].chan[i].pin != GPIO_UNDEF)) {
        gpio_init(pwm_config[pwm].chan[i].pin, GPIO_OUT);
        gpio_init_af(pwm_config[pwm].chan[i].pin, pwm_config[pwm].af);
        i++;
    }

    /* configure the PWM frequency and resolution by setting the auto-reload
     * and prescaler registers */
    dev(pwm)->PSC = (timer_clk / (res * freq)) - 1;
    dev(pwm)->CAR = (mode == PWM_CENTER) ? (res / 2) : res - 1;

    /* set PWM mode */
    switch (mode) {
    case PWM_LEFT:
        dev(pwm)->CHCTL0_Output = CHCTL0_MODE0;
        dev(pwm)->CHCTL1_Output = CHCTL0_MODE0;
        break;
    case PWM_RIGHT:
        dev(pwm)->CHCTL0_Output = CHCTL0_MODE1;
        dev(pwm)->CHCTL1_Output = CHCTL0_MODE1;
        /* duty cycle should be reversed */
        break;
    case PWM_CENTER:
        dev(pwm)->CHCTL0_Output = CHCTL0_MODE0;
        dev(pwm)->CHCTL1_Output = CHCTL0_MODE0;
        /* center-aligned mode 3 */
        dev(pwm)->CTL0 |= TIMER0_CTL0_CAM_Msk;
        break;
    }

    /* enable PWM outputs and start PWM generation */
#ifdef TIM_BDTR_MOE
    dev(pwm)->BDTR = TIM_BDTR_MOE;
#endif
    dev(pwm)->CHCTL2 = (TIMER0_CHCTL2_CH0EN_Msk | TIMER0_CHCTL2_CH1EN_Msk |
                        TIMER0_CHCTL2_CH2EN_Msk | TIMER0_CHCTL2_CH3EN_Msk);
    dev(pwm)->CTL0 |= TIMER0_CTL0_CEN_Msk;

    /* return the actual used PWM frequency */
    return (timer_clk / (res * (dev(pwm)->PSC + 1)));
}

uint8_t pwm_channels(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);

    unsigned i = 0;
    while ((i < TIMER_CHANNEL_NUMOF) && (pwm_config[pwm].chan[i].pin != GPIO_UNDEF)) {
        i++;
    }
    return (uint8_t)i;
}

void pwm_set(pwm_t pwm, uint8_t channel, uint16_t value)
{
    assert((pwm < PWM_NUMOF) &&
           (channel < TIMER_CHANNEL_NUMOF) &&
           (pwm_config[pwm].chan[channel].pin != GPIO_UNDEF));

    /* norm value to maximum possible value */
    if (value > dev(pwm)->CAR + 1) {
        value = (uint16_t)dev(pwm)->CAR + 1;
    }

    if (dev(pwm)->CHCTL0_Output == CHCTL0_MODE1) {
        /* reverse the value */
        value = (uint16_t)dev(pwm)->CAR + 1 - value;
    }

    /* set new value */
    TIMER_CHANNEL(pwm, pwm_config[pwm].chan[channel].cc_chan) = value;
}

void pwm_poweron(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    periph_clk_en(pwm_config[pwm].bus, pwm_config[pwm].rcu_mask);
    dev(pwm)->CTL0 |= TIMER0_CTL0_CEN_Msk;
}

void pwm_poweroff(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    dev(pwm)->CTL0 &= ~TIMER0_CTL0_CEN_Msk;
    periph_clk_dis(pwm_config[pwm].bus, pwm_config[pwm].rcu_mask);
}
