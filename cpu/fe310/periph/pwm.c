/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @ingroup     drivers_periph_pwm
 * @{
 *
 * @file
 * @brief       PWM driver implementation for the FE310
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "periph/pwm.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define PWM_CMPSIZE     (0x4)
#define PWM_CMPWIDTH    (16)

uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    assert(dev < PWM_NUMOF);

    /* Compute PWM counter scale */
    uint8_t scale = 0;
    while ((scale < (PWM_CMPWIDTH - 1)) &&
           (freq * res) > (uint32_t)(1 << (scale + PWM_CMPWIDTH))) {
        scale++;
    }

    /* Prepare PWM counter scale and enable continuous mode */
    uint32_t config = ((scale & PWM_CFG_SCALE) | PWM_CFG_ENALWAYS);

    /* Configure wave forms */
    if (mode == PWM_CENTER) {
        config |= (PWM_CFG_CMP0CENTER | PWM_CFG_CMP1CENTER | \
                   PWM_CFG_CMP2CENTER | PWM_CFG_CMP3CENTER);
    }

    /* setup the configured channels */
    for (unsigned i = 0; i < ARRAY_SIZE(pwm_config[dev].chan); ++i) {
        if (pwm_config[dev].chan[i].pin == GPIO_UNDEF) {
            continue;
        }
        GPIO_REG(GPIO_IOF_SEL) |= (1 << pwm_config[dev].chan[i].pin);
        GPIO_REG(GPIO_IOF_EN) |= (1 << pwm_config[dev].chan[i].pin);
    }

    /* Configure and enable device */
    _REG32(pwm_config[dev].addr, PWM_CFG) = config;

    /* and return the actual configured frequency */
    return cpu_freq() / (uint32_t)(1 << (scale + PWM_CMPWIDTH));
}

uint8_t pwm_channels(pwm_t dev)
{
    assert(dev < PWM_NUMOF);
    return (uint8_t)ARRAY_SIZE(pwm_config[dev].chan);
}

void pwm_set(pwm_t dev, uint8_t channel, uint16_t value)
{
    assert((dev < PWM_NUMOF) && (channel < ARRAY_SIZE(pwm_config[dev].chan)));

    uint8_t scale = _REG32(pwm_config[dev].addr, PWM_CFG) & PWM_CFG_SCALE;
    uint32_t freq = cpu_freq() / (uint32_t)(1 << (scale + PWM_CMPWIDTH)) ;

    uint32_t cmp_reg = PWM_CMP0 + (PWM_CMPSIZE * pwm_config[dev].chan[channel].cmp);
    _REG32(pwm_config[dev].addr, cmp_reg) = value * freq;
}

void pwm_poweron(pwm_t dev)
{
    assert(dev < PWM_NUMOF);
    _REG32(pwm_config[dev].addr, PWM_CFG) |= PWM_CFG_ENALWAYS;
}

void pwm_poweroff(pwm_t dev)
{
    assert(dev < PWM_NUMOF);
    _REG32(pwm_config[dev].addr, PWM_CFG) &= ~PWM_CFG_ENALWAYS;
}
