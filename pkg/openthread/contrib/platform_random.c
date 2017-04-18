/*
 * Copyright (C) 2016 José Ignacio Alamos
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */

#include <stdint.h>

#include <openthread/platform/random.h>
#include "random.h"

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "periph/cpuid.h"

/* init random */
void ot_random_init(void)
{
#ifdef CPUID_LEN
    char cpu_id[CPUID_LEN];
    cpuid_get(cpu_id);
    uint32_t seed = 0;
    for (int i = 0; i < (int) CPUID_LEN; i++) {
        seed += cpu_id[i];
    }
    random_init(seed);
#else
    #error "CPU not supported (current CPU doesn't provide CPUID, required for entropy)"
#endif
}

/* OpenThread will call this to get a random number */
uint32_t otPlatRandomGet(void)
{
    uint32_t rand_val = random_uint32();

    DEBUG("otPlatRandomGet: %i\n", (int) rand_val);
    return rand_val;
}

/** @} */
