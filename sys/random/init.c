/**
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Freie Universität Berlin
 *               2020 Sören Tempel <tempel@uni-bremen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#include "log.h"
#include "random.h"

#ifdef MODULE_PUF_SRAM
#include "puf_sram.h"
#endif
#ifdef MODULE_PERIPH_HWRNG
#include "periph/hwrng.h"
#endif
#ifdef MODULE_PERIPH_CPUID
#include "luid.h"
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
#elif defined (MODULE_PERIPH_HWRNG)
    hwrng_read(&seed, 4);
#elif defined (MODULE_PERIPH_CPUID)
    luid_get(&seed, 4);
#else
    LOG_WARNING("random: USING COMPILE-TIME GENERATED SEED!\n");
    seed = RANDOM_SEED_DEFAULT;
#endif
    DEBUG("random: using seed value %u\n", (unsigned)seed);
    random_init(seed);
}
