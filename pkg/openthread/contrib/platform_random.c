/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread random platform abstraction
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @}
 */

#include <stdint.h>

#include "openthread/platform/random.h"
#include "periph/cpuid.h"
#include "random.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* init random */
void ot_random_init(void)
{
#ifdef CPUID_LEN
    char cpu_id[CPUID_LEN];
    cpuid_get(cpu_id);
    uint32_t seed = 0;
    for (unsigned i = 0; i < CPUID_LEN; i++) {
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

otError otPlatRandomGetTrue(uint8_t *aOutput, uint16_t aOutputLength)
{
    for (uint16_t index = 0; index < aOutputLength; index++) {
        aOutput[index] = 0;
        uint32_t rand_val = random_uint32();
        aOutput[index] = (uint8_t) rand_val;
    }
    return OT_ERROR_NONE;
}
