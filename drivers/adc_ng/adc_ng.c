/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_adc_ng
 * @{
 *
 * @file
 * @brief       Implementation of common functions of the ADC API
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "adc_ng.h"
#include "kernel_defines.h"
#include "xfa.h"

int adc_ng_select_res(uint8_t adc, uint8_t *res, uint8_t *shift)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    assert(res != NULL);
    uint8_t res_selected = *res;
    while (!adc_ng_supports_res(adc, res_selected)) {
        if (++res_selected > ADC_NG_MAX_RES) {
            return -ENOTSUP;
        }
    }

    if (shift) {
        *shift = res_selected - *res;
    }
    *res = res_selected;
    return 0;
}

int adc_ng_init(uint8_t adc, uint8_t chan, uint8_t res, int16_t *ref)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    assert(ref);
    assert((res > 0) && (res <= 32));
    const adc_ng_backend_t *be = &adc_ng_backends[adc];
    adc_ng_state_t *state = &adc_ng_states[adc];

    if (!(be->driver->res_supported & (1 << (res - 1)))) {
        return -ENOTSUP;
    }

    uint8_t idx = 0;

    if (*ref == ADC_NG_MAX_REF) {
        while (be->driver->refs[++idx]) { }
        idx--;
    }
    else {
        if ((*ref < 0) && (*ref < be->driver->refs[0])) {
            /* No reference small enough to cover the range [*ref; 0] */
            return -ERANGE;
        }
        /* Positive reference voltage: Select the lowest reference that
         * is equal to or greater than the requested reference */
        while (be->driver->refs[idx] < *ref) {
            if (!be->driver->refs[++idx]) {
                if (*ref < 0) {
                    idx--;
                    break;
                }
                /* No reference big enough to cover the range [0; ref] */
                return -ERANGE;
            }
        }
    }

    state->reference_mv = *ref = be->driver->refs[idx];
    state->resolution = res;

    return be->driver->init(be->handle, chan, res, (uint8_t)idx);
}

int16_t adc_ng_convert(uint8_t adc, int32_t sample)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    adc_ng_state_t *state = &adc_ng_states[adc];
    /* V_in = (sample * V_ref) / (2^resolution - 1) */
    int64_t vin = sample;
    vin *= state->reference_mv;
    vin /= (1 << state->resolution) - 1;
    return (int16_t)vin;
}

int adc_ng_measure_ref(uint8_t adc, uint8_t ref_idx, int16_t *dest)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    assert(dest);
    const adc_ng_backend_t *be = &adc_ng_backends[adc];
    /* It makes no sense to use the same voltage source as reference and input */
    assert(be->driver->ref_input_idx != ref_idx);

    int retval;
    int32_t sample;
    uint8_t res_max = adc_ng_max_res(adc);
    retval = be->driver->init(be->handle, ADC_NG_CHAN_FIXED_REF, res_max,
                              ref_idx);
    if (retval) {
        return retval;
    }
    retval = be->driver->single(be->handle, &sample);
    be->driver->off(be->handle);
    if (retval) {
        return retval;
    }
    /*
     * The sample s with the resolution r has the value:
     *
     *     s = (V_in * 2^r) / V_ref
     *
     * In this case we're interested in V_ref and know V_in, so:
     *
     *     V_ref = (V_in * 2^r) / s
     */
    int64_t vref = (1ULL << res_max);
    vref *= be->driver->refs[be->driver->ref_input_idx];
    vref += sample >> 1; /* <- Scientific rounding */
    vref /= sample;
    *dest = (int16_t)vref;
    return 0;
}

int adc_ng_quick(uint8_t adc, uint8_t chan, int32_t *dest)
{
    int16_t ref = ADC_NG_MAX_REF;
    int retval = adc_ng_init(adc, chan, adc_ng_max_res(adc), &ref);
    if (retval) {
        return retval;
    }
    retval = adc_ng_single(adc, dest);
    adc_ng_off(adc);
    return retval;
}

int adc_ng_voltage(uint8_t adc, int16_t *dest_mv)
{
    int32_t sample;
    int retval = adc_ng_single(adc, &sample);
    if (retval) {
        return retval;
    }

    *dest_mv = adc_ng_convert(adc, sample);
    return 0;
}
