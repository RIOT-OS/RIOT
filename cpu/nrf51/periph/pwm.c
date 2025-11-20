/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf51
 * @ingroup     drivers_periph_pwm
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "periph/gpio.h"
#include "periph/pwm.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define PWM_PS_MAX          (9U)
#define PWM_PPI_CHANNELS    ((1 << PWM_PPI_A) | (1 << PWM_PPI_B))
#ifndef PWM_PERCENT_VAL
    #define PWM_PERCENT_VAL (1U)
#endif

static const uint32_t divtable[10] = {
    (CLOCK_CORECLOCK >> 0),
    (CLOCK_CORECLOCK >> 1),
    (CLOCK_CORECLOCK >> 2),
    (CLOCK_CORECLOCK >> 3),
    (CLOCK_CORECLOCK >> 4),
    (CLOCK_CORECLOCK >> 5),
    (CLOCK_CORECLOCK >> 6),
    (CLOCK_CORECLOCK >> 7),
    (CLOCK_CORECLOCK >> 8),
    (CLOCK_CORECLOCK >> 9),
};

static uint32_t init_data[2];

uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    assert(dev == 0 && ((mode == PWM_LEFT) || (mode == PWM_RIGHT)));

    /* reset and configure the timer */
    PWM_TIMER->POWER = 1;
    PWM_TIMER->TASKS_STOP = 1;
    PWM_TIMER->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
    PWM_TIMER->MODE = TIMER_MODE_MODE_Timer;
    PWM_TIMER->TASKS_CLEAR = 1;

    /* calculate and set prescaler */
    uint32_t timer_freq = freq * res;
    uint32_t lower = (timer_freq - (PWM_PERCENT_VAL * (timer_freq / 100)));
    uint32_t upper = (timer_freq + (PWM_PERCENT_VAL * (timer_freq / 100)));
    for (uint32_t ps = 0; ps <= (PWM_PS_MAX + 1); ps++) {
        if (ps == (PWM_PS_MAX + 1)) {
            DEBUG("[pwm] init error: resolution or frequency not supported\n");
            return 0;
        }
        if ((divtable[ps] < upper) && (divtable[ps] > lower)) {
            PWM_TIMER->PRESCALER = ps;
            timer_freq = divtable[ps];
            break;
        }
    }

    /* reset timer compare events */
    PWM_TIMER->EVENTS_COMPARE[0] = 0;
    PWM_TIMER->EVENTS_COMPARE[1] = 0;
    /* init timer compare values */
    PWM_TIMER->CC[0] = 1;
    PWM_TIMER->CC[1] = res;

    /* configure PPI Event (set compare values and pwm width) */
    if (mode == PWM_LEFT) {
        NRF_GPIOTE->CONFIG[PWM_GPIOTE_CH] = (GPIOTE_CONFIG_MODE_Task     |
                                             (PWM_PIN << 8)              |
                                             GPIOTE_CONFIG_POLARITY_Msk  |
                                             GPIOTE_CONFIG_OUTINIT_Msk);
    }
    else if (mode == PWM_RIGHT) {
        NRF_GPIOTE->CONFIG[PWM_GPIOTE_CH] = (GPIOTE_CONFIG_MODE_Task     |
                                             (PWM_PIN << 8)              |
                                             GPIOTE_CONFIG_POLARITY_Msk);
    }

    /* configure PPI Channels (connect compare-event and gpiote-task) */
    NRF_PPI->CH[PWM_PPI_A].EEP = (uint32_t)(&PWM_TIMER->EVENTS_COMPARE[0]);
    NRF_PPI->CH[PWM_PPI_B].EEP = (uint32_t)(&PWM_TIMER->EVENTS_COMPARE[1]);

    NRF_PPI->CH[PWM_PPI_A].TEP =
        (uint32_t)(&NRF_GPIOTE->TASKS_OUT[PWM_GPIOTE_CH]);
    NRF_PPI->CH[PWM_PPI_B].TEP =
        (uint32_t)(&NRF_GPIOTE->TASKS_OUT[PWM_GPIOTE_CH]);

    /* enable configured PPI Channels */
    NRF_PPI->CHENSET = PWM_PPI_CHANNELS;

    /* shortcut to reset Counter after CC[1] event */
    PWM_TIMER->SHORTS = TIMER_SHORTS_COMPARE1_CLEAR_Msk;

    /* start pwm with value '0' */
    pwm_set(dev, 0, 0);

    DEBUG("Timer frequency is set to %" PRIu32 "\n", timer_freq);

    return (uint32_t)(timer_freq / res);
}

void pwm_set(pwm_t dev, uint8_t channel, uint16_t value)
{
#ifdef NDEBUG
    (void)dev;
    (void)channel;
#endif
    assert((dev == 0) && (channel == 0));

    /*
     * make sure duty cycle is set at the beginning of each period
     * ensure to stop the timer as soon as possible
     */
    PWM_TIMER->TASKS_STOP = 1;
    PWM_TIMER->EVENTS_COMPARE[1] = 0;
    PWM_TIMER->SHORTS = TIMER_SHORTS_COMPARE1_STOP_Msk;
    PWM_TIMER->TASKS_START = 1;

    /*
     * waiting for the timer to stop
     * This loop generates heavy load. This is not optimal therefore a local
     * sleep function should be implemented.
     */
    while (PWM_TIMER->EVENTS_COMPARE[1] == 0) {};

    /*
     * checking pwm alignment first
     * and guarding if duty cycle is 0% / 100%
     */
    if (NRF_GPIOTE->CONFIG[PWM_GPIOTE_CH] & GPIOTE_CONFIG_OUTINIT_Msk) {
        if (value == 0) {
            if (PWM_TIMER->CC[0] != 0) {
                NRF_GPIOTE->TASKS_OUT[PWM_GPIOTE_CH] = 1;
            }

            NRF_PPI->CHENCLR = PWM_PPI_CHANNELS;
            PWM_TIMER->CC[0] = 0;
        } else {
            if (PWM_TIMER->CC[0] == 0) {
                NRF_GPIOTE->TASKS_OUT[PWM_GPIOTE_CH] = 1;
            }
            if (value >= PWM_TIMER->CC[1]) {
                NRF_PPI->CHENCLR = PWM_PPI_CHANNELS;
                PWM_TIMER->CC[0] = PWM_TIMER->CC[1];
            } else {
                NRF_PPI->CHENSET = PWM_PPI_CHANNELS;
                PWM_TIMER->CC[0] = value;
            }
        }
    } else {
        if (value >= PWM_TIMER->CC[1]) {
            if (PWM_TIMER->CC[0] != PWM_TIMER->CC[1]) {
                NRF_GPIOTE->TASKS_OUT[PWM_GPIOTE_CH] = 1;
            }
            NRF_PPI->CHENCLR = PWM_PPI_CHANNELS;
            PWM_TIMER->CC[0] = PWM_TIMER->CC[1];
        } else {
            if (PWM_TIMER->CC[0] == PWM_TIMER->CC[1]) {
                NRF_GPIOTE->TASKS_OUT[PWM_GPIOTE_CH] = 1;
            }
            if (value == 0) {
                NRF_PPI->CHENCLR = PWM_PPI_CHANNELS;
                PWM_TIMER->CC[0] = 0;
            } else {
                NRF_PPI->CHENSET = PWM_PPI_CHANNELS;
                PWM_TIMER->CC[0] = PWM_TIMER->CC[1] - value;
            }
        }
    }

    /* reconfigure pwm to standard mode */
    PWM_TIMER->TASKS_CLEAR = 1;
    PWM_TIMER->SHORTS = TIMER_SHORTS_COMPARE1_CLEAR_Msk;
    PWM_TIMER->TASKS_START = 1;
}

uint8_t pwm_channels(pwm_t dev)
{
#ifdef NDEBUG
    (void)dev;
#endif
    assert(dev == 0);
    return 1;
}

void pwm_poweron(pwm_t dev)
{
    assert(dev == 0);

    /*
     * reinit pwm with correct alignment
     */
    if (NRF_GPIOTE->CONFIG[PWM_GPIOTE_CH] & GPIOTE_CONFIG_OUTINIT_Msk) {
        pwm_init(dev, PWM_LEFT, init_data[1], (init_data[0] >> 16));
    } else {
        pwm_init(dev, PWM_RIGHT, init_data[1], (init_data[0] >> 16));
    }

    /*
     * reset dutycycle
     */
    pwm_set(dev, 0, (init_data[0] & 0xffff));

}

void pwm_poweroff(pwm_t dev)
{
    assert(dev == 0);

    PWM_TIMER->TASKS_STOP = 1;

    /*
     * power off function ensures that the inverted CC[0] is cached correctly
     * when right aligned
     */
    if (((NRF_GPIOTE->CONFIG[PWM_GPIOTE_CH] & GPIOTE_CONFIG_OUTINIT_Msk) == 0) &
        (PWM_TIMER->CC[1] != PWM_TIMER->CC[0]) &
        (PWM_TIMER->CC[0] != 0)) {
            init_data[0] = ((PWM_TIMER->CC[1] << 16) |
                            (PWM_TIMER->CC[1] - PWM_TIMER->CC[0]));
    } else {
        init_data[0] = ((PWM_TIMER->CC[1] << 16) | PWM_TIMER->CC[0]);
    }

    init_data[1] = (divtable[PWM_TIMER->PRESCALER] / PWM_TIMER->CC[1]);

    /*
     * make sure the gpio is set to '0' while power is off
     */
    pwm_set(dev, 0, 0);

    PWM_TIMER->POWER = 0;
}
