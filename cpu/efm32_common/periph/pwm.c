/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32_common
 * @{
 *
 * @file
 * @brief       Low-level PWM peripheral driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "cpu.h"

#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/pwm.h"

#include "em_cmu.h"
#include "em_timer.h"

/* guard file in case no PWM device is defined */
#if PWM_NUMOF

static const uint32_t channel_to_route[] = {
#ifdef _SILICON_LABS_32B_PLATFORM_1
    TIMER_ROUTE_CC0PEN,
    TIMER_ROUTE_CC1PEN,
    TIMER_ROUTE_CC2PEN
#else
    TIMER_ROUTEPEN_CC0PEN,
    TIMER_ROUTEPEN_CC1PEN,
    TIMER_ROUTEPEN_CC2PEN,
    TIMER_ROUTEPEN_CC3PEN
#endif
};

static const TIMER_Prescale_TypeDef prescalers[] = {
    timerPrescale1,
    timerPrescale2,
    timerPrescale4,
    timerPrescale8,
    timerPrescale16,
    timerPrescale32,
    timerPrescale64,
    timerPrescale128,
    timerPrescale256,
    timerPrescale512,
    timerPrescale1024,
};

int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution)
{
    /* check if device is valid */
    if (dev >= PWM_NUMOF) {
        return -1;
    }

    /* enable clocks */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(pwm_config[dev].cmu, true);

    /* calculate the prescaler by determining the best prescaler */
    uint32_t actual_freq = CMU_ClockFreqGet(pwm_config[dev].cmu);
    TIMER_Prescale_TypeDef prescaler = timerPrescale1;

    for (int i = 0; i < 11; i++) {
        if ((actual_freq / (1 << i)) < (frequency * resolution)) {
            /* if first prescaler doesn't work, then no pwm frequency fits */
            if (i == 0) {
                return -2;
            }
            else {
                prescaler = prescalers[i - 1];
                actual_freq = actual_freq / (1 << (i - 1));
                break;
            }
        }
    }

    /* initialize the timer */
    TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;

    init.enable = false;
    init.prescale = prescaler;

    TIMER_Reset(pwm_config[dev].dev);
    TIMER_Init(pwm_config[dev].dev, &init);

    /* configure the period */
    TIMER_TopSet(pwm_config[dev].dev, resolution);

    /* initialize all channels */
    TIMER_InitCC_TypeDef ccInit = TIMER_INITCC_DEFAULT;

    ccInit.mode = timerCCModePWM;

    for (int i = 0; i < pwm_config[dev].channels; i++) {
        uint8_t index = pwm_config[dev].channel_offset + i;

        gpio_init(pwm_channel_config[index].pin, GPIO_DIR_OUT, GPIO_NOPULL);

        /* setup timer channel */
        TIMER_InitCC(pwm_config[dev].dev,
                     pwm_channel_config[index].index, &ccInit);
        TIMER_CompareBufSet(pwm_config[dev].dev,
                            pwm_channel_config[index].index, 0);

        /* enable pin output */
#ifdef _SILICON_LABS_32B_PLATFORM_1
        pwm_config[dev].dev->ROUTE |= (channel_to_route[pwm_channel_config[index].index] | pwm_channel_config[index].loc);
#else
        pwm_config[dev].dev->ROUTEPEN |= channel_to_route[pwm_channel_config[index].index];
        pwm_config[dev].dev->ROUTELOC0 |= pwm_channel_config[index].loc;
#endif
    }

    /* enable timer */
    TIMER_Enable(pwm_config[dev].dev, true);

    return actual_freq / resolution;
}

int pwm_set(pwm_t dev, int channel, unsigned int value)
{
    if (channel >= pwm_config[dev].channels) {
        return -1;
    }

    uint8_t index = pwm_config[dev].channel_offset + channel;

    TIMER_CompareBufSet(
        pwm_config[dev].dev, pwm_channel_config[index].index, value);

    return 0;
}

void pwm_start(pwm_t dev)
{
    TIMER_Enable(pwm_config[dev].dev, true);
}

void pwm_stop(pwm_t dev)
{
    TIMER_Enable(pwm_config[dev].dev, false);
}

void pwm_poweron(pwm_t dev)
{
    CMU_ClockEnable(pwm_config[dev].cmu, true);
}

void pwm_poweroff(pwm_t dev)
{
    CMU_ClockEnable(pwm_config[dev].cmu, false);
}

#endif /* PWM_NUMOF */
