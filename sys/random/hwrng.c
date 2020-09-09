/**
 * Copyright (C) 2020 ML!PA Consulting GmbH
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
 * @brief   Use the HWRNG as source of randomness
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "kernel_defines.h"
#include "periph/hwrng.h"
#include "random.h"

uint32_t random_uint32(void)
{
    uint32_t rnd;
    hwrng_read(&rnd, sizeof(rnd));
    return rnd;
}

void random_init(uint32_t val)
{
    (void) val;

    if (!IS_ACTIVE(MODULE_PERIPH_INIT_HWRNG)) {
        hwrng_init();
    }
}
