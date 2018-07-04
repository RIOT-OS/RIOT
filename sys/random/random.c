/**
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
 * @ingroup sys_random
 * @{
 * @file
 *
 * @brief PRNG seeding
 *
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdint.h>

#include "log.h"
#include "luid.h"
#include "periph/cpuid.h"
#include "random.h"
#ifdef MODULE_PUF_SRAM
#include "puf_sram.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

void auto_init_random(void)
{
    uint32_t seed;
#ifdef MODULE_PUF_SRAM
    /* TODO: hand state to application? */
    if (puf_sram_state) {
        LOG_WARNING("random: PUF SEED not fresh\n");
    }
    seed = puf_sram_seed;
#elif defined (MODULE_PERIPH_CPUID)
    luid_get(&seed, 4);
#else
    LOG_WARNING("random: NO SEED AVAILABLE!\n");
    seed = RANDOM_SEED_DEFAULT;
#endif
    DEBUG("random: using seed value %u\n", (unsigned)seed);
    random_init(seed);
}

void random_bytes(uint8_t *target, size_t n)
{
    uint32_t random;
    uint8_t *random_pos = (uint8_t*)&random;
    unsigned _n = 0;

    while (n--) {
        if (! (_n++ & 0x3)) {
            random = random_uint32();
            random_pos = (uint8_t *) &random;
        }
        *target++ = *random_pos++;
    }
}

uint32_t random_uint32_range(uint32_t a, uint32_t b)
{
    assert(a < b);

    uint32_t divisor, rand_val, range = b - a;
    uint8_t range_msb = bitarithm_msb(range);

    /* check if range is a power of two */
    if (!(range & (range - 1))) {
        divisor = (1 << range_msb) - 1;
    }
    else if (range_msb < 31) {
        /* leftshift for next power of two interval */
        divisor = (1 << (range_msb + 1)) -1;
    }
    else {
        /* disable modulo operation in loop below */
        divisor = UINT32_MAX;
    }
    /* get random numbers until value is smaller than range */
    do {
        rand_val = (random_uint32() & divisor);
    } while (rand_val >= range);
    /* return random in range [a,b] */
    return (rand_val + a);
}
