/*
 * Copyright (C) 2018 Eistec AB
 *               2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_phydat
 * @{
 *
 * @file
 * @brief       Generic sensor/actuator data handling
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include "phydat.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef PHYDAT_FIT_TRADE_PRECISION_FOR_ROM
#define PHYDAT_FIT_TRADE_PRECISION_FOR_ROM 1
#endif

static const uint32_t lookup_table_positive[] = {
    327674999,
    32767499,
    3276749,
    327674,
    32767,
};

#if !(PHYDAT_FIT_TRADE_PRECISION_FOR_ROM)
static const uint32_t lookup_table_negative[] = {
    327684999,
    32768499,
    3276849,
    327684,
    32768,
};
#endif

static const uint32_t divisors[] = {
    100000,
    10000,
    1000,
    100,
    10,
};

#define LOOKUP_LEN ARRAY_SIZE(lookup_table_positive)

void phydat_fit(phydat_t *dat, const int32_t *values, unsigned int dim)
{
    assert(dim <= ARRAY_SIZE(dat->val));
    uint32_t divisor = 0;
    uint32_t max = 0;
    const uint32_t *lookup = lookup_table_positive;

    /* Get the value with the highest magnitude and the correct lookup table.
     * If PHYDAT_FIT_TRADE_PRECISION_FOR_ROM is true, the same lookup table will
     * be used for both positive and negative values. As result, -32768 will be
     * considered as out of range and scaled down. So statistically in 0.00153%
     * of the cases an unneeded scaling is performed, when
     * PHYDAT_FIT_TRADE_PRECISION_FOR_ROM is true.
     */
    for (unsigned int i = 0; i < dim; i++) {
        if (values[i] > (int32_t)max) {
            max = values[i];
#if !(PHYDAT_FIT_TRADE_PRECISION_FOR_ROM)
            lookup = lookup_table_positive;
#endif
        }
        else if (-values[i] > (int32_t)max) {
            max = -values[i];
#if !(PHYDAT_FIT_TRADE_PRECISION_FOR_ROM)
            lookup = lookup_table_negative;
#endif
        }
    }

    for (unsigned int i = 0; i < LOOKUP_LEN; i++) {
        if (max > lookup[i]) {
            divisor = divisors[i];
            dat->scale += 5 - i;
            break;
        }
    }

    if (!divisor) {
        /* No rescaling required */
        for (unsigned int i = 0; i < dim; i++) {
            dat->val[i] = values[i];
        }
        return;
    }

    /* Applying scale and add half of the divisor for correct rounding */
    uint32_t divisor_half = divisor >> 1;
    for (unsigned int i = 0; i < dim; i++) {
        if (values[i] >= 0) {
            dat->val[i] = (uint32_t)(values[i] + divisor_half) / divisor;
        }
        else {
            /* For negative integers the C standards seems to lack information
             * on whether to round down or towards zero. So using positive
             * integer division as last resort here.
             */
            dat->val[i] = -((uint32_t)((-values[i]) + divisor_half) / divisor);
        }
    }
}
