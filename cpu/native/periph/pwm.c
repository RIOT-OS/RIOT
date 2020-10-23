/*
 * Copyright (C) 2018 Gilles DOFFE <g.doffe@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_native
 * @ingroup     drivers_periph_pwm
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Gilles DOFFE <g.doffe@gmail.com>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "periph/pwm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef PWM_NUMOF

#define NATIVE_PWM_SPEED    1000000
#define NATIVE_PWM_NB_CHAN  2
#define NATIVE_PWM_MAX      65535

typedef struct {
    uint16_t duty_cycle;
    uint8_t on;
} native_pwm_t;

native_pwm_t pwms[PWM_NUMOF][NATIVE_PWM_NB_CHAN];

uint32_t pwm_init(pwm_t pwm, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    uint8_t i = 0;

    (void) mode;
    (void) freq;
    (void) res;

    if (pwm >= PWM_NUMOF)
    {
        errno = ENODEV;
        goto pwm_init_err;
    }

    /* reset pwm channels */
    for (i = 0; i < NATIVE_PWM_NB_CHAN; i++)
    {
        pwms[pwm][i].duty_cycle = 0;
        pwms[pwm][i].on = 0;
    }

    return freq;

pwm_init_err:
    return 0;
}

uint8_t pwm_channels(pwm_t pwm)
{
    (void) pwm;
    return NATIVE_PWM_NB_CHAN;
}

void pwm_set(pwm_t pwm, uint8_t channel, uint16_t value)
{
    assert(pwm < PWM_NUMOF);

    /* set new value */
    pwms[pwm][channel].duty_cycle = value;
    DEBUG("%s pwms[%u][%u] = %u\n", __func__, pwm, channel, value);
}

void pwm_poweron(pwm_t pwm)
{
    uint8_t i = 0;

    assert(pwm < PWM_NUMOF);
    /* reset pwm channels */
    for (i = 0; i < NATIVE_PWM_NB_CHAN; i++)
        pwms[pwm][i].on = 1;
}

void pwm_poweroff(pwm_t pwm)
{
    uint8_t i = 0;

    assert(pwm < PWM_NUMOF);
    /* reset pwm channels */
    for (i = 0; i < NATIVE_PWM_NB_CHAN; i++)
        pwms[pwm][i].on = 0;
}

#endif /* PWM_NUMOF */
