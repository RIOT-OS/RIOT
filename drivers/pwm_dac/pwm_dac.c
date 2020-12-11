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

#ifdef HAVE_DAC_T
#error "pwm_dac is not usable on platforms with a DAC, especially such that" \
    " define their own dac_t."
#endif

#if UINT_MAX != 0xffffffff
#error "pwm_dac requires a 32-bit default dac_t to fit all internal details" \
    " into the dac_t value."
#endif

static inline dac_t encode(pwm_t dev, uint8_t channel, uint16_t res)
{
    assert((dev & 0xffff) == dev);
    uint8_t log2_res = bitarithm_msb(res);
    assert(res == 1 << log2_res);
    return ((uint32_t)dev << 16) | ((uint32_t)log2_res << 8) | channel;
}

static inline pwm_t decode_dev(dac_t dac)
{
    return dac >> 16;
}

static inline uint8_t decode_res_bits(dac_t dac)
{
    return dac >> 8;
}

static inline uint8_t decode_channel(dac_t dac)
{
    return dac;
}

dac_t pwm_dac_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    int ret = pwm_init(dev, mode, freq, res);
    pwm_poweroff(dev);
    if (ret == 0) {
        return DAC_UNDEF;
    }
    return encode(dev, 0, res);
}

int8_t dac_init(dac_t line)
{
    dac_set(line, 0);
    pwm_poweron(decode_dev(line));
    return DAC_OK;
}

void dac_poweroff(dac_t line)
{
    pwm_poweroff(decode_dev(line));
}

void dac_poweron(dac_t line)
{
    // FIXME needs consideration for multiline PWM devices
    // FIXME we might want to check whether the line is actually valid
    pwm_poweron(decode_dev(line));
}

void dac_set(dac_t line, uint16_t value)
{
    pwm_t pwm = decode_dev(line);
    uint8_t channel = decode_channel(line);
    uint8_t resbits = decode_res_bits(line);

    pwm_set(pwm, channel, value >> (16 - resbits));
}
