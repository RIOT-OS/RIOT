/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Implementations of conversion routines for CPU internal analog signals
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>

#include "board.h"
#include "phydat.h"
#include "periph/adc.h"
#include "periph/adc_conv.h"
#include "analog_util.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief ADC low side potential
 *
 * An ADC reading of zero means that the sampled voltage is close to or less than this value.
 */
static int vrefl = BOARD_ADC_VREFL_DEFAULT;

/**
 * @brief ADC reference voltage high potential
 *
 * An ADC reading of max means that the sampled voltage is close to or greater than this value
 */
static int vrefh = BOARD_ADC_VREFH_DEFAULT;

/**
 * @brief ADC unit scale exponent
 *
 * e.g. -3 for millivolts
 */
static int vref_scale = BOARD_ADC_VREF_SCALE_DEFAULT;

int adc_conv_volts(int sample, adc_res_t res)
{
    /* Convert to voltage */
    /* sample is in the range 0..max, where 0 means the sampled voltage was in
     * the range [vrefl, vrefl + LSB), and max means the sampled voltage was in
     * the range [vrefh - LSB, vrefh) */
    return adc_util_map(sample, res, vrefl, vrefh);
}

void adc_conv_set_vrefh(int new_vrefh)
{
    vrefh = new_vrefh;
}

void adc_conv_set_vrefl(int new_vrefl)
{
    vrefl = new_vrefl;
}

void adc_conv_set_scale(int new_scale)
{
    vref_scale = new_scale;
}

#if MODULE_SAUL_ADC
int adc_conv_saul_adc_volts(const saul_adc_params_t *params, phydat_t *res)
{
    int sample = res->val[0];
    if (params->res == ADC_RES_16BIT) {
        /* Assume single-ended reading for now, clear sign bits */
        sample &= 0xffff;
    }
    int converted = adc_conv_volts(sample, params->res);
    DEBUG("vref: %de%d\n", vrefh, vref_scale);
    DEBUG("conv: %d\n", converted);
    res->unit = UNIT_V;
    res->scale = vref_scale;
    phydat_fit(res, converted, 0, 0);
    DEBUG("res: %d\n", res->val[0]);
    return 1;
}
#endif
