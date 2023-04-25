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

#include "adc_ng_util.h"

int adc_ng_vcc(uint8_t adc, int16_t *dest_mv)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    assert(dest_mv);
    const adc_ng_backend_t be = adc_ng_backends[adc];
    if (be.driver->ref_vcc_idx == ADC_NG_NO_SUCH_REF) {
        return -ENOTSUP;
    }

    return adc_ng_measure_ref(adc, be.driver->ref_vcc_idx, dest_mv);
}

int adc_ng_ntc(uint8_t adc, uint8_t chan, const adc_ng_ntc_t *ntc, int16_t *temp)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    assert(ntc && temp);

    uint8_t res = adc_ng_max_res(adc);
    int16_t ref_mv = ntc->offset << 1;
    int retval = adc_ng_init(adc, chan, res, &ref_mv);
    if (retval) {
        return retval;
    }

    int16_t vin;
    retval = adc_ng_voltage(adc, &vin);
    adc_ng_off(adc);
    if (retval) {
        return retval;
    }

    int32_t tmp = vin;
    tmp -= ntc->offset;
    tmp *= (uint32_t)ntc->coefficient;
    tmp >>= 10;
    *temp = (int16_t)tmp;

    return 0;
}

int adc_ng_entropy(uint8_t adc, void *_dest, size_t size)
{
    uint8_t *dest = _dest;
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    assert(dest && size);
    const adc_ng_backend_t be = adc_ng_backends[adc];

    if (!be.driver->entropy_bits) {
        return -ENOTSUP;
    }

    uint8_t *end = dest + size;

    uint8_t res = adc_ng_max_res(adc);
    int16_t ref_mv = 0;
    int retval = adc_ng_init(adc, ADC_NG_CHAN_ENTROPY, res, &ref_mv);
    if (retval) {
        return retval;
    }

    uint16_t entropy = 0;
    unsigned entropy_bits = 0;
    const unsigned bytes_per_iteration = be.driver->entropy_bits / 8;
    const unsigned bits_per_iteration = be.driver->entropy_bits % 8;
    /* Bitmask to select only the bits_per_iteration least significant bits */
    const uint8_t entropy_mask = (1 << bits_per_iteration) - 1;
    while (dest < end) {
        union {
            int32_t s32;
            uint8_t u8[4];
        } tmp;
        retval = be.driver->single(be.handle, &tmp.s32);
        if (retval) {
            be.driver->off(be.handle);
            return retval;
        }

        for (unsigned i = 0; i < bytes_per_iteration; i++) {
            *dest++ = tmp.u8[i];
            if (dest >= end) {
                be.driver->off(be.handle);
                return 0;
            }
        }
        entropy <<= bits_per_iteration;
        entropy |= tmp.u8[0] & entropy_mask;
        entropy_bits += bits_per_iteration;
        if (entropy_bits >= 8) {
            *dest++ = (uint8_t)entropy;
            entropy >>= 8;
            entropy_bits -= 8;
        }
    }

    if (be.driver->off) {
        be.driver->off(be.handle);
    }
    return 0;
}
