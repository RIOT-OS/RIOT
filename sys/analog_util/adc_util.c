/*
 * Copyright (C) 2018 Eistec AB
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_analog_util
 * @{
 *
 * @file
 * @brief       ADC utility function implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include <inttypes.h>

#include "cpu.h"
#include "periph/adc.h"
#include "analog_util.h"
#include "assert.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Convert adc_res_t resolution setting into numeric bit count
 */
static unsigned int _adc_res_bits(adc_res_t res)
{
    switch (res) {
        case ADC_RES_6BIT:
            return 6;
        case ADC_RES_8BIT:
            return 8;
        case ADC_RES_10BIT:
            return 10;
        case ADC_RES_12BIT:
            return 12;
        case ADC_RES_14BIT:
            return 14;
        case ADC_RES_16BIT:
            return 16;
        default:
            /* Unsupported ADC resolution, modify your application to use a
             * different resolution, or add it above */
            assert(0 == 1);
            return 0;
    }
}

int32_t adc_util_map(int sample, adc_res_t res, int32_t min, int32_t max)
{
    /* Using 64 bit signed int as intermediate to prevent overflow when range
     * multiplied by sample requires more than 32 bits */
    int32_t scaled = (((int64_t)(max - min) * sample) >> _adc_res_bits(res));
    DEBUG("scaled: %" PRId32 "\n", scaled);
    return (min + scaled);
}

/* only pro adc_util_mapf if float_math is in use */
#if MODULE_FLOAT_MATH
float adc_util_mapf(int sample, adc_res_t res, float min, float max)
{
    return ((((max - min) * sample) / ((int32_t)1L << _adc_res_bits(res))) + min);
}
#endif
