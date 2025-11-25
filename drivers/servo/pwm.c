/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Eistec AB
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_servo_pwm
 * @{
 *
 * @file
 * @brief       Servo motor driver implementation using PWM
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include "kernel_defines.h"
#include "periph/pwm.h"
#include "servo.h"
#include "test_utils/expect.h"
#include "macros/math.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Calculate the duty cycle corresponding to the given duration of the
 *          "on duration"
 * @param   pwm_freq        frequency of the PWM peripheral
 * @param   pwm_res         resolution of the PWM peripheral
 * @param   duration        duration of the "on phase" in microseconds
 *
 * @note    Scientific rounding is used
 */
static uint16_t duty_cycle(uint32_t freq, uint16_t res, uint16_t duration_us)
{
    return DIV_ROUND((uint64_t)duration_us * freq * res, US_PER_SEC);
}

int servo_init(servo_t *dev, const servo_params_t *params)
{
    memset(dev, 0, sizeof(*dev));
    const servo_pwm_params_t *pwm_params = params->pwm;
    DEBUG("[servo] trying to initialize PWM %u with frequency %u Hz "
          "and resolution %u\n",
          (unsigned)pwm_params->pwm, (unsigned)pwm_params->freq,
          (unsigned)pwm_params->res);

    /* Note: This may initialize the PWM dev over and over again if multiple
     *       servos are connected to the same PWM. But other than wasting CPU
     *       cycles, this does no harm. And it greatly simplifies the API, so
     *       we willfully accept this inefficiency here.
     */
    uint32_t freq = pwm_init(pwm_params->pwm, PWM_LEFT, pwm_params->freq,
                             pwm_params->res);
    DEBUG("[servo] initialized PWM %u with frequency %" PRIu32 " Hz\n",
          (unsigned)pwm_params->pwm, freq);

    /* assert successful initialization with frequency roughly matching
     * requested frequency. A 50 Hz MG90S servo controlled by a 100 Hz PWM
     * worked just fine for me, so we are really lax here and accept
     * everything in the range of [0.5f; 2f] */
    assert((freq != 0)
            && (freq >= pwm_params->freq - pwm_params->freq / 2U)
            && (freq <= pwm_params->freq * 2));

    if (!freq) {
        return -EIO;
    }

    dev->params = params;
    dev->min = duty_cycle(freq, pwm_params->res, params->min_us);
    dev->max = duty_cycle(freq, pwm_params->res, params->max_us);

    return 0;
}

void servo_set(servo_t *dev, uint8_t pos)
{
    const servo_params_t *par = dev->params;
    uint32_t duty = dev->max - dev->min;
    duty *= pos;
    duty >>= 8;
    duty += dev->min;
    DEBUG("[servo] setting %p to %u (%u / 255)\n",
          (void *)dev, (unsigned)duty, (unsigned)pos);
    pwm_set(par->pwm->pwm, par->pwm_chan, duty);
}
