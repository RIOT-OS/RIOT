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

#define ENABLE_DEBUG (0)
#include "debug.h"

void auto_init_random(void)
{
    uint32_t seed;
#ifdef MODULE_PERIPH_CPUID
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
