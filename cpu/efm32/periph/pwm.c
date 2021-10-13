/*
 * Copyright (C) 2016-2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_pwm
 * @{
 *
 * @file
 * @brief       Low-level PWM peripheral driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include <assert.h>

#include "cpu.h"

#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/pwm.h"

#include "em_cmu.h"
#include "em_timer.h"
#include "em_timer_utils.h"

uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    /* check if device is valid */
    if (dev >= PWM_NUMOF) {
        return -1;
    }

    /* enable clocks */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(pwm_config[dev].cmu, true);

    /* calculate the prescaler by determining the best prescaler */
    uint32_t freq_timer = CMU_ClockFreqGet(pwm_config[dev].cmu);
    TIMER_Prescale_TypeDef prescaler = TIMER_PrescalerCalc(freq * res,
                                                           freq_timer);

    if (prescaler > timerPrescale1024) {
        return -2;
    }

    /* reset and initialize peripheral */
    TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;

    init.enable = false;
    init.prescale = prescaler;
    init.mode = (TIMER_Mode_TypeDef) mode;

    TIMER_Reset(pwm_config[dev].dev);
    TIMER_Init(pwm_config[dev].dev, &init);
    TIMER_TopSet(pwm_config[dev].dev, res);

    /* initialize channels */
    TIMER_InitCC_TypeDef init_channel = TIMER_INITCC_DEFAULT;

    init_channel.mode = timerCCModePWM;

    for (int i = 0; i < pwm_config[dev].channels; i++) {
        pwm_chan_conf_t channel = pwm_config[dev].channel[i];

        /* configure the pin */
        gpio_init(channel.pin, GPIO_OUT);

        /* configure pin function */
#if defined(_SILICON_LABS_32B_SERIES_0)
        pwm_config[dev].dev->ROUTE |= (channel.loc |
                                       TIMER_Channel2Route(channel.index));
#elif defined(_SILICON_LABS_32B_SERIES_1)
        pwm_config[dev].dev->ROUTELOC0 |= channel.loc;
        pwm_config[dev].dev->ROUTEPEN |= TIMER_Channel2Route(channel.index);
#endif

        /* setup channel */
        TIMER_InitCC(pwm_config[dev].dev, channel.index, &init_channel);
    }

    /* enable peripheral */
    TIMER_Enable(pwm_config[dev].dev, true);

    return freq_timer / TIMER_Prescaler2Div(prescaler) / res;
}

uint8_t pwm_channels(pwm_t dev)
{
    assert(dev < PWM_NUMOF);
    return pwm_config[dev].channels;
}

void pwm_set(pwm_t dev, uint8_t channel, uint16_t value)
{
    assert(dev < PWM_NUMOF);
    TIMER_CompareBufSet(pwm_config[dev].dev,
                        pwm_config[dev].channel[channel].index,
                        value);
}

void pwm_start(pwm_t dev)
{
    assert(dev < PWM_NUMOF);
    TIMER_Enable(pwm_config[dev].dev, true);
}

void pwm_stop(pwm_t dev)
{
    assert(dev < PWM_NUMOF);
    TIMER_Enable(pwm_config[dev].dev, false);
}

void pwm_poweron(pwm_t dev)
{
    assert(dev < PWM_NUMOF);
    CMU_ClockEnable(pwm_config[dev].cmu, true);
}

void pwm_poweroff(pwm_t dev)
{
    assert(dev < PWM_NUMOF);
    CMU_ClockEnable(pwm_config[dev].cmu, false);
}
