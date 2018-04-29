/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_sens0
 * @{
 *
 * @file
 * @brief       Sens0 default parsing function implementation
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include "assert.h"
#include "sens0.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

int sens0_parse_raw(const sens0_output_t *out_spec, phydat_t *data, uint8_t *raw)
{
    if (out_spec->conv_func) {
        return out_spec->conv_func(out_spec, data, raw);
    }
    assert(out_spec->nelem <= PHYDAT_DIM);
    assert(out_spec->elem_size <= sizeof(int32_t));
    data->scale = out_spec->scale;
    data->unit = out_spec->unit;
    for (uint8_t k = 0; k < out_spec->nelem; ++k) {
        int32_t value = 0;
        if (out_spec->flags & SENS0_BIG_ENDIAN) {
            /* Big endian byte order */
            for (uint8_t j = 0; j < out_spec->elem_size; ++j) {
                value = (value << 8) | raw[j];
            }
        }
        else {
            /* Little endian byte order */
            for (uint8_t j = out_spec->elem_size; j > 0; --j) {
                value = (value << 8) | raw[j - 1];
            }
        }
        raw += out_spec->elem_size;
        if (out_spec->flags & SENS0_SIGNED) {
            /* sign extend */
            unsigned shift = 8 * (sizeof(value) - out_spec->elem_size);
            value = (value << shift) >> shift;
        }
        value = (value * out_spec->conv_frac_num) / out_spec->conv_frac_den;
        data->val[k] = value;
    }
    return out_spec->nelem;
}
