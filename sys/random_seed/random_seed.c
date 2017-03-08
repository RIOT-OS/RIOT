/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_random
 *
 * @{
 *
 * @file
 * @brief       Generate a "unique" seed for PRNGs
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */

#include <stdio.h>

#include "random_seed.h"
#include "luid.h"
#include "xtimer.h"
#include "periph/adc.h"
#include "periph/hwrng.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

uint16_t _seed_by_adc(void)
{
    int sample;
    uint16_t val = 0;

    /* init ADC */
    if (adc_init(ADC_LINE(RANDOM_SEED_ADC_LINE)) != 0) {
        DEBUG("random_prng_seed: unable to initialize ADC_LINE(%i)\n",
                RANDOM_SEED_ADC_LINE);
        return -1;
    }

    /* use LSB of each ADC sample and shift 16
        of these LSBs to form an 8-bit integer */
    for (int iter = 0; iter < 16; iter ++) {
        /* TODO check if samples are not equal, e.g.
           pulled to high or low voltage */
        sample = adc_sample(ADC_LINE(RANDOM_SEED_ADC_LINE), RANDOM_SEED_ADC_RES);
        val |= (sample & 0x01) << (iter);
    }

    /* TODO turn off ADC ??? */
    return val;
}


uint32_t random_prng_seed(void)
{
    uint32_t res = 0;

#ifdef FEATURE_PERIPH_HWRNG

    hwrng_init();
    hwrng_read(&res, 4);
    DEBUG("random_prng_seed: hwrng as seed for PRNG %" PRIu32 "\n", res);

#else

    /* Get three bytes ID. Upper byte will be filled by
       different source */
    luid_get(&res, 3);
    DEBUG("random_prng_seed: combination of luid and ");

#ifdef FEATURE_PERIPH_ADC

    /* left shift by 16 to fill upper byte of uint32_t res
       one byte of the LUID will be overwritten*/
    res |= (_seed_by_adc() << 16);
    DEBUG("ADC as seed for PRNG %" PRIu32 "\n", res);

#else
    xtimer_ticks32_t last_wakeup = xtimer_now();

    /* left shift by 24 to fill upper byte of uint32_t res */
    res |= (last_wakeup.ticks32 << 24);
    DEBUG("xtimer as seed for PRNG %" PRIu32 "\n", res);

    puts("random_prng_seed: Attention! Seed for PRNG might be not unique");
#endif /* FEATURE_PERIPH_ADC */
#endif /* FEATURE_PERIPH_HWRNG */

    return res;
}
