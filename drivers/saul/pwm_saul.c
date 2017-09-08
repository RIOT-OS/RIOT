/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_saul
 * @{
 *
 * @file
 * @brief       SAUL wrapper for direct access to PWM pins
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "saul/periph.h"
#include "phydat.h"
#include "periph/pwm.h"

static int set_pwm(const void *dev, phydat_t *value)
{
    const saul_pwm_params_t *params = *((const saul_pwm_params_t **)dev);
    uint32_t freq;
    uint16_t res;

    if (value->val[1] > 0) {
        freq = value->val[1];
    }
    else {
        freq = params->freq;
    }

    if (value->val[2] > 0) {
        res = value->val[2];
    }
    else {
        res = params->res;
    }

    pwm_init(params->line, params->mode, freq, res);
    pwm_set(params->line, params->channel, value->val[0]);

    /* PWM unit contains (in order) duty cycle, frequency and resolution */
    value->unit = UNIT_PWM;
    value->scale = 0;
    return 1;
}

const saul_driver_t pwm_saul_driver = {
    .read = saul_notsup,
    .write = set_pwm,
    .type = SAUL_ACT_ANY,
};
