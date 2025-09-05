/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "bitarithm.h"

/**
 * Find factor and shiftback such that for each value entry in the phydat, the
 * resulting PWM duty cycle would be (value * factor) >> shiftback.
 *
 * This also makes the maximum legal input value for that input (which allows
 * clipping the input to a value that doesn't wrap during that multiplication,
 * or just to err out). If future versions of this take the scale into account,
 * they will adjust the maximum accordingly.
 */
static int extract_scaling(const phydat_t *state, int *factor, int *shiftback, int *max)
{
    if (state->scale != 0) {
        return -ECANCELED;
    }

    /** Number of bits i by which we can shift the calculation (value * (255 << i)/100) >> i
     * to get a better result than value * 2 (which would otherwise happen in integers) */
    int shift100 = bitarithm_msb(INT_MAX) - bitarithm_msb(saul_pwm_resolution);

    switch (state->unit) {
    case UNIT_UNDEF:
    case UNIT_NONE:
        *factor = 1;
        *shiftback = 0;
        *max = saul_pwm_resolution;
        break;
    case UNIT_BOOL:
        *factor = saul_pwm_resolution;
        *shiftback = 0;
        *max = 1;
        break;
    case UNIT_PERCENT:
        *factor = ((int)saul_pwm_resolution << shift100) / 100;
        *shiftback = shift100;
        *max = 100;
        break;
    default:
        return -ECANCELED;
    }

    return 0;
}

static inline void setchan(const saul_pwm_channel_t *chan, uint16_t value)
{
    pwm_set(chan->dev,
            chan->channel,
            (chan->flags & SAUL_PWM_INVERTED) ? saul_pwm_resolution - value : value);
}

static int write_dimmer(const void *dev, const phydat_t *state)
{
    const saul_pwm_dimmer_params_t *p = dev;

    int factor, shiftback, max;

    int err = extract_scaling(state, &factor, &shiftback, &max);
    if (err < 0) {
        return err;
    }

    if (state->val[0] < 0 || state->val[0] > max) {
        return -ECANCELED;
    }

    setchan(&p->channel, (state->val[0] * factor) >> shiftback);
    return 3;
}

const saul_driver_t dimmer_saul_driver = {
    .read = saul_read_notsup,
    .write = write_dimmer,
    .type = SAUL_ACT_DIMMER
};

static int write_rgb(const void *dev, const phydat_t *state)
{
    const saul_pwm_rgb_params_t *p = dev;

    int factor, shiftback, max;

    int err = extract_scaling(state, &factor, &shiftback, &max);
    if (err < 0) {
        return err;
    }

    for (int i = 0; i < 3; ++i) {
        if (state->val[i] < 0 || state->val[i] > max) {
            return -ECANCELED;
        }
    }

    for (int i = 0; i < 3; ++i) {
        setchan(&p->channels[i], (state->val[i] * factor) >> shiftback);
    }
    return 3;
}

const saul_driver_t rgb_saul_driver = {
    .read = saul_read_notsup,
    .write = write_rgb,
    .type = SAUL_ACT_LED_RGB
};
