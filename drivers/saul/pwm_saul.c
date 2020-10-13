/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       SAUL wrapper for PWM pins
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 *
 * @}
 */

#include "saul.h"
#include "phydat.h"
#include "periph/pwm.h"
#include "saul/periph.h"

static inline void setchan(const saul_pwm_channel_t *chan, uint16_t value)
{
    pwm_set(chan->dev,
            chan->channel,
            (chan->flags & SAUL_PWM_INVERTED) ? saul_pwm_resolution - value : value);
}

static int write_dimmer(const void *dev, phydat_t *state)
{
    const saul_pwm_dimmer_params_t *p = dev;

    setchan(&p->channel, state->val[0]);
    return 3;
}

const saul_driver_t dimmer_saul_driver = {
    .read = saul_notsup,
    .write = write_dimmer,
    .type = SAUL_ACT_DIMMER
};

static int write_rgb(const void *dev, phydat_t *state)
{
    const saul_pwm_rgb_params_t *p = dev;

    for (int i = 0; i < 3; ++i) {
        setchan(&p->channels[i], state->val[i]);
    }
    return 3;
}

const saul_driver_t rgb_saul_driver = {
    .read = saul_notsup,
    .write = write_rgb,
    .type = SAUL_ACT_LED_RGB
};
