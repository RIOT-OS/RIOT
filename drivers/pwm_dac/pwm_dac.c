/*
 * Copyright (C) 2020 Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pwm_dac
 * @{
 *
 * @file
 * @brief       DAC implementation based on a PWM device
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 * @}
 */

#include "pwm_dac.h"
#include <periph/dac.h>
#include <periph/pwm.h>
#include <assert.h>
#include <bitarithm.h>
#include <errno.h>

#include "pwm_dac_params.h"

void pwm_dac_init_all(void)
{
    for (unsigned int i = 0; i < DAC_NUMOF; ++i) {
        /* Silently ignoring intialization failure */
        pwm_dac_init(i);
    }
}

int pwm_dac_init(dac_t line)
{
    const pwm_dac_params_t *params = &pwm_dac_params[line];
    int ret = pwm_init(params->dev, params->mode, params->freq, 1 << params->resbits);
    if (ret != 0) {
        /* Discarding any details on the frequency -- if one was picked
         * successfully, it's close enough to the requested one for DAC
         * purposes */

        pwm_poweroff(params->dev);
        return 0;
    } else {
        return -EINVAL;
    }
}

int8_t dac_init(dac_t line)
{
    const pwm_dac_params_t *params = &pwm_dac_params[line];
    dac_set(line, 0);
    pwm_poweron(params->dev);
    return DAC_OK;
}

void dac_poweroff(dac_t line)
{
    const pwm_dac_params_t *params = &pwm_dac_params[line];
    pwm_poweroff(params->dev);
}

void dac_poweron(dac_t line)
{
    const pwm_dac_params_t *params = &pwm_dac_params[line];
    pwm_poweron(params->dev);
}

void dac_set(dac_t line, uint16_t value)
{
    const pwm_dac_params_t *params = &pwm_dac_params[line];
    pwm_set(params->dev, 0, value >> (16 - params->resbits));
}
