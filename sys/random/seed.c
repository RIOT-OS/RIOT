/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include <stdint.h>
#include <time.h>

#include "hwtimer.h"
#include "periph/cpuid.h"
#include "periph/random.h"
#include "periph/timer.h"

#include "random.h"

#if defined(MODULE_PERIPH_CPUID) && !defined(MODULE_PERIPH_RANDOM)
static inline uint32_t cpuid_to_uint32()
{
    uint8_t cpuid[CPUID_ID_LEN], uint8_t shift = 0;
    uint32_t res = 0;

    cpuid_get(cpuid);
    for (unsigned int i = 0; i < CPUID_ID_LEN; i++) {
        res |= cpuid[i] << (sizeof(uint8_t) * shift);
        shift = (shift + 1) & 0xff; /* (shift + 1) % 4  */
    }

    return res;
}
#endif

uint32_t genrand_get_seed(void)
{
    uint32_t seed;

#if   defined(MODULE_PERIPH_RANDOM)
    random_init();
    random_read((char *)(&seed), sizeof(seed));
    random_poweroff();
#elif defined(MODULE_PERIPH_TIMER) && defined(MODULE_PERIPH_CPUID)
    seed = timer_read(TIMER_0) ^ cpuid_to_uint32();
#elif defined(MODULE_PERIPH_CPUID)
    seed = cpuid_to_uint32();
#elif defined(MODULE_PERIPH_TIMER)
    seed = timer_read(TIMER_0);
#else
    seed = (uint32_t)hwtimer_now();
#endif

    return seed;
}

/** @} */
